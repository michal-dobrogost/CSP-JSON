#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../cj/cj-csp.h"
#include "../cj/cj-csp-io.h"

#include "test.h"

const char* cspJsonMin = "{"
  "\"meta\": {"
    "\"id\": \"\","
    "\"algo\": \"\","
    "\"params\": null"
  "},"
  "\"domains\": [],"
  "\"vars\": [],"
  "\"constraintDefs\": [],"
  "\"constraints\": []"
"}";

const char* cspJsonSmall= "{"
  "\"meta\": {"
    "\"id\": \"test/small\","
    "\"algo\": \"test\","
    "\"params\": {\"vals\": 2, \"vars\": 2}"
  "},"
  "\"domains\": [{\"values\": [0, 1]}],"
  "\"vars\": [0, 0],"
  "\"constraintDefs\": ["
    "{\"noGoods\": [[0, 0], [1, 1]]}"
  "],"
  "\"constraints\": ["
    "{\"id\": 0, \"vars\": [0, 1]}"
  "]"
"}";

////////////////////////////////////////////////////////////////////////////////
// cjIntTuplesJsonParse

void cjIntTuplesParseTestZero() {
  const char* tsJson = "[]";
  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesParse(0, tsJson, strlen(tsJson), &ts), CJ_ERROR_OK);
  EXPECT_EQ(ts.size, 0);
  EXPECT_EQ(ts.arity, 0);
  EXPECT_PTR_EQ(ts.data, NULL);
}

void cjIntTuplesParseTestNull() {
  const char* tsJson = "[]";
  EXPECT_RETURN(cjIntTuplesParse(0, tsJson, strlen(tsJson), NULL), CJ_ERROR_ARG);

  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesParse(0, NULL, 9999, &ts), CJ_ERROR_ARG);
}

void cjIntTuplesParseTestEmpty() {
  const char* tsJson = "";
  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesParse(0, tsJson, strlen(tsJson), &ts), CJ_ERROR_ARG);
  EXPECT_EQ(ts.size, 0);
  EXPECT_EQ(ts.arity, 0);
}

void cjIntTuplesParseTestNotArray() {
  const char* tsJson = "\"this is not an array\"";
  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesParse(0, tsJson, strlen(tsJson), &ts), CJ_ERROR_IS_NOT_ARRAY);
}

void cjIntTuplesParseTest1D() {
  const char* tsJson = "[1,2,3]";
  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesParse(-1, tsJson, strlen(tsJson), &ts), CJ_ERROR_OK);
  EXPECT_EQ(ts.size, 3);
  EXPECT_EQ(ts.arity, -1);
  EXPECT_EQ(ts.data[0], 1);
  EXPECT_EQ(ts.data[1], 2);
  EXPECT_EQ(ts.data[2], 3);
}

void cjIntTuplesParseTest1DInvalidItem() {
  const char* tsJson = "[1,2,\"3\"]";
  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesParse(-1, tsJson, strlen(tsJson), &ts), CJ_ERROR_INTTUPLES_ITEM_TYPE);
}

void cjIntTuplesParseTest2DArity2() {
  const char* tsJson = "[[1,2],[3,4]]";
  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesParse(0, tsJson, strlen(tsJson), &ts), CJ_ERROR_OK);
  EXPECT_EQ(ts.size, 2);
  EXPECT_EQ(ts.arity, 2);
  EXPECT_EQ(ts.data[0], 1);
  EXPECT_EQ(ts.data[1], 2);
  EXPECT_EQ(ts.data[2], 3);
  EXPECT_EQ(ts.data[3], 4);
}

void cjIntTuplesParseTest2DArity1() {
  const char* tsJson = "[[1],[2],[3]]";
  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesParse(0, tsJson, strlen(tsJson), &ts), CJ_ERROR_OK);
  EXPECT_EQ(ts.size, 3);
  EXPECT_EQ(ts.arity, 1);
  EXPECT_EQ(ts.data[0], 1);
  EXPECT_EQ(ts.data[1], 2);
  EXPECT_EQ(ts.data[2], 3);
}

