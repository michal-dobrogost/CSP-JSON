#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JSMN_STRICT
#include "jsmn.h"
#include "cj-csp-io.h"

//#define CJ_DEBUG

////////////////////////////////////////////////////////////////////////////////
// json helpers
//

/**
 * Copy a JSON field into a new allocated string.
 * Return CJ_ERROR_OK on success.
 * Free the output string with free().
 *
 * JSON strings will not have enclosing quotes. Braces are included.
 */
static int jsonStrCpy(int includeQuotes, const char* json, jsmntok_t* t, char** out) {
  if (!json || !t || !out) { return CJ_ERROR_ARG; }
  const int offset = includeQuotes && t->type == JSMN_STRING ? 1 : 0;
  *out = malloc(t->end - t->start + 1 + 2*offset);
  if (!(*out)) { return CJ_ERROR_NOMEM; }
  strncpy(*out, json + t->start - offset, t->end - t->start + 2*offset);
  (*out)[t->end - t->start + 2*offset] = '\0';
  return CJ_ERROR_OK;
}

/** Return 1 if equal, 0 otherwise. */
static int jsonEq(const char *json, jsmntok_t* tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 1;
  }
  return 0;
}

/** return 1 if character is numeric, 0 otherwise. */
static int jsonIsNumeric(char c) {
  switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return 1;
  }
  return 0;
}

/** Return 1 if tok is a JSON int, 0 otherwise. */
static int jsonIsInt(const char *json, jsmntok_t* tok) {
  if (tok->type != JSMN_PRIMITIVE) { return 0; }

  const char first = (json + tok->start)[0];
  if (first != '-' && !jsonIsNumeric(first)) { return 0; }

  for (int iChar = 1; iChar < tok->size; ++iChar) {
    const char c = (json + tok->start)[iChar];
    if (!jsonIsNumeric(c)) { return 0; }
  }

  return 1;
}

static void logTok(const char* prefix, const char* json, jsmntok_t* t) {
#ifdef CJ_DEBUG
  if (!prefix || !json || !t) { return; }
  printf("%s{", prefix);
  switch (t->type) {
    case JSMN_UNDEFINED: printf("undef"); break;
    case JSMN_OBJECT: printf("object"); break;
    case JSMN_ARRAY: printf("array"); break;
    case JSMN_STRING: printf("string"); break;
    case JSMN_PRIMITIVE: printf("prim"); break;
  }
  printf("|s:%d|e:%d|size:%d|", t->start, t->end, t->size);
  for (size_t i = 0; i < t->end - t->start; ++i) {
    printf("%c", (json + t->start)[i]);
  }
  printf("}\n");
#endif
}

static int jsonConsumeAny(const char* json, jsmntok_t* t) {
  logTok("consumeAny:", json, t);
  if (!json || !t) { return CJ_ERROR_ARG; }

  if (t->type == JSMN_OBJECT) {
    int consumed = 1;
    for (int iChild = 0; iChild < t->size; ++iChild) {
      int stat = jsonConsumeAny(json, t + consumed + 1);
      if (stat < 0) { return stat; }
      consumed += 1 + stat;
    }
    return consumed;
  }
  else if (t->type == JSMN_STRING || t->type == JSMN_PRIMITIVE) {
    return 1;
  }
  else if (t->type == JSMN_ARRAY) {
    int consumed = 1;
    for (int iChild = 0; iChild < t->size; ++iChild) {
      int stat = jsonConsumeAny(json, t + consumed);
      if (stat < 0) { return stat; }
      consumed += stat;
    }
    return consumed;
  }
  else {
    return CJ_ERROR_JSON_TYPE;
  }
}

////////////////////////////////////////////////////////////////////////////////
// cjIntTuples
//

