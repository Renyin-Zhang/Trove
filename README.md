# Trove: A Command-line Utility for Indexing and Searching File Content

## Introduction
Trove is a command-line utility written in C11 that is designed to build and search a compressed index of file content. In a digital world where the storage of numerous files is a norm, finding required information can be daunting. Trove simplifies this process by creating an index of your files and providing a search feature to find content within those files.

## Building the Project

The project can be compiled using the provided Makefile:
```
make
```

This will produce an executable called `trove`.

## Program Usage

Trove can be invoked in two ways:

1. Search for a word in the indexed files:
```
./trove [-f trovefile] word
```

2. Build, remove, or update an index:
```
./trove [-f trovefile] [-b  |  -r  |  -u] [-l length] filelist
```

## Command-line Options

* `-b`: Build a new trove-file from the contents of a filelist. The new trove-file will replace an existing one of the same name.
* `-f trovefile`: Provide the name of the trove-file to be built or searched. If the -f option is not provided, the default name of the trove-file is /tmp/trove.
* `-l length`: Only words of at least the indicated length should be added to the trove-file. `length` must be a positive integer. If the -l option is not provided, the default length is 4.
* `-r`: If any of the files from the filelist appear in the trove-file, remove all of their information from the trove-file.
* `-u`: Update the trove-file with the contents of all files in the filelist. If the information from any of the files already exists in the trove-file, then that (old) information is first removed.

## Trove-file Format

* Filenames stored in the trove-file are absolute pathnames.
* To reduce the size of the trove-file, each indexed word and each absolute filename is stored only once.
* The trove-file is a text-file.

## Note
The default name of the trove-file is /tmp/trove, and the default minimum length of each indexed word is 4. Each trove-file is stored on, and read from, disk using the `/usr/bin/gzip` and `/usr/bin/zcat` utilities.
