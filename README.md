# Constraint Satisfaction Problems in JSON

csp-json specifies a simple JSON schema for describing integer binary CSP instances. It provides a C library for printing to and parsing from the json format and C datastructures. Command line tools are provided for generating random CSP instances and verifying csp-json files.

Goals:
1. Use a dataformat to be language agnostic and easy to verify (vs. runtime instances generators such as [urbcsp](https://www.lirmm.fr/~bessiere/generator.html)).
2. Use a standard syntax (JSON) to leverage standard parsing libraries and tools (vs. custom syntax in [minizinc](minizinc.org)).
3. Prefer simple and explicit data-listing semantics to provide a lowest common denominator interchange format (vs. higher level constructs).
4. Enable a tool ecosystem such as verifying a solutions against a CSP instance, problem decomposition or rewrite tools, etc.
5. Allow future extensions to non-binary CSPs or global constraints such as all-different.
6. Centralize an archive of CSP problem instances (see the [csp-json-archive](https://github.com/michal-dobrogost/csp-json-archive) github repo). Instances added there will never be deleted and can be safely referenced.

This project is licensed under the terms of the [MIT license](https://github.com/michal-dobrogost/csp-json/blob/main/LICENSE.txt).

# Dataformat

## The csp-json instance schema

The format stays close to the mathematical roots of CSP problems (domains, veriables and constraints). Preference is given to explicit data listing as opposed to higher level constructs for simplicity and to maintain flexibility. It can easily be extended to non-binary CSPs. An implementation of the urbcsp CSP generator outputing to this format is included.

Below is an example of the JSON schema for encoding binary CSP instances:
* `meta` is not needed by solvers but it helps with data analysis. *Don't skip it*.
  * `id` is a unique string for this CSP instance. It should be a combination of the *algo* and *params* fields.
  * `algo` specifies the CSP generator algorithm.
  * `params` specifies the CSP generator parameters (can be a string, object or array).
* `domains` lists the starting domains (the values that are allowed for each variable).
* `vars` has one entry per variable in the CSP and each entry references one of the domains by 0-based index.
* `constraintsDef` defines the constraints used in the CSP.
  * `noGoods` is a constraint defined by listing the combination of values for a pair of variables that is not allowed.
* `constraints` is a list of constraints between variables.
  * `constraint` references one of the constraints in *constraintsDef* by 0-based index.
  * `vars` references a pair of variables by 0-based index.

```JSON
{
  "meta": {
    "id": "human/color-australia",
    "algo": "human",
    "params": null
  },
  "domains": [
    {"values": [0, 1, 2]}
  ],
  "vars": [0, 0, 0, 0, 0, 0, 0],
  "constraintDefs": [
    {"noGoods": [[0, 0], [1, 1], [2, 2]]}
  ],
  "constraints": [
    {"id": 0, "vars": [0, 1]},
    {"id": 0, "vars": [0, 2]},
    {"id": 0, "vars": [1, 2]},
    {"id": 0, "vars": [1, 3]},
    {"id": 0, "vars": [2, 3]},
    {"id": 0, "vars": [2, 4]},
    {"id": 0, "vars": [2, 5]},
    {"id": 0, "vars": [3, 4]},
    {"id": 0, "vars": [4, 5]}
  ]
}
```

Sub-objects define their own types. For example, `noGoods` in `constraintDefs` defines a constraint by the combinations of values that are not allowed and self identifies as such by using the `"noGoods"` key. This allows extending the format with additional datatypes in the future. Support for an all-different constraint might be added in the future like this:
```JSON
{
  "constraintDefs": [
    {"all-different": null}
  ],
}
```

# C Library

## Including the library

The csp-json C library is most easily integrated into your project by copying the source files directly into your source tree (or via git submodule). There are two ways of integrating csp-json into your build:

1. ([cj-csp-json.h](https://github.com/michal-dobrogost/csp-json/blob/main/cj-csp-json.h)) is a single monolithic header file for bootstrapping a project. Just include the header in your main.c, no need to link.
```C
#include "cj-csp-json.h"
```
2. The [cj directory](https://github.com/michal-dobrogost/csp-json/blob/main/cj)) contains separate .h and .c files for projects that reference the headers in multiple files or that need to build a library. You will need to link against cj-csp.c and optionally cj-csp-io.c.
```C
#include <cj/cj-csp.h>    // For C Datastructures
#include <cj/cj-csp-io.h> // For print/parse functionality
```

The two options provide the exact same declarations and implementations.

## Datastructures

The csp-json C library provides datastructures for representing CSPs. See [cj-csp.h](https://github.com/michal-dobrogost/csp-json/blob/main/cj/cj-csp.h).

The top-level structure closely follows the JSON schema:
```C
typedef struct CjCsp {
  CjMeta meta;

  int domainsSize;
  CjDomain* domains;

  /** Each variable references a domain above. Arity is 1. */
  CjIntTuples vars;

  int constraintDefsSize;
  CjConstraintDef* constraintDefs;

  int constraintsSize;
  CjConstraint* constraints;
} CjCsp;
```

Sub-objects define their types by using an enum and a union type. The enum identifies the constraintDef type (similarly to the JSON key: `noGoods`) while the union specifies type-specific fields (similarly to the JSON value: `[[0, 0], [1, 1], [2, 2]]`). This approach is extensible to new constraintDef types in the future (such as all-different).

This example initializes the noGood constraintDef for the color-australia problem:
```C
CjConstraintDef c = cjConstraintDefInit();
c.type = CJ_CONSTRAINT_DEF_NO_GOODS;
int status = cjIntTuplesAlloc(3 /* size */, 2 /* arity */, &c.noGoods);
if (status != 0) { /* handle errors */ }

c.noGoods.data[0 /* tuple */ + 0 /* element */] = 0;
c.noGoods.data[0 /* tuple */ + 1 /* element */] = 0;
c.noGoods.data[1 /* tuple */ + 0 /* element */] = 1;
c.noGoods.data[1 /* tuple */ + 1 /* element */] = 1;
c.noGoods.data[2 /* tuple */ + 0 /* element */] = 2;
c.noGoods.data[2 /* tuple */ + 1 /* element */] = 2;
```

The `CjIntTuples` type is used throughout and can represent both:
1. 1D arrays (like `[1, 2, 3]`) when `arity = -1`
2. 2D arrays of a fixed arity (like `[[1, 2], [3, 4]]` when `arity >= 0`.

## Parsing

Functionality for printing a CjCsp structure to a JSON string and parsing a JSON string to a CjCsp structure is provided in [cj-csp-io.h](https://github.com/michal-dobrogost/csp-json/blob/main/cj/cj-csp-io.h))
. A good example is available in [cj-echo/main.c](https://github.com/michal-dobrogost/csp-json/blob/main/cj-echo/main.c)). Focus on:
```C
int cjCspJsonParse(const char* json, const size_t jsonLen, CjCsp* csp);
int cjCspJsonPrint(FILE* f, CjCsp* csp);
```

# Building Tools / Testing

## Build using Nix + CMake

1. Install the [nix package manager](https://nixos.org/download). This will automatically handle enabling extra dependancies such as CMake without installing them globally on your system.
2. Invoke [./scripts/build.sh](https://github.com/michal-dobrogost/csp-json/blob/main/scripts/build.sh) in the main directory.

This will populate the `build` directory as well as `${HOME}/exe`.

## Build using CMake

Use CMake to build the project. See [./scripts/build.sh](https://github.com/michal-dobrogost/csp-json/blob/main/scripts/build.sh) for hints on how to invoke.

This will populate the `build` directory.

## Testing using Nix + CMake

1. Install the [nix package manager](https://nixos.org/download). This will automatically handle enabling extra dependancies such as CMake without installing them globally on your system.
2. Invoke [./scripts/test.sh](https://github.com/michal-dobrogost/csp-json/blob/main/scripts/test.sh) in the main directory.

## Testing using CMake

Use CMake to test the project. See [./scripts/test.sh](https://github.com/michal-dobrogost/csp-json/blob/main/scripts/test.sh) for hints on how to invoke.

# Tools

## Instance Generators

See the [cj-gen-urbcsp](https://github.com/michal-dobrogost/csp-json/blob/main/tools/cj-gen-urbcsp) folder or binary for a random binary CSP instance generator.

## Verification

See the [cj-validate](https://github.com/michal-dobrogost/csp-json/blob/main/tools/cj-validate) tool which takes a csp-json as input and verifies the values, for exapmle that values are in range given other fields in the JSON.

See the [cj-echo](https://github.com/michal-dobrogost/csp-json/blob/main/tools/cj-echo) tool which takes a csp-json as input and outputs a pretty-printed version. This will validate the parsing phase only.

# Contributing

* Look around the code to maintain a consistent style.
* Make changes in the [cj directory](https://github.com/michal-dobrogost/csp-json/blob/main/cj)). Use [scripts/single-header-make.sh](https://github.com/michal-dobrogost/csp-json/blob/main/scripts/single-header-make.sh)) to generate the standalone header file.
* Run tests with [scripts/test.sh](https://github.com/michal-dobrogost/csp-json/blob/main/scripts/test.sh).
* Push to a branch and create a pull request for review.

## TODO

### AllDifferent

```
Add an enum for representing a global constraint like all different which does
not require an additional parameterization (although perhaps params like
filtering string (eg. arc-consistency) is useful?).

Possible ways to implement:
  (1) "constraints": [
        {"def": 0, "vars": [0, 1]},
        {"def": 1, "vars": [0, 2]},
        {"def": "allDiff", "vars": [1, 2]}
      ]
  (2) "constraints": [
        {"id": 0, "vars": [0, 1]},
        {"id": 1, "vars": [0, 2]},
        {"def": "allDiff", "vars": [1, 2]}
      ]
  (3) "constraints": [
        {"id": 0, "vars": [0, 1]},
        {"id": 1, "vars": [0, 2]},
        {"id": 2, "vars": [1, 2]}  // {"allDiff": null} in constraintsDef
      ]

A PR with an addition like this including changes to all the required places
(struct, parsing, printing, tests, etc) would be a good example for adding other
changes.
```
