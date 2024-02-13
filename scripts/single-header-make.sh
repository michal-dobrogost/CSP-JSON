#!/usr/bin/env bash

set -ex

CJ_DIR=$(git rev-parse --show-toplevel)
CJ_HEADER="${CJ_DIR}/cj-csp-json.h"

rm -f "${CJ_HEADER}"
cat "${CJ_DIR}/cj/jsmn.h" > "${CJ_HEADER}"
for file in 'cj-csp.h' 'cj-csp.c' 'cj-csp-io.h' 'cj-csp-io.c'; do
  cat "${CJ_DIR}/cj/${file}" | grep -v "#include \"cj-" | grep -v "#include \"jsmn.h" >> "${CJ_HEADER}"
done
