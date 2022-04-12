#!/usr/bin/env python3
#
# Usage: copy_data_files.py SOURCE_DIRECTORY DESTINATION_DIRECTORY
#
# This script copies files from the argv[1] directory to the argv[2] directory.
# It also checks timestamps to ensure that only files that have changed are
# copied over.
#
# Note that this script does not automatically delete files.
# It only creates or overwrites files.
#

import sys
import os
import shutil

# Parse the inputs...
if len(sys.argv) != 3:
    raise ValueError(
        "copy_data_files.py needs two arguments: "
        "A source directory and a destination directory.\n"
        f"It was given {len(sys.argv) - 1} arguments: {sys.argv}")

input_directory = sys.argv[1]
output_directory = sys.argv[2]

# Say hello...
print(f'Copying data files from "{input_directory}" to "{output_directory}"...')

# Copy the files...
changed_file_count = 0
directory_count = 0


def copy_file(source, dest):
    global changed_file_count
    changed_file_count += 1
    shutil.copy2(source, dest)


def copy_directory_contents(source_dir, dest_dir):
    global directory_count
    directory_count += 1

    for file in os.listdir(source_dir):
        source_path = os.path.join(source_dir, file)
        destination_path = os.path.join(dest_dir, file)

        if os.path.isdir(source_path):
            if not os.path.exists(destination_path):
                os.mkdir(destination_path)
            copy_directory_contents(source_path, destination_path)

        elif not os.path.exists(destination_path):
            copy_file(source_path, destination_path)

        else:
            # Only overwrite when the source file is newer to prevent pointless
            # overwrites.
            source_mod_time = os.path.getmtime(source_path)
            destination_mod_time = os.path.getmtime(destination_path)

            if source_mod_time > destination_mod_time:
                copy_file(source_path, destination_path)


copy_directory_contents(input_directory, output_directory)

# Say goodbye...
if changed_file_count == 0:
    print('All data files are up to date. Nothing was copied. '
          f'(searched {directory_count} '
          f'folder{"s" if directory_count > 1 else ""})')
else:
    # This is a bit overcomplicated because I wanted to correctly use plurals
    print(f'Copied {changed_file_count} modified '
          f'file{"s" if changed_file_count > 1 else ""} '
          f'(searched {directory_count} '
          f'folder{"s" if directory_count > 1 else ""})')
