#include <stddef.h>
#include <stdbool.h>
#include <libk/string.h>
#include <libk/assert.h>
#include <libk/math.h>
#include "kmalloc.h"
#include "ata.h"
#include "epicfs.h"

size_t epicfs_parse_directory_entry(u8 *buffer, fs_entry *root_entry);
size_t epicfs_parse_file_entry(u8 *buffer, fs_entry *root_entry);

// TODO: Add some sort of EPICFS magic.
fs_entry *epicfs_parse_from_ata(size_t sector_offset)
{
  u16 header_size = (u16)*ata_read(sector_offset, 2);

  ASSERT(header_size > 0);

  u8 *header_buffer = ata_read(sector_offset, header_size * 512) + 2;

  return epicfs_parse(header_buffer);
}

fs_entry *epicfs_parse(u8 *buffer)
{
  fs_entry *root_entry = kmalloc(sizeof(fs_entry));

  epicfs_parse_directory_entry(buffer, root_entry);

  return root_entry;
}

size_t epicfs_parse_file_entry(u8 *buffer, fs_entry *result)
{
  file_entry *entry = kmalloc(sizeof(file_entry));
  file_header header;
  u8 *current = buffer + 1;

  memcpy(&header, current, sizeof(file_header));
  current += sizeof(file_header);

  char *name = kmalloc(header.name_length);

  memcpy(name, current, header.name_length);
  current += header.name_length;

  entry->header = header;
  entry->name = name;

  result->type = FS_TYPE_FILE;
  result->file = entry;
  result->children = NULL;

  return current - buffer;
}

size_t epicfs_parse_directory_entry(u8 *buffer, fs_entry *result)
{
  directory_entry *entry = kmalloc(sizeof(directory_entry));
  directory_header header;
  u8 *current = buffer + 1;

  memcpy(&header, current, sizeof(directory_header));
  current += sizeof(directory_header);

  char *name = kmalloc(header.name_length);

  memcpy(name, current, header.name_length);
  current += header.name_length;

  entry->header = header;
  entry->name = name;

  u16 children_count = entry->header.children_count;

  result->type = FS_TYPE_DIRECTORY;
  result->directory = entry;
  result->children = kalloc(children_count, sizeof(fs_entry));

  for (int i = 0; i < children_count; i++)
  {
    if (*current == FS_TYPE_DIRECTORY)
    {
      current += epicfs_parse_directory_entry(current, &result->children[i]);
    }
    else if (*current == FS_TYPE_FILE)
    {
      current += epicfs_parse_file_entry(current, &result->children[i]);
    }
    else
    {
      ASSERT_ALWAYS("Unknown FS type!");
    }
  }

  return current - buffer;
}

void epicfs_pretty_print_file_entry(fs_entry entry, char *path, int indent)
{
  ASSERT(entry.type == FS_TYPE_FILE);

  u32 file_size = entry.file->header.file_size;

  //char *contents = kmalloc(file_size);

  //memcpy(contents, buffer + entry.file->header.start_sector * 512, file_size);

  serial_port_printf(COM1, "%*sfile ('%s%s'): %d bytes\n", indent * 2, "", path, entry.file->name, file_size);
}

void epicfs_pretty_print_directory_entry(fs_entry entry, char *parent_path, int indent)
{
  ASSERT(entry.type == FS_TYPE_DIRECTORY);

  directory_entry *dir = entry.directory;
  u16 children_count = dir->header.children_count;

  serial_port_printf(COM1, "%*sdir ('%s%s'): %d children\n", indent * 2, "", parent_path, dir->name, children_count);

  char *path = kmalloc(string_length(parent_path) + dir->header.name_length + 1);

  string_concat(path, parent_path);
  string_concat(path, dir->name);

  if (string_compare(parent_path, ""))
  {
    string_concat(path, "/");
  }

  for (int i = 0; i < children_count; i++)
  {
    fs_entry child = entry.children[i];

    if (child.type == FS_TYPE_DIRECTORY)
    {
      epicfs_pretty_print_directory_entry(child, path, indent + 1);
    }
    else if (child.type == FS_TYPE_FILE)
    {
      epicfs_pretty_print_file_entry(child, path, indent + 1);
    }
    else
    {
      ASSERT(!"Unknown FS entry type!");
    }
  }
}

size_t epicfs_create_directory_entry(char *name, u16 children_count, directory_entry **entry)
{
  *entry = kmalloc(sizeof(directory_entry));

  (*entry)->header.name_length = strlen(name);
  (*entry)->header.children_count = children_count;
  (*entry)->name = name;

  return sizeof(directory_header) + (*entry)->header.name_length + 1;
}

size_t epicfs_create_file_entry(char *name, u16 file_size, file_entry **entry)
{
  *entry = kmalloc(sizeof(file_entry));

  (*entry)->header.name_length = strlen(name);
  (*entry)->header.file_size = file_size;
  (*entry)->header.start_sector = 0;
  (*entry)->name = name;

  return sizeof(file_header) + (*entry)->header.file_size + (*entry)->header.name_length + 1;
}

size_t epicfs_write_fs_header(fs_header *header, u8 *buffer)
{
  u8 *offset = buffer;

  memcpy(offset, (u8 *)header, sizeof(fs_header));
  offset += sizeof(fs_header);

  return offset - buffer;
}

size_t epicfs_write_directory_entry(directory_entry *entry, u8 *buffer)
{
  u8 *offset = buffer;

  memset(offset, FS_TYPE_DIRECTORY, 1);
  offset += 1;
  memcpy(offset, (u8 *)&entry->header, sizeof(directory_header));
  offset += sizeof(directory_header);
  memcpy(offset, entry->name, entry->header.name_length);
  offset += entry->header.name_length;

  return offset - buffer;
}

size_t epicfs_write_file_entry(file_entry *entry, u8 *buffer)
{
  u8 *offset = buffer;

  memset(offset, FS_TYPE_FILE, 1);
  offset += 1;
  memcpy(offset, (u8 *)&entry->header, sizeof(file_header));
  offset += sizeof(file_header);
  memcpy(offset, entry->name, entry->header.name_length);
  offset += entry->header.name_length;

  return offset - buffer;
}

size_t epicfs_write_file_contents(u8 *contents, file_entry *entry, u8 *buffer, size_t offset)
{
  size_t start_sector = divide_and_round_up(offset, 512);
  size_t start_sector_offset = start_sector * 512;

  entry->header.start_sector = start_sector;

  memcpy(buffer + start_sector_offset, contents, entry->header.file_size);

  return (start_sector_offset - offset) + entry->header.file_size;
}