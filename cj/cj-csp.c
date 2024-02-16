#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "cj-csp.h"

CjIntTuples cjIntTuplesInit() {
  CjIntTuples x;
  x.size = 0;
  x.arity = 0;
  x.data = NULL;
  return x;
}

CjError cjIntTuplesAlloc(int size, int arity, CjIntTuples* out) {
  if (size < 0 || arity < -1) {
    return CJ_ERROR_ARG;
  }
  out->size = size;
  out->arity = arity;
  out->data = NULL;
  if (size > 0 && abs(arity) > 0) {
    out->data = (int*) malloc(sizeof(int) * size * abs(arity));
    if (!out->data) {
      *out = cjIntTuplesInit();
      return CJ_ERROR_NOMEM;
    }
  }
  return CJ_ERROR_OK;
}

void cjIntTuplesFree(CjIntTuples* inout) {
  if (!inout) { return; }
  free(inout->data);
  inout->data = NULL;
  inout->arity = 0;
  inout->size = 0;
}

CjIntTuples* cjIntTuplesArray(int size) {
  CjIntTuples* xs = (CjIntTuples*) malloc(sizeof(CjIntTuples) * size);
  if (!xs) { return NULL; }
  for (int i = 0; i < size; ++i) {
    xs[i] = cjIntTuplesInit();
  }
  return xs;
}

void cjIntTuplesArrayFree(CjIntTuples** inout, int size) {
  if (!inout) { return; }
  if (!(*inout)) { return; }
  for (int i = 0; i < size; ++i) {
    cjIntTuplesFree(&((*inout)[i]));
  }
  free(*inout);
  *inout = NULL;
}

CjMeta cjMetaInit() {
  CjMeta x;
  x.id = NULL;
  x.algo = NULL;
  x.paramsJSON = NULL;
  return x;
}

void cjMetaFree(CjMeta* inout) {
  if (!inout) { return; }
  free(inout->id);
  free(inout->algo);
  free(inout->paramsJSON);
  *inout = cjMetaInit();
}

CjDomain cjDomainInit() {
  CjDomain x;
  x.type = CJ_DOMAIN_UNDEF;
  return x;
}

CjError cjDomainValuesAlloc(int size, CjDomain* out) {
  if (!out) { return CJ_ERROR_ARG; }
  const int arity = -1;
  out->type = CJ_DOMAIN_VALUES;
  int stat = cjIntTuplesAlloc(size, arity, &out->values);
  if (stat != CJ_ERROR_OK) {
    *out = cjDomainInit();
    return stat;
  }
  return CJ_ERROR_OK;
}

void cjDomainFree(CjDomain* inout) {
  if (!inout) { return; }
  switch (inout->type) {
    case CJ_DOMAIN_VALUES:
      cjIntTuplesFree(&inout->values);
    default:
      assert(0);
      break;
  }
  inout->type = CJ_DOMAIN_UNDEF;
}

CjDomain* cjDomainArray(int size) {
  CjDomain* xs = (CjDomain*) malloc(sizeof(CjDomain) * size);
  if (!xs) { return NULL; }
  for (int i = 0; i < size; ++i) {
    xs[i] = cjDomainInit();
  }
  return xs;
}

void cjDomainArrayFree(CjDomain** inout, int size) {
  if (!inout) { return; }
  if (!(*inout)) { return; }
  for (int i = 0; i < size; ++i) {
    cjDomainFree(&((*inout)[i]));
  }
  free(*inout);
  *inout = NULL;
}

CjConstraintDef cjConstraintDefInit() {
  CjConstraintDef x;
  x.type = CJ_CONSTRAINT_DEF_UNDEF;
  return x;
}

CjError cjConstraintDefNoGoodAlloc(int arity, int size, CjConstraintDef* out) {
  if (!out) { return CJ_ERROR_ARG; }
  out->type = CJ_CONSTRAINT_DEF_NO_GOODS;
  int stat = cjIntTuplesAlloc(size, arity, &out->noGoods);
  if (stat != CJ_ERROR_OK) { return stat; }
  return CJ_ERROR_OK;
}

void cjConstraintDefFree(CjConstraintDef* inout) {
  if (!inout) { return; }
  switch (inout->type) {
    case CJ_CONSTRAINT_DEF_NO_GOODS:
      cjIntTuplesFree(&inout->noGoods);
    default:
      assert(0);
      break;
  }
}

CjConstraintDef* cjConstraintDefArray(int size) {
  CjConstraintDef* xs = (CjConstraintDef*) malloc(sizeof(CjConstraintDef) * size);
  if (!xs) { return NULL; }
  for (int i = 0; i < size; ++i) {
    xs[i] = cjConstraintDefInit();
  }
  return xs;
}

void cjConstraintDefArrayFree(CjConstraintDef** inout, int size) {
  if (!inout) { return; }
  if (!(*inout)) { return; }
  for (int i = 0; i < size; ++i) {
    cjConstraintDefFree(&((*inout)[i]));
  }
  free(*inout);
  *inout = NULL;
}

CjConstraint cjConstraintInit() {
  CjConstraint x;
  x.id = -1;
  x.vars = cjIntTuplesInit();
  return x;
}