static int cjIntTuplesParseTok(
  const int defaultArity, const char* json, jsmntok_t* t, CjIntTuples* ts)
{
  logTok("CjIntTuples:", json, t);
  if (!json || !t || !ts) { return CJ_ERROR_ARG; }
  if (t->type != JSMN_ARRAY) { return CJ_ERROR_IS_NOT_ARRAY; }
  int consumed = 1;

  if (t->size == 0) {
    *ts = cjIntTuplesInit();
    ts->arity = defaultArity;
    return consumed;
  }

  // 2D case (array of tuples)
  if (t[consumed].type == JSMN_ARRAY) {
    int arity = t[consumed].size;
    int stat = cjIntTuplesAlloc(t->size, arity, ts);
    if (stat != CJ_ERROR_OK) { return stat; }

    for (int iChild = 0; iChild < t->size; ++iChild) {
      logTok("CjIntTuples-item:", json, &t[consumed]);
      if (t[consumed].type != JSMN_ARRAY || t[consumed].size != arity) {
        cjIntTuplesFree(ts);
        return CJ_ERROR_INTTUPLES_ITEM_TYPE;
      }

      size_t jSize = t[consumed].size;
      ++consumed;

      for (int jItem = 0; jItem < jSize; ++jItem, ++consumed) {
        if (! jsonIsInt(json, t + consumed)) {
          cjIntTuplesFree(ts);
          return CJ_ERROR_INTTUPLES_ITEM_TYPE;
        }
        ts->data[iChild*arity + jItem] = strtol(json + t[consumed].start, NULL, 10);
      }
    }
  }
  // 1D case (array of ints)
  else if (jsonIsInt(json, t + consumed)) {
    const int arity = -1;
    int stat = cjIntTuplesAlloc(t->size, arity, ts);
    if (stat != CJ_ERROR_OK) { return stat; }

    for (int iChild = 0; iChild < t->size; ++iChild, ++consumed) {
      logTok("CjIntTuples-item:", json, &t[consumed]);
      if (!jsonIsInt(json, t + consumed)) {
        cjIntTuplesFree(ts);
        return CJ_ERROR_INTTUPLES_ITEM_TYPE;
      }
      ts->data[iChild] = strtol(json + t[consumed].start, NULL, 10);
    }
  }
  // Error case (eg. array of objects)
  else {
    *ts = cjIntTuplesInit();
    return CJ_ERROR_INTTUPLES_ITEM_TYPE;
  }

  return consumed;
}

////////////////////////////////////////////////////////////////////////////////
// cjCsp
//

static int cjCspJsonParseMeta(const char* json, jsmntok_t* t, CjCsp* csp) {
  logTok("meta:", json, t);
  if (!json || !t || !csp) { return CJ_ERROR_ARG; }
  if (t->type != JSMN_OBJECT || t->size != 3) { return CJ_ERROR_META_IS_NOT_OBJECT; }

  int consumed = 1;
  for (int iChild = 0; iChild < t->size; ++iChild) {
    logTok("meta-child:", json, &t[consumed]);
    if (jsonEq(json, t + consumed, "id")) {
      if (t[consumed + 1].type != JSMN_STRING) { return CJ_ERROR_META_ID_NOT_STRING; }
      int stat = jsonStrCpy(0, json, t + consumed + 1, &csp->meta.id);
      if (stat != CJ_ERROR_OK) { return stat; }
      consumed += 2;
    }
    else if (jsonEq(json, t + consumed, "algo")) {
      if (t[consumed + 1].type != JSMN_STRING) { return CJ_ERROR_META_ALGO_NOT_STRING; }
      int stat = jsonStrCpy(0, json, t + consumed + 1, &csp->meta.algo);
      if (stat != CJ_ERROR_OK) { return stat; }
      consumed += 2;
    }
    else if (jsonEq(json, t + consumed, "params")) {
      int stat = jsonConsumeAny(json, t + consumed + 1);
      if (stat < 0) { return stat; }
      int cpyStat = jsonStrCpy(1, json, t + consumed + 1, &csp->meta.paramsJSON);
      if (cpyStat != CJ_ERROR_OK) { return cpyStat; }
      consumed += 1 + stat;
    }
    else {
      return CJ_ERROR_META_UNKNOWN_FIELD;
    }
  }

  return consumed;
}