void cjIntTuplesParseTest2DArity0() {
  const char* tsJson = "[[],[]]";
  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesParse(0, tsJson, strlen(tsJson), &ts), CJ_ERROR_OK);
  EXPECT_EQ(ts.size, 2);
  EXPECT_EQ(ts.arity, 0);
  EXPECT_PTR_EQ(ts.data, NULL);
}

void cjIntTuplesParseTest2DInvalidItemArity() {
  const char* tsJson = "[[1,2],[3]]";
  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesParse(0, tsJson, strlen(tsJson), &ts), CJ_ERROR_INTTUPLES_ITEM_TYPE);
}

void cjIntTuplesParseTest2DInvalidItemType() {
  const char* tsJson = "[[1,2],\"3\"]";
  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesParse(0, tsJson, strlen(tsJson), &ts), CJ_ERROR_INTTUPLES_ITEM_TYPE);
}

void cjIntTuplesParseTest2DInvalidSubItemType() {
  const char* tsJson = "[[1,2],[\"3\",\"4\"]]";
  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesParse(0, tsJson, strlen(tsJson), &ts), CJ_ERROR_INTTUPLES_ITEM_TYPE);
}

////////////////////////////////////////////////////////////////////////////////
// cjIntTuplesJsonPrint

void cjIntTuplesJsonPrintTestNull() {
  EXPECT_RETURN(cjIntTuplesJsonPrint(stdout, NULL), CJ_ERROR_ARG);

  CjIntTuples ts = cjIntTuplesInit();
  EXPECT_RETURN(cjIntTuplesJsonPrint(NULL, &ts), CJ_ERROR_ARG);
}

////////////////////////////////////////////////////////////////////////////////
// cjCspJsonParse

void cjCspJsonParseTestMin() {
  CjCsp csp = cjCspInit();
  EXPECT_RETURN(cjCspJsonParse(cspJsonMin, strlen(cspJsonMin), &csp), CJ_ERROR_OK);
  EXPECT_STR_EQ(csp.meta.id, "");
  EXPECT_STR_EQ(csp.meta.algo, "");
  EXPECT_STR_EQ(csp.meta.paramsJSON, "null");
  EXPECT_EQ(csp.domainsSize, 0);
  EXPECT_PTR_EQ(csp.domains, NULL);
  EXPECT_EQ(csp.vars.arity, -1);
  EXPECT_EQ(csp.vars.size, 0);
  EXPECT_PTR_EQ(csp.vars.data, NULL);
  EXPECT_EQ(csp.constraintDefsSize, 0);
  EXPECT_PTR_EQ(csp.constraintDefs, NULL);
  EXPECT_EQ(csp.constraintsSize, 0);
  EXPECT_PTR_EQ(csp.constraints, NULL);
}

void cjCspJsonParseTestNull() {
  EXPECT_RETURN(cjCspJsonParse(cspJsonMin, strlen(cspJsonMin), NULL), CJ_ERROR_ARG);

  CjCsp csp = cjCspInit();
  EXPECT_RETURN(cjCspJsonParse(NULL, 9999, &csp), CJ_ERROR_ARG);
}

void cjCspJsonParseTestEmpty() {
  CjCsp csp = cjCspInit();
  EXPECT_RETURN(cjCspJsonParse("", 0, &csp), CJ_ERROR_ARG);
}

