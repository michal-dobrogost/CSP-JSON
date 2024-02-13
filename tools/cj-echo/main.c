#include <stddef.h>
#include <string.h>
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
  if (argc != 3) {
    fprintf(stderr, "ERROR: number of command line parameters.\n\n");
    printUsage();
    return 1;
  }
  else if (strcmp(argv[1], "--csp") != 0) {
    fprintf(stderr, "ERROR: missing --csp flag.\n\n");
    printUsage();
    return 1;
  }
  char* cspInstanceFilename = argv[2];

  FILE* cspInstanceFile = fopen(cspInstanceFilename, "r");
  if (! cspInstanceFile) {
    fprintf(stderr, "ERROR: failed to open csp instance file.\n");
    return 1;
  }

  char* cspJson = NULL;
  size_t cspJsonLen = 0;
  if (0 != (err = readAll(cspInstanceFile, &cspJson, &cspJsonLen))) {
    fprintf(stderr, "ERROR(%d): failed to read csp instance file.", err);
    return 1;
  }

  CjCsp csp = cjCspInit();
  if (0 != (err = cjCspJsonParse(cspJson, cspJsonLen, &csp))) {
    fprintf(stderr, "ERROR(%d): failed to parse csp instance file.", err);
    return 1;
  }

  if (0 != (err = cjCspJsonPrint(stdout, &csp))) {
    fprintf(stderr, "ERROR(%d): failed to print CSP.", err);
  }

  cjCspFree(&csp);

  return 0;
}