static int cjCspJsonParseDomain(const char* json, jsmntok_t* t, CjDomain* domain) {
  logTok("values:", json, t);
  if (!json || !t || !domain) { return CJ_ERROR_ARG; }
  if (t->type != JSMN_OBJECT || t->size != 1) { return CJ_ERROR_DOMAIN_IS_NOT_OBJECT; }
  if (! jsonEq(json, t + 1, "values")) { return CJ_ERROR_DOMAIN_UNKNOWN_TYPE; }
  if (t[2].type != JSMN_ARRAY) { return CJ_ERROR_DOMAIN_VALUES_IS_NOT_ARRAY; }

  const int defaultArity = -1;
  int stat = cjIntTuplesParseTok(defaultArity, json, &t[2], &domain->values);
  if (stat < 0) {
    return stat;
  }
  domain->type = CJ_DOMAIN_VALUES;
  return 2 + stat;
}

static int cjCspJsonParseDomains(const char* json, jsmntok_t* t, CjCsp* csp) {
  logTok("domains:", json, t);
  if (t->type != JSMN_ARRAY) { return CJ_ERROR_DOMAINS_IS_NOT_ARRAY; }

  if (t->size > 0) {
    csp->domains = cjDomainArray(t->size);
    if (!csp->domains) { return CJ_ERROR_NOMEM; }
    csp->domainsSize = t->size;
  }
  csp->domainsSize = t->size;

  int consumed = 1;
  for (int iChild = 0; iChild < t->size; ++iChild) {
    logTok("domains-child:", json, &t[consumed]);
    int stat = cjCspJsonParseDomain(json, t + consumed, &csp->domains[iChild]);
    if (stat < 0) {
      cjDomainArrayFree(&csp->domains, csp->domainsSize);
      csp->domainsSize = 0;
      return stat;
    }
    consumed += stat;
  }

  return consumed;
}

static int cjCspJsonParseVars(const char* json, jsmntok_t* t, CjCsp* csp) {
  logTok("vars:", json, t);
  if (!json || !t || !csp) { return CJ_ERROR_ARG; }
  if (t->type != JSMN_ARRAY) { return CJ_ERROR_VARS_IS_NOT_ARRAY; }

  const int defaultArity = -1;
  return cjIntTuplesParseTok(defaultArity, json, t, &csp->vars);
}

static int cjCspJsonParseNoGoods(const char* json, jsmntok_t* t, CjConstraintDef* constraintDef) {
  logTok("noGoods:", json, t);
  if (!json || !t || !constraintDef) { return CJ_ERROR_ARG; }
  if (t->type != JSMN_ARRAY) { return CJ_ERROR_NOGOODS_IS_NOT_ARRAY; }

  const int defaultArity = 0;
  int stat = cjIntTuplesParseTok(defaultArity, json, t, &constraintDef->noGoods);
  if (stat < 0) {
    return stat;
  }
  constraintDef->type = CJ_CONSTRAINT_DEF_NO_GOODS;
  return stat;
}

static int cjCspJsonParseConstraintsDef(const char* json, jsmntok_t* t, CjCsp* csp) {
  logTok("constraintDefs:", json, t);
  if (!json || !t || !csp) { return CJ_ERROR_ARG; }
  if (t->type != JSMN_ARRAY) { return CJ_CONSTRAINTDEFS_IS_NOT_ARRAY; }

  if (t->size > 0) {
    csp->constraintDefs = cjConstraintDefArray(t->size);
    if (!csp->constraintDefs) { return CJ_ERROR_NOMEM; }
    csp->constraintDefsSize = t->size;
  }
  csp->constraintDefsSize = t->size;

  int consumed = 1;
  for (int iChild = 0; iChild < t->size; ++iChild) {
    logTok("constraintDefs-child:", json, &t[consumed]);
    if (jsonEq(json, t + consumed + 1, "noGoods")) {
      int stat = cjCspJsonParseNoGoods(json, t + consumed + 2, &csp->constraintDefs[iChild]);
      if (stat < 0) {
        cjConstraintDefArrayFree(&csp->constraintDefs, csp->constraintDefsSize);
        csp->constraintDefsSize = 0;
        return stat;
      }
      consumed += 2 + stat;
    }
    else {
      return CJ_CONSTRAINTDEF_UNKNOWN_TYPE;
    }
  }

  return consumed;
}

