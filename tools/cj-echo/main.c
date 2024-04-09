#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../cj/cj-csp.h"
#include "../../cj/cj-csp-io.h"
#include "../../common/io.h"

void printUsage() {
  fprintf(stderr, "Usage: csp-json-satisfied --csp INSTANCE_FILENAME\n");
}

int main(int argc, char** argv) {
  int err = 0;
  if (argc != 3 && argc != 4) {
    fprintf(stderr, "ERROR: number of command line parameters.\n\n");
    printUsage();
    return 1;
  }
  bool normalize = false;
  char* cspInstanceFilename = NULL;
  for (int iArg = 1; iArg < argc; ) {
    if (strcmp(argv[iArg], "--normalize") == 0) {
      normalize = true;
      iArg++;
    }
    else if (strcmp(argv[iArg], "--csp") == 0) {
      if (iArg >= argc - 1) {
        fprintf(stderr, "ERROR: --csp flag takes 1 argument.\n\n");
        printUsage();
        return 1;
      }
      cspInstanceFilename = argv[iArg+1];
      iArg += 2;
    }
    else {
      fprintf(stderr, "ERROR: unknown argument: %s\n\n", argv[iArg]);
      printUsage();
      return 1;
    }
  }
  if (! cspInstanceFilename) {
    fprintf(stderr, "ERROR: missing --csp flag.\n\n");
    printUsage();
    return 1;
  }

  FILE* cspInstanceFile = fopen(cspInstanceFilename, "r");
  if (! cspInstanceFile) {
    fprintf(stderr, "ERROR: failed to open csp instance file: %s\n", cspInstanceFilename);
    return 1;
  }

  char* cspJson = NULL;
  size_t cspJsonLen = 0;
  if (CJ_ERROR_OK != (err = readAll(cspInstanceFile, &cspJson, &cspJsonLen))) {
    fprintf(stderr, "ERROR(%d): failed to read csp instance file.", err);
    return 1;
  }

  CjCsp csp = cjCspInit();
  if (CJ_ERROR_OK != (err = cjCspJsonParse(cspJson, cspJsonLen, &csp))) {
    fprintf(stderr, "ERROR(%d): failed to parse csp instance file.", err);
    return 1;
  }

  if (normalize) {
    if (CJ_ERROR_OK != (err = cjCspNormalize(&csp))) {
      fprintf(stderr, "ERROR(%d): failed to normalize the csp instance.", err);
      return 1;
    }
  }

  if (CJ_ERROR_OK != (err = cjCspJsonPrint(stdout, &csp))) {
    fprintf(stderr, "ERROR(%d): failed to print CSP.", err);
  }

  cjCspFree(&csp);

  return 0;
}