void cjCspJsonParseTestSmall() {
  CjCsp csp = cjCspInit();
  EXPECT_RETURN(cjCspJsonParse(cspJsonSmall, strlen(cspJsonSmall), &csp), CJ_ERROR_OK);
  EXPECT_STR_EQ(csp.meta.id, "test/small");
  EXPECT_STR_EQ(csp.meta.algo, "test");
  EXPECT_STR_EQ(csp.meta.paramsJSON, "{\"vals\": 2, \"vars\": 2}");

  EXPECT_EQ(csp.domainsSize, 1);
  EXPECT_PTR_NEQ(csp.domains, NULL);
  EXPECT_PTR_EQ(csp.domains[0].type, CJ_DOMAIN_VALUES);
  EXPECT_EQ(csp.domains[0].values.size, 2);
  EXPECT_EQ(csp.domains[0].values.arity, -1);
  EXPECT_EQ(csp.domains[0].values.data[0], 0);
  EXPECT_EQ(csp.domains[0].values.data[1], 1);

  EXPECT_EQ(csp.vars.arity, -1);
  EXPECT_EQ(csp.vars.size, 2);
  EXPECT_PTR_NEQ(csp.vars.data, NULL);
  EXPECT_EQ(csp.vars.data[0], 0);
  EXPECT_EQ(csp.vars.data[1], 0);

  EXPECT_EQ(csp.constraintDefsSize, 1);
  EXPECT_PTR_NEQ(csp.constraintDefs, NULL);
  EXPECT_EQ(csp.constraintDefs[0].type, CJ_CONSTRAINT_DEF_NO_GOODS);
  EXPECT_EQ(csp.constraintDefs[0].noGoods.arity, 2);
  EXPECT_EQ(csp.constraintDefs[0].noGoods.size, 2);
  EXPECT_EQ(csp.constraintDefs[0].noGoods.data[0], 0);
  EXPECT_EQ(csp.constraintDefs[0].noGoods.data[1], 0);
  EXPECT_EQ(csp.constraintDefs[0].noGoods.data[2], 1);
  EXPECT_EQ(csp.constraintDefs[0].noGoods.data[3], 1);

  EXPECT_EQ(csp.constraintsSize, 1);
  EXPECT_PTR_NEQ(csp.constraints, NULL);
  EXPECT_EQ(csp.constraints[0].id, 0);
  EXPECT_EQ(csp.constraints[0].vars.arity, -1);
  EXPECT_EQ(csp.constraints[0].vars.size, 2);
  EXPECT_EQ(csp.constraints[0].vars.data[0], 0);
  EXPECT_EQ(csp.constraints[0].vars.data[1], 1);
}

////////////////////////////////////////////////////////////////////////////////
// cjCspJsonPrint

void cjCspJsonPrintTestNull() {
  CjCsp csp = cjCspInit();
  EXPECT_RETURN(cjCspJsonPrint(NULL, &csp), CJ_ERROR_ARG);

  FILE* f = fopen("/dev/null", "w");
  EXPECT_RETURN(cjCspJsonPrint(f, NULL), CJ_ERROR_ARG);
  fclose(f);
}

////////////////////////////////////////////////////////////////////////////////
// main

void printUsage(int argc, char** argv) {
  char* exe = argc > 1 ? argv[0] : "cj-test-csp-io";
  fprintf(stderr, "Usage: %s\n", exe);
}

/** Exit with 0 on success. Exit != 0 on failure. */
int main(int argc, char** argv) {
  if (argc != 1) {
    printUsage(argc, argv);
    return 1;
  }

  TEST(cjIntTuplesParseTestZero());
  TEST(cjIntTuplesParseTestNull());
  TEST(cjIntTuplesParseTestEmpty());
  TEST(cjIntTuplesParseTestNotArray());
  TEST(cjIntTuplesParseTest1D());
  TEST(cjIntTuplesParseTest1DInvalidItem());
  TEST(cjIntTuplesParseTest2DArity2());
  TEST(cjIntTuplesParseTest2DArity1());
  TEST(cjIntTuplesParseTest2DArity0());
  TEST(cjIntTuplesParseTest2DInvalidItemArity());
  TEST(cjIntTuplesParseTest2DInvalidItemType());
  TEST(cjIntTuplesParseTest2DInvalidSubItemType());

  TEST(cjIntTuplesJsonPrintTestNull());

  TEST(cjCspJsonParseTestMin());
  TEST(cjCspJsonParseTestNull());
  TEST(cjCspJsonParseTestEmpty());
  TEST(cjCspJsonParseTestSmall());

  TEST(cjCspJsonPrintTestNull());

  return 0;
}