static int cjCspJsonParseConstraint(const char* json, jsmntok_t* t, CjConstraint* constraint) {
  logTok("constraint:", json, t);
  if (!json || !t || !constraint) { return CJ_ERROR_ARG; }
  if (t->type != JSMN_OBJECT) { return CJ_ERROR_CONSTRAINT_IS_NOT_OBJECT; }

  *constraint = cjConstraintInit();

  int consumed = 1;
  for (int iChild = 0; iChild < t->size; ++iChild) {
    logTok("constraint-child:", json, &t[consumed]);
    if (jsonEq(json, t + consumed, "id")) {
      if (! jsonIsInt(json, t + consumed + 1)) { return CJ_ERROR_CONSTRAINT_ID_IS_NOT_INT; }
      constraint->id = strtol(json + t[consumed + 1].start, NULL, 10);
      consumed += 2;
    }
    else if (jsonEq(json, t + consumed, "vars")) {
      if (t[consumed + 1].type != JSMN_ARRAY) { return CJ_ERROR_CONSTRAINT_VARS_IS_NOT_ARRAY; }

      const int defaultArity = -1;
      int stat = cjIntTuplesParseTok(defaultArity, json, &t[consumed + 1], &constraint->vars);
      if (stat < 0) {
        return stat;
      }
      consumed += 1 + stat;
    }
    else {
      return CJ_ERROR_CONSTRAINT_UNKNOWN_FIELD;
    }
  }

  return consumed;
}

static int cjCspJsonParseConstraints(const char* json, jsmntok_t* t, CjCsp* csp) {
  logTok("constraints:", json, t);
  if (!json || !t || !csp) { return CJ_ERROR_ARG; }
  if (t->type != JSMN_ARRAY) { return CJ_ERROR_CONSTRAINTS_IS_NOT_ARRAY; }

  if (t->size > 0) {
    csp->constraints = cjConstraintArray(t->size);
    if (!csp->constraints) { return CJ_ERROR_NOMEM; }
    csp->constraintsSize = t->size;
  }
  csp->constraintsSize = t->size;

  int consumed = 1;
  for (int iChild = 0; iChild < t->size; ++iChild) {
    int stat = cjCspJsonParseConstraint(json, t + consumed, &csp->constraints[iChild]);
    if (stat < 0) {
      cjConstraintArrayFree(&csp->constraints, csp->constraintsSize);
      csp->constraintsSize = 0;
      return stat;
    }
    consumed += stat;
  }

  return consumed;
}

/** Return negative on error, otherwise number of tokens consumed. */
static int cjCspJsonParseTop(const char* json, jsmntok_t* t, CjCsp* csp) {
  logTok("top:", json, t);
  if (!json || !t || !csp) { return CJ_ERROR_ARG; }
  if (t->type != JSMN_OBJECT) { return CJ_ERROR_CSPJSON_IS_NOT_OBJECT; }
  if (t->size != 5) { return CJ_ERROR_CSPJSON_BAD_FIELD_COUNT; }

  int consumed = 1;
  for (int iChild = 0; iChild < t->size; ++iChild) {
    logTok("top-child:", json, &t[consumed]);
    if (jsonEq(json, t + consumed, "meta")) {
      int stat = cjCspJsonParseMeta(json, t + consumed + 1, csp);
      if (stat < 0) { return stat; }
      consumed += 1 + stat;
    }
    else if (jsonEq(json, t + consumed, "domains")) {
      int stat = cjCspJsonParseDomains(json, t + consumed + 1, csp);
      if (stat < 0) { return stat; }
      consumed += 1 + stat;
    }
    else if (jsonEq(json, t + consumed, "vars")) {
      int stat = cjCspJsonParseVars(json, t + consumed + 1, csp);
      if (stat < 0) { return stat; }
      consumed += 1 + stat;
    }
    else if (jsonEq(json, t + consumed, "constraintDefs")) {
      int stat = cjCspJsonParseConstraintsDef(json, t + consumed + 1, csp);
      if (stat < 0) { return stat; }
      consumed += 1 + stat;
    }
    else if (jsonEq(json, t + consumed, "constraints")) {
      int stat = cjCspJsonParseConstraints(json, t + consumed + 1, csp);
      if (stat < 0) { return stat; }
      consumed += 1 + stat;
    }
    else {
      return CJ_ERROR_CSPJSON_UNKNOWN_FIELD;
    }
  }

  return consumed;
}

static CjError jsmnErrorToCjError(int jsmnErr) {
  switch (jsmnErr) {
    case JSMN_ERROR_NOMEM: return CJ_ERROR_JSMN_NOMEM;
    case JSMN_ERROR_INVAL: return CJ_ERROR_JSMN_INVAL;
    case JSMN_ERROR_PART:  return CJ_ERROR_JSMN_PART;
    default:               return CJ_ERROR_JSMN;
  }
}


/** jsmn_parse() tokens into (*t). Call free(*t) after use. */
static CjError jsmnTokenize(const char* json, const size_t jsonLen, jsmntok_t** t, int* numTokens) {
  if (!json || !t || !numTokens) { return CJ_ERROR_ARG; }

  jsmn_parser p;
  jsmn_init(&p);

  // Calculate number of tokens needed
  *numTokens = jsmn_parse(&p, json, jsonLen, NULL, 0);
  if (*numTokens < 0) {
    return jsmnErrorToCjError(*numTokens);
  }
  if (*numTokens == 0) {
    return CJ_ERROR_OK;
  }
  *t = malloc(sizeof(jsmntok_t) * (*numTokens));
  if (! (*t)) {
    return CJ_ERROR_NOMEM;
  }

  // Tokenize
  jsmn_init(&p);
  *numTokens = jsmn_parse(&p, json, jsonLen, *t, *numTokens);
  if (*numTokens < 0) {
    free(*t);
    *t = NULL;
    return jsmnErrorToCjError(*numTokens);
  }
  return CJ_ERROR_OK;
}

////////////////////////////////////////////////////////////////////////////////
// Public parsing functions
//

CjError cjIntTuplesParse(
  const int defaultArity, const char* json, const size_t jsonLen, CjIntTuples* ts)
{
  if (!json || !ts) { return CJ_ERROR_ARG; }
  if (defaultArity < -1) { return CJ_ERROR_ARG; }

  *ts = cjIntTuplesInit();

  jsmntok_t* t = NULL;
  int numTokens = 0;
  CjError stat = jsmnTokenize(json, jsonLen, &t, &numTokens);
  if (stat != CJ_ERROR_OK) { return stat; }
  if (numTokens == 0) { free(t); return CJ_ERROR_ARG; }

  int consumedOrStat = cjIntTuplesParseTok(defaultArity, json, t, ts);
  free(t);
  if (consumedOrStat < 0)       { return consumedOrStat; }
  else if (consumedOrStat == 0) { return CJ_ERROR_ARG; }
  else                          { return CJ_ERROR_OK; }
}

CjError cjIntTuplesJsonPrint(FILE* f, CjIntTuples* ts) {
  if (!f || !ts) { return CJ_ERROR_ARG; }
  if (ts->size < 0 || ts->arity < -1) { return CJ_ERROR_ARG; }
  fprintf(f, "[");
  for (int s = 0; s < ts->size; ++s) {
    if (s > 0) { fprintf(f, ", "); }
    if (ts->arity >= 0) { fprintf(f, "["); }
    for (int a = 0; a < abs(ts->arity); ++a) {
      if (a > 0) { fprintf(f, ", "); }
      fprintf(f, "%d", ts->data[s*abs(ts->arity) + a]);
    }
    if (ts->arity >= 0) { fprintf(f, "]"); }
  }
  fprintf(f, "]");

  return CJ_ERROR_OK;
}



////////////////////////////////////////////////////////////////////////////////
// Public printing functions
//

CjError cjCspJsonParse(const char* json, const size_t jsonLen, CjCsp* csp) {
  if (!json || !csp) { return CJ_ERROR_ARG; }

  *csp = cjCspInit();

  jsmntok_t* t = NULL;
  int numTokens = 0;
  CjError stat = jsmnTokenize(json, jsonLen, &t, &numTokens);
  if (stat != CJ_ERROR_OK) { return stat; }
  if (numTokens == 0) { free(t); return CJ_ERROR_ARG; }

  int consumedOrStat = cjCspJsonParseTop(json, t, csp);
  free(t);
  if (consumedOrStat < 0)       { return consumedOrStat; }
  else if (consumedOrStat == 0) { return CJ_ERROR_ARG; }
  else                          { return CJ_ERROR_OK; }
}


CjError cjCspJsonPrint(FILE* f, CjCsp* csp) {
  if (!f || !csp) { return CJ_ERROR_ARG; }
  fprintf(f, "{\n");

  fprintf(f, "  \"meta\": {\n");
  fprintf(f, "    \"id\": \"%s\",\n", csp->meta.id);
  fprintf(f, "    \"algo\": \"%s\",\n", csp->meta.algo);
  fprintf(f, "    \"params\": %s\n", csp->meta.paramsJSON);
  fprintf(f, "  },\n");

  if (csp->domainsSize == 0) {
    fprintf(f, "  \"domains\": [],\n");
  } else {
    fprintf(f, "  \"domains\": [\n");
    for (int iDom = 0; iDom < csp->domainsSize; ++iDom) {
      if (csp->domains[iDom].type == CJ_DOMAIN_VALUES) {
        fprintf(f, "    {\"values\": ");
        cjIntTuplesJsonPrint(f, &csp->domains[iDom].values);
        fprintf(f, "}");
      }
      else {
        return CJ_ERROR_DOMAIN_UNKNOWN_TYPE;
      }
      if (iDom != csp->domainsSize - 1) { fprintf(f, ",\n"); }
      else { fprintf(f, "\n");  }
    }
    fprintf(f, "  ],\n");
  }

  fprintf(f, "  \"vars\": ");
  cjIntTuplesJsonPrint(f, &csp->vars);
  fprintf(f, ",\n");

  if (csp->constraintDefsSize == 0) {
    fprintf(f, "  \"constraintDefs\": [],\n");
  }
  else {
    fprintf(f, "  \"constraintDefs\": [\n");
    for (int iDef = 0; iDef < csp->constraintDefsSize; ++iDef) {
      if (csp->constraintDefs[iDef].type == CJ_CONSTRAINT_DEF_NO_GOODS) {
        fprintf(f, "    {\"noGoods\": ");
        cjIntTuplesJsonPrint(f, &csp->constraintDefs[iDef].noGoods);
        fprintf(f, "}");
      }
      else {
        return CJ_CONSTRAINTDEF_UNKNOWN_TYPE;
      }
      if (iDef != csp->constraintDefsSize - 1) { fprintf(f, ",\n"); }
      else { fprintf(f, "\n");  }
    }
    fprintf(f, "  ],\n");
  }

  if (csp->constraintsSize == 0) {
    fprintf(f, "  \"constraints\": []\n");
  }
  else {
    fprintf(f, "  \"constraints\": [\n");
    for (int i = 0; i < csp->constraintsSize; ++i) {
      fprintf(f, "    {\"id\": %d, \"vars\": ", csp->constraints[i].id);
      cjIntTuplesJsonPrint(f, &csp->constraints[i].vars);
      fprintf(f, "}");
      if (i != csp->constraintsSize - 1) { fprintf(f, ",\n"); }
      else { fprintf(f, "\n");  }
    }
    fprintf(f, "  ]\n");
  }

  fprintf(f, "}\n");

  return CJ_ERROR_OK;
}


