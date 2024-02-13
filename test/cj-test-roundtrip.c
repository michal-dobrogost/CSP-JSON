#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cj/cj-csp.h"
#include "../cj/cj-csp-io.h"
#include "test-on-files.h"

int testCspJsonRoundtrip(const char* cspJson, size_t cspJsonLen) {
  CjCsp csp;
  int stat = cjCspJsonParse(cspJson, cspJsonLen, &csp);
  if (stat != 0) {
    printf("FAIL: cjCspJsonParse returned %d\n", stat);
    return stat;
  }

  CjError err = cjCspValidate(&csp);
  if (err != CJ_ERROR_OK) {
    printf("FAIL: input CSP does not validate.\n");
    return 1;
  }

  const int json2Size = 2*cspJsonLen + 1;
  char* json2 = (char*) malloc(json2Size);
  FILE* f = fmemopen(json2, json2Size, "w");
  stat = cjCspJsonPrint(f, &csp);
  fclose(f);

  if (strcmp(cspJson, json2) != 0) {
    printf("FAIL\n");
    printf("Got:\n");
    printf("---\n");
    printf("%s\n", json2);
    printf("---\n");
    printf("Expected:\n");
    printf("---\n");
    printf("%s\n", cspJson);
    printf("---\n");
    return 2;
  }

  return 0;
}

void printUsage(int argc, char** argv) {
  char* exe = argc > 1 ? argv[0] : "cj-test-roundtrip";
  fprintf(stderr, "Usage: %s DATA_DIR_PATH\n", exe);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printUsage(argc, argv);
    return 1;
  }
  char* dataPath = argv[1];
  const char* testName = "testRoundTrip";
  return testOnFilesInDir(testName, dataPath, &testCspJsonRoundtrip);
}
