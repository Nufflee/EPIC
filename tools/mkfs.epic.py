#!/usr/bin/env python3

import os
import sys
import struct
import math
import time

FS_TYPE_DIRECTORY = 0
FS_TYPE_FILE = 1
SECTOR_SIZE = 512

"""c
  typedef struct directory_entry
  {
    uint16_t name_length;
    uint16_t children_count;
    char *name;
  } __attribute__((packed)) directory_entry;
"""
class DirectoryEntry:
  def __init__(self, name: str, children_count: int):
    self.name = name
    self.children_count = children_count

"""c
  typedef struct file_entry
  {
    uint16_t name_length;
    uint32_t file_size;
    uint32_t start_sector;
    char *name;
  } __attribute__((packed)) file_entry;
"""
class FileEntry:
  def __init__(self, name: str, file_size: int, start_sector: int, path: str):
    self.name = name
    self.file_size = file_size
    self.start_sector = start_sector
    self.path = path

def entrify_dir(path):
  entries = []

  for e in os.scandir(path):
    if e.is_dir():
      entries.append(DirectoryEntry(e.name, len(os.listdir(os.path.join(path, e.name)))))
      entries.extend(entrify_dir(os.path.join(path, e.name)))
    else:
      entries.append(FileEntry(e.name, e.stat().st_size, 0, e.path))

  return entries

def sizeof_entry(entry):
  if isinstance(entry, DirectoryEntry):
    return 4 + len(entry.name) + 1
  elif isinstance(entry, FileEntry):
    return 10 + len(entry.name) + 1
  else:
    assert False, f'Unknown entry type \'{type(entry)}\'.'

def serialize_entry(entry):
  buffer = b''

  if isinstance(entry, DirectoryEntry):
    buffer += struct.pack('B', FS_TYPE_DIRECTORY)
    buffer += struct.pack('H', len(entry.name))
    buffer += struct.pack('H', entry.children_count)
    buffer += bytes(entry.name, 'ascii')
  elif isinstance(entry, FileEntry):
    buffer += struct.pack('B', FS_TYPE_FILE)
    buffer += struct.pack('H', len(entry.name))
    buffer += struct.pack('I', entry.file_size)
    buffer += struct.pack('I', entry.start_sector)
    buffer += bytes(entry.name, 'ascii')
  else:
    assert False, f'Unknown entry type \'{type(entry)}\'.'

  return buffer

def bytes_to_sectors(b):
  return math.ceil(b / SECTOR_SIZE)

def align_end_to_sector(buffer):
  return buffer + (bytes_to_sectors(len(buffer)) * SECTOR_SIZE - len(buffer)) * b'\x00'

def serialize_file_contents(entry, offset):
  with open(entry.path, 'rb') as file:
    contents = file.read()

    entry.start_sector = bytes_to_sectors(offset)

    return align_end_to_sector(contents)

def main():
  if len(sys.argv) < 2 or len(sys.argv) > 4:
    print('usage: mkfs.epic <directory> [output] [-f]')

    return

  path = sys.argv[1]
  output = 'disk.img'

  force = '-f' in sys.argv

  if (not force and len(sys.argv) == 3) or len(sys.argv) == 4:
    output = sys.argv[2]

  if not os.path.exists(path):
    print(f'Input directory \'{path}\' doesn\'t exist!')

    return

  if os.path.exists(output) and not force:
    print(f'Output file \'{output}\' already exists! Use \'-f\' to override it.')

    return

  start_time = time.time()

  entries = []

  entries.append(DirectoryEntry('/', len(os.listdir(path))))
  entries.extend(entrify_dir(path))

  buffer = b''
  header_size = sum([sizeof_entry(e) for e in entries])

  offset = header_size
  file_contents_buffer = b''

  for e in entries:
    if isinstance(e, FileEntry):
      file_buffer = serialize_file_contents(e, offset)
      offset += len(file_buffer)

      file_contents_buffer += file_buffer

  buffer += struct.pack('H', bytes_to_sectors(header_size))
  buffer += b''.join([serialize_entry(e) for e in entries])

  buffer = align_end_to_sector(buffer)

  with open(output, 'wb+') as file:
    bytes_written = file.write(buffer + file_contents_buffer)

    print(f'{len(entries)} fs entries epicly written to \'{output}\' in {time.time() - start_time} sec ({bytes_written} bytes).')

main()