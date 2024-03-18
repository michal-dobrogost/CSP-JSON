#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../cj/cj-csp.h"
#include "../../cj/cj-csp-io.h"
#include "../../common/io.h"

void printUsage() {
  fprintf(stderr, "Usage: csp-json-satisfied --csp INSTANCE_FILENAME --solution SOLUTION_JSON\n");
}

int main(int argc, char** argv) {
  int stat = 0;
  CjError err = CJ_ERROR_OK;
  if (argc != 5) {
    fprintf(stderr, "ERROR: number of command line parameters.\n\n");
    printUsage();
    return 1;
  }
  else if (strcmp(argv[1], "--csp") != 0) {
    fprintf(stderr, "ERROR: missing --csp flag.\n\n");
    printUsage();
    return 1;
  }
  else if (strcmp(argv[3], "--solution") != 0) {
    fprintf(stderr, "ERROR: missing --solution flag.\n\n");
    printUsage();
    return 1;
  }
  char* cspInstanceFilename = argv[2];
  char* solutionJson = argv[4];

  FILE* cspInstanceFile = fopen(cspInstanceFilename, "r");
  if (! cspInstanceFile) {
    fprintf(stderr, "ERROR: failed to open csp instance file: %s\n", cspInstanceFilename); // TODO: make all tools include better error messages
    return 1;
  }

  char* cspJson = NULL;
  size_t cspJsonLen = 0;
  if (0 != (stat = readAll(cspInstanceFile, &cspJson, &cspJsonLen))) {
    fprintf(stderr, "ERROR(%d): failed to read csp instance file: %s\n", stat, cspInstanceFilename); // TODO: check other tools for \n in all prints.
    return 1;
  }

  CjCsp csp = cjCspInit();
  if (CJ_ERROR_OK != (err = cjCspJsonParse(cspJson, cspJsonLen, &csp))) {
    fprintf(stderr, "ERROR(%d): failed to parse csp instance file: %s\n", err, cspInstanceFilename);
    return err;
  }

  if (CJ_ERROR_OK != (err = cjCspValidate(&csp))) {
    fprintf(stderr, "ERROR: CSP does not pass validation: %d\n", err);
    return err; // TODO: check other tools that they return err from main
  }

  if (strcmp(solutionJson, "null") == 0) {
    printf("true\n");
    cjCspFree(&csp);
    return 0;
  }

  CjIntTuples solution = cjIntTuplesInit();
  const int defaultArity = -1;
  if (CJ_ERROR_OK != (err = cjIntTuplesParse(defaultArity, solutionJson, strlen(solutionJson), &solution))) {
    fprintf(stderr, "ERROR(%d): failed to parse solution JSON: %s\n", err, solutionJson);
    return err;
  }
  else if (solution.arity != -1) {
    fprintf(stderr, "ERROR: --solution must be a 1D array.\n");
    return err;
  }

  int solved = 0;
  if (CJ_ERROR_OK != (err = cjCspIsSolved(&csp, &solution, &solved))) {
    fprintf(stderr, "ERROR: solved check error: %d\n", err);
    return err;
  }

  if (solved) {
    printf("true\n");
  }
  else {
    printf("false\n");
  }

  cjCspFree(&csp);
  cjIntTuplesFree(&solution);

  return 0;
}
