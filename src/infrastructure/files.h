#pragma once

// can be expanded with metadata if needed
typedef struct sFileInfo {
    char* path;
    bool isDirectory;
    struct sFileInfo** children;
} FileInfo;

// quick-and-dirty; not meant for fancy file system work or traversal
// assumes every handle is either a file or a directory, which is not
//   always true, but close enough to always for our purposes.
// will break on very deep nested directories, pathological symlinks,
//   moving files during the listing, etc.
FileInfo* GetFileSystemInformation(char* path);

void FileInfoFree(FileInfo* fi);
