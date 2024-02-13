#include <stdlib.h>
#include <string.h>

#include "../cj/cj-csp.h"
#include "../cj/cj-csp-io.h"
#include "test-on-files.h"

int testCjCspIsValid(const char* cspJson, size_t cspJsonLen) {
  CjCsp csp;
  int stat = cjCspJsonParse(cspJson, cspJsonLen, &csp);
  if (stat != 0) {
    printf("FAIL: cjCspJsonParse returned %d\n", stat);
    return stat;
  }

  CjError err = cjCspValidate(&csp);
  if (err != CJ_ERROR_OK) {
    printf("FAIL: cjCspValidate(%d) invalidated incorrectly.\n", err);
    return 1;
  }

  return 0;
}

int testCjCspIsInvalid(const char* cspJson, size_t cspJsonLen) {
  CjCsp csp;
  int stat = cjCspJsonParse(cspJson, cspJsonLen, &csp);
  if (stat != 0) {
    printf("FAIL: cjCspJsonParse returned %d\n", stat);
    return stat;
  }

  CjError err = cjCspValidate(&csp);
  if (err == CJ_ERROR_OK) {
    printf("FAIL: cjCspValidate(%d) validated incorrectly.\n", err);
    return 1;
  }

  return 0;
}

void printUsage(int argc, char** argv) {
  char* exe = argc > 1 ? argv[0] : "cj-test-validation";
  fprintf(stderr, "Usage: %s DATA_DIR_PATH\n", exe);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printUsage(argc, argv);
    return 1;
  }
  char* projectPath = argv[1];

  char notValidPath[1024];
  snprintf(notValidPath, sizeof(notValidPath), "%s/test/data/validation-neg", projectPath);
  int stat = testOnFilesInDir("testInvalid", notValidPath, &testCjCspIsInvalid);
  if (stat != 0) {
    return stat;
  }

  char validPath[1024];
  snprintf(validPath, sizeof(notValidPath), "%s/data", projectPath);
  stat = testOnFilesInDir("testValid", validPath, &testCjCspIsValid);
  if (stat != 0) {
    return stat;
  }

  return 0;
}
