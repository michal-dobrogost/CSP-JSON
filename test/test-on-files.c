#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../common/io.h"
#include "test-on-files.h"

static int ignoreFilename(const char* filename) {
  size_t filenameLen = strlen(filename);
  if (filenameLen < 5 || strcmp(".json", filename + filenameLen - 5) != 0) {
    return 1;
  }
  return 0;
}

static int isRegularFile(const char* path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

static int isDirectory(const char* path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}


/* Return 0 on success */
int testOnFile(const char* testName, char* path, testCspJsonT fn) {
  printf("%s(%s)... ", testName, path);
  fflush(stdout);
  char* json = NULL;
  size_t jsonLen = 0;
  FILE* f = fopen(path, "r");
  if (!f) {
    printf("FAIL: fopen\n");
    return 1;
  }
  int stat = readAll(f, &json, &jsonLen);
  fclose(f);
  if (stat != 0) {
    printf("FAIL: readAll returned %d\n", stat);
    return stat;
  }

  stat = (*fn)(json, jsonLen);
  if (stat != 0) { return stat; }

  printf("OK\n");
  return 0;
}

int testOnFilesInDir(const char* testName, char* dirPath, testCspJsonT fn) {
  DIR* d = opendir(dirPath);
  if (!d) { return 1; }
  struct dirent* dir = NULL;
  while (NULL != (dir = readdir(d))) {
    const size_t subPathSize = 4096;
    char* subPath = (char*) malloc(subPathSize);
    snprintf(subPath, subPathSize, "%s/%s", dirPath, dir->d_name);
    if (isRegularFile(subPath)) {
      if (ignoreFilename(dir->d_name)) {
        printf("%s(%s)... SKIP\n", testName, subPath);
        continue;
      }
      int stat = testOnFile(testName, subPath, fn);
      if (stat != 0) { return stat; }
    }
    else if (strlen(dir->d_name) > 0 && dir->d_name[0] != '.' && isDirectory(subPath)) {
      int stat = testOnFilesInDir(testName, subPath, fn);
      if (stat != 0) { return stat; }
    }
  }
  return 0;
}
