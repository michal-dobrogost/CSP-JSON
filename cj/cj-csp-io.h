#ifndef __CJ_CSP_IO_H__
#define __CJ_CSP_IO_H__

#include <stdio.h>

#include "cj-csp.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
// cjIntTuples Parsing and Printing
//

/**
 * Parse into ts which needs to be freed prior to call.
 * @arg defaultArity specifies the arity to use for an size 0 array
 *                   where you can infer the arity from the data.
 *                   Use -1 for 1D, 0+ for 2D.
 * @return CJ_ERROR_OK on success
 */
CjError cjIntTuplesParse(
  const int defaultArity,
  const char* json,
  const size_t jsonLen,
  CjIntTuples* ts);

/** Print from ts. @return CJ_ERROR_OK on success */
CjError cjIntTuplesJsonPrint(FILE* f, CjIntTuples* ts);

////////////////////////////////////////////////////////////////////////////////
// cjCsp Parsing and Printing
//

/**
 * @param json does not have to be null terminated.
 * @param jsonLen specifies the length of the json arg.
 * @param csp parse into this variable. Needs to be freed prior to call.
 * @return CJ_ERROR_OK on success
 * free CjCsp with CjCspFree().
 * */
CjError cjCspJsonParse(const char* json, const size_t jsonLen, CjCsp* csp);

/** return CJ_ERROR_OK on success */
CjError cjCspJsonPrint(FILE* f, CjCsp* csp);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __CJ_CSP_IO_H__
