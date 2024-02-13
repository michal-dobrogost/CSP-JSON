#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../cj/cj-csp.h"

#include "test.h"

////////////////////////////////////////////////////////////////////////////////
// cjIntTuples

void cjIntTuplesArrayTestSize2() {
  CjIntTuples* xs = cjIntTuplesArray(2);
  EXPECT_PTR_NEQ(xs, NULL);
  EXPECT_EQ(xs[0].size, 0);
  EXPECT_EQ(xs[0].arity, 0);
  EXPECT_PTR_EQ(xs[0].data, NULL);
  EXPECT_EQ(xs[1].size, 0);
  EXPECT_EQ(xs[1].arity, 0);
  EXPECT_PTR_EQ(xs[1].data, NULL);
  cjIntTuplesArrayFree(&xs, 2);
  EXPECT_PTR_EQ(xs, NULL);
}

void cjIntTuplesInitFree() {
  CjIntTuples xs = cjIntTuplesInit();
  cjIntTuplesFree(&xs);
}

////////////////////////////////////////////////////////////////////////////////
// cjDomain

void cjDomainArrayTestSize2() {
  CjDomain* xs = cjDomainArray(2);
  EXPECT_PTR_NEQ(xs, NULL);
  EXPECT_EQ(xs[0].type, CJ_DOMAIN_UNDEF);
  EXPECT_EQ(xs[1].type, CJ_DOMAIN_UNDEF);
  cjDomainArrayFree(&xs, 2);
  EXPECT_PTR_EQ(xs, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// cjConstraintDef

void cjConstraintDefArrayTestSize2() {
  CjConstraintDef* xs = cjConstraintDefArray(2);
  EXPECT_PTR_NEQ(xs, NULL);
  EXPECT_EQ(xs[0].type, CJ_CONSTRAINT_DEF_UNDEF);
  EXPECT_EQ(xs[1].type, CJ_CONSTRAINT_DEF_UNDEF);
  cjConstraintDefArrayFree(&xs, 2);
  EXPECT_PTR_EQ(xs, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// cjConstraint

void cjConstraintArrayTestSize2() {
  CjConstraint* xs = cjConstraintArray(2);
  EXPECT_PTR_NEQ(xs, NULL);
  EXPECT_EQ(xs[0].id, -1);
  EXPECT_EQ(xs[0].vars.arity, 0);
  EXPECT_EQ(xs[0].vars.size, 0);
  EXPECT_PTR_EQ(xs[0].vars.data, NULL);
  EXPECT_EQ(xs[1].id, -1);
  EXPECT_EQ(xs[1].vars.arity, 0);
  EXPECT_EQ(xs[1].vars.size, 0);
  EXPECT_PTR_EQ(xs[1].vars.data, NULL);
  cjConstraintArrayFree(&xs, 2);
  EXPECT_PTR_EQ(xs, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// cjCsp

void cjCspInitFree() {
  CjCsp csp = cjCspInit();
  cjCspFree(&csp);
}

////////////////////////////////////////////////////////////////////////////////
// main

void printUsage(int argc, char** argv) {
  char* exe = argc > 1 ? argv[0] : "cj-test-csp";
  fprintf(stderr, "Usage: %s\n", exe);
}

/** Exit with 0 on success. Exit != 0 on failure. */
int main(int argc, char** argv) {
  if (argc != 1) {
    printUsage(argc, argv);
    return 1;
  }

  TEST(cjIntTuplesArrayTestSize2());
  TEST(cjIntTuplesInitFree());

  TEST(cjDomainArrayTestSize2());

  TEST(cjConstraintDefArrayTestSize2());

  TEST(cjCspInitFree());

  return 0;
}


