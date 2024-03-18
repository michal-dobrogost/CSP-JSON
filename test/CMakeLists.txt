include(CTest)

add_executable(cj-test-cpp-compilation)
target_sources(cj-test-cpp-compilation PRIVATE cj-test-cpp-compilation.cpp ../cj/cj-csp.c ../cj/cj-csp-io.c)

add_executable(cj-test-csp)
target_sources(cj-test-csp PRIVATE cj-test-csp.c ../cj/cj-csp.c)

add_executable(cj-test-csp-io)
target_sources(cj-test-csp-io PRIVATE cj-test-csp-io.c ../cj/cj-csp.c ../cj/cj-csp-io.c)

add_executable(cj-test-roundtrip)
target_sources(cj-test-roundtrip PRIVATE cj-test-roundtrip.c test-on-files.c ../cj/cj-csp.c ../cj/cj-csp-io.c)

add_executable(cj-test-validation)
target_sources(cj-test-validation PRIVATE cj-test-validation.c test-on-files.c ../cj/cj-csp.c ../cj/cj-csp-io.c)
