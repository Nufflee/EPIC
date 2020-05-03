#include <stddef.h>

typedef enum fs_type
{
  FS_TYPE_DIRECTORY,
  FS_TYPE_FILE
} fs_type;

typedef struct fs_header
{
  u16 size;
} __attribute__((packed)) fs_header;

typedef struct file_header
{
  u16 name_length;
  u32 file_size;
  u32 start_sector;
} __attribute__((packed)) file_header;

typedef struct file_entry
{
  file_header header;
  char *name;
} __attribute__((packed)) file_entry;

typedef struct directory_header
{
  u16 name_length;
  u16 children_count;
} __attribute__((packed)) directory_header;

typedef struct directory_entry
{
  directory_header header;
  char *name;
} __attribute__((packed)) directory_entry;

typedef struct fs_entry
{
  fs_type type;

  union {
    directory_entry *directory;
    file_entry *file;
  };

  struct fs_entry *children;
} __attribute__((packed)) fs_entry;

fs_entry *epicfs_parse_from_ata(size_t sector_offset);
fs_entry *epicfs_parse(u8 *buffer);

u8 *epicfs_read_file(char *path, fs_entry *root);

size_t epicfs_parse_file_entry(u8 *buffer, fs_entry *result);
size_t epicfs_parse_directory_entry(u8 *buffer, fs_entry *result);

void epicfs_pretty_print_file_entry(fs_entry entry, char *path, int indent);
void epicfs_pretty_print_directory_entry(fs_entry entry, char *parent_path, int indent);

size_t epicfs_create_directory_entry(char *name, u16 children_count, directory_entry **entry);
size_t epicfs_create_file_entry(char *name, u16 file_size, file_entry **entry);

size_t epicfs_write_fs_header(fs_header *header, u8 *buffer);
size_t epicfs_write_directory_entry(directory_entry *entry, u8 *buffer);
size_t epicfs_write_file_entry(file_entry *entry, u8 *buffer);
size_t epicfs_write_file_contents(u8 *contents, file_entry *entry, u8 *buffer, size_t offset);