CjError cjConstraintAlloc(int size, CjConstraint* out) {
  const int arity = -1;
  if (!out) { return CJ_ERROR_ARG; }
  out->id = -1;
  int stat = cjIntTuplesAlloc(size, arity, &out->vars);
  if (stat != CJ_ERROR_OK) { return stat; }
  return CJ_ERROR_OK;
}

void cjConstraintFree(CjConstraint* inout) {
  if (!inout) { return; }
  inout->id = -1;
  cjIntTuplesFree(&inout->vars);
}

CjConstraint* cjConstraintArray(int size) {
  CjConstraint* xs = (CjConstraint*) malloc(sizeof(CjConstraint) * size);
  if (!xs) { return NULL; }
  for (int i = 0; i < size; ++i) {
    xs[i] = cjConstraintInit();
  }
  return xs;
}

void cjConstraintArrayFree(CjConstraint** inout, int size) {
  if (!inout) { return; }
  if (!(*inout)) { return; }
  for (int i = 0; i < size; ++i) {
    cjConstraintFree(&((*inout)[i]));
  }
  free(*inout);
  *inout = NULL;
}

CjCsp cjCspInit() {
  CjCsp x;

  x.meta = cjMetaInit();

  x.domainsSize = 0;
  x.domains = NULL;

  x.vars = cjIntTuplesInit();

  x.constraintDefsSize = 0;
  x.constraintDefs = NULL;

  x.constraintsSize = 0;
  x.constraints = NULL;

  return x;
}

void cjCspFree(CjCsp* inout) {
  if (!inout) { return; }
  cjMetaFree(&inout->meta);
  cjDomainArrayFree(&inout->domains, inout->domainsSize);
  cjConstraintDefArrayFree(&inout->constraintDefs, inout->constraintDefsSize);
  cjConstraintArrayFree(&inout->constraints, inout->constraintsSize);
  *inout = cjCspInit();
}

CjError cjCspValidate(const CjCsp* csp) {
  if (!csp) { return CJ_ERROR_ARG; }

  // Check domains
  if (csp->domainsSize < 0) { return CJ_ERROR_VALIDATION_DOMAINS_SIZE; }
  for (int iDom = 0; iDom < csp->domainsSize; ++iDom) {
    if (csp->domains[iDom].type <= CJ_DOMAIN_UNDEF) { return CJ_ERROR_VALIDATION_DOMAINS_TYPE; }
    if (csp->domains[iDom].type >= CJ_DOMAIN_SIZE) { return CJ_ERROR_VALIDATION_DOMAINS_TYPE; }
  }

  // Check vars
  if (csp->vars.arity != -1) { return CJ_ERROR_VALIDATION_VARS_ARITY; }
  if (csp->vars.size < 0) { return CJ_ERROR_VALIDATION_VARS_SIZE; }
  for (int iVar = 0; iVar < csp->vars.size; ++iVar) {
    if (csp->vars.data[iVar] < 0) { return CJ_ERROR_VALIDATION_VAR_RANGE; }
    if (csp->domainsSize <= csp->vars.data[iVar]) { return CJ_ERROR_VALIDATION_VAR_RANGE; }
  }

  // Check constraintDefs
  if (csp->constraintDefsSize < 0) { return CJ_ERROR_VALIDATION_CONSTRAINTDEFS_SIZE; }
  for (int iCDef = 0; iCDef < csp->constraintDefsSize; ++iCDef) {
    if (csp->constraintDefs[iCDef].type <= CJ_CONSTRAINT_DEF_UNDEF) { return CJ_ERROR_VALIDATION_CONSTRAINTDEF_TYPE; }
    if (csp->constraintDefs[iCDef].type >= CJ_CONSTRAINT_DEF_SIZE) { return CJ_ERROR_VALIDATION_CONSTRAINTDEF_TYPE; }
  }

  // Check constraints
  if (csp->constraintsSize < 0) { return CJ_ERROR_VALIDATION_CONSTRAINTS_SIZE; }
  for (int iC = 0; iC < csp->constraintsSize; ++iC) {
    if (csp->constraints[iC].id < 0) { return CJ_ERROR_VALIDATION_CONSTRAINT_ID_RANGE; }
    if (csp->constraints[iC].id >= csp->constraintDefsSize) { return CJ_ERROR_VALIDATION_CONSTRAINT_ID_RANGE; }
    if (csp->constraints[iC].vars.arity != -1) { return CJ_ERROR_VALIDATION_CONSTRAINT_VARS_ARITY; }
    if (csp->constraints[iC].vars.size < 0) { return CJ_ERROR_VALIDATION_CONSTRAINT_VARS_SIZE; }
    for (int iVar = 0; iVar < csp->constraints[iC].vars.size; ++iVar) {
      if (csp->constraints[iC].vars.data[iVar] < 0) { return CJ_ERROR_VALIDATION_CONSTRAINT_VAR_RANGE; }
      if (csp->constraints[iC].vars.data[iVar] >= csp->vars.size) { return CJ_ERROR_VALIDATION_CONSTRAINT_VAR_RANGE; }
    }
  }

  return CJ_ERROR_OK;
}
