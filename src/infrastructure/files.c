#include "stdafx.h"
#include "files.h"


int compareFileInfoForSort(const void* a, const void* b) {
    FileInfo** fia = (FileInfo**)a;
    FileInfo** fib = (FileInfo**)b;
    const char* ap = (*fia)->path;
    const char* bp = (*fib)->path;
    return strcmp(ap, bp);
}

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    FileInfo* GetFileSystemInformation(char* path) {
        WIN32_FILE_ATTRIBUTE_DATA faData;
        bool exists = GetFileAttributesExA(path, GetFileExInfoStandard, &faData);
        if (!exists) {
            return NULL;
        }

        FileInfo* fi = malloc(sizeof(FileInfo));
        fi->path = malloc(sizeof(char) * (strlen(path) + 1));
        strcpy(fi->path, path);
        fi->isDirectory = faData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
        fi->children = NULL;
        if (fi->isDirectory) {
            size_t pathLen = strlen(fi->path);
            if ('/' != fi->path[pathLen-1]) {
                char* newPath = malloc(sizeof(char) * (pathLen + 2));
                sprintf(newPath, "%s/", fi->path);
                free(fi->path);
                fi->path = newPath;
            }

            char searchPath[MAX_PATH];
            sprintf(searchPath, "%s*", fi->path);

            FileInfo** childFileList = NULL;
            FileInfo** childDirList = NULL;

            WIN32_FIND_DATA fd;
            HANDLE findHandle = FindFirstFile(searchPath, &fd);
            do {
                if ((strcmp(fd.cFileName, ".") == 0) || (strcmp(fd.cFileName, "..") == 0)) {
                    continue;
                }
                char* childPath = malloc(sizeof(char) * (strlen(path) + strlen(fd.cFileName) + 2));
                sprintf(childPath, "%s/%s", path, fd.cFileName);

                FileInfo* childInfo = GetFileSystemInformation(childPath);
                if (childInfo->isDirectory) {
                    arrpush(childDirList, childInfo);
                }
                else {
                    arrpush(childFileList, childInfo);
                }
            } while (FindNextFile(findHandle, &fd));

            qsort(childDirList, arrlen(childDirList), sizeof(FileInfo*), compareFileInfoForSort);
            qsort(childFileList, arrlen(childFileList), sizeof(FileInfo*), compareFileInfoForSort);

            for (int i=0; i < arrlen(childDirList); i++) {
                arrpush(fi->children, childDirList[i]);
            }
            for (int i=0; i < arrlen(childFileList); i++) {
                arrpush(fi->children, childFileList[i]);
            }
            arrfree(childDirList);
            arrfree(childFileList);
        }

        return fi;
    }
#else
    #include <dirent.h>
	#include <unistd.h>
    #include <sys/stat.h>

    FileInfo* GetFileSystemInformation(char* path) {
        struct stat faData;
        int status = stat(path, &faData);

        if (status != 0) {
            return NULL;
        }

        FileInfo* fi = malloc(sizeof(FileInfo));
        fi->path = malloc(sizeof(char) * (strlen(path) + 1));
        strcpy(fi->path, path);

        fi->isDirectory = S_ISDIR(faData.st_mode);
        fi->children = NULL;
        if (fi->isDirectory) {
            size_t pathLen = strlen(fi->path);
            if ('/' != fi->path[pathLen-1]) {
                char* newPath = malloc(sizeof(char) * (pathLen + 2));
                sprintf(newPath, "%s/", fi->path);
                free(fi->path);
                fi->path = newPath;
            }

            FileInfo** childFileList = NULL;
            FileInfo** childDirList = NULL;

            DIR *dirHandle = opendir(fi->path);
            struct dirent *entry;
            while ((entry = readdir(dirHandle))) {
                if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
                    continue;
                }
                char* childPath = malloc(sizeof(char) * (strlen(path) + strlen(entry->d_name) + 2));
                sprintf(childPath, "%s/%s", path, entry->d_name);

                FileInfo* childInfo = GetFileSystemInformation(childPath);
                if (childInfo->isDirectory) {
                    arrpush(childDirList, childInfo);
                }
                else {
                    arrpush(childFileList, childInfo);
                }
            }
            closedir(dirHandle);

            qsort(childDirList, arrlen(childDirList), sizeof(FileInfo*), compareFileInfoForSort);
            qsort(childFileList, arrlen(childFileList), sizeof(FileInfo*), compareFileInfoForSort);

            for (int i=0; i < arrlen(childDirList); i++) {
                arrpush(fi->children, childDirList[i]);
            }
            for (int i=0; i < arrlen(childFileList); i++) {
                arrpush(fi->children, childFileList[i]);
            }
            arrfree(childDirList);
            arrfree(childFileList);
        }

        return fi;
    }
#endif // defined(_WIN32)

void FileInfoFree(FileInfo* fi) {
    if (fi->isDirectory) {
        for (int i=0; i < arrlen(fi->children); i++) {
            FileInfoFree(fi->children[i]);
        }
        arrfree(fi->children);
    }
    free(fi->path);
    free(fi);
}

void PrintFileInfo(FileInfo* fi, int depth) {
    printf("%*s%s\n", depth*2, " ", fi->path);
    if (fi->isDirectory) {
        for (int i=0; i < arrlen(fi->children); i++) {
            PrintFileInfo(fi->children[i], depth+1);
        }
    }
}
