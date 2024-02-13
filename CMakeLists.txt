cmake_minimum_required(VERSION 3.5)

project(csp-json)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

add_subdirectory(tools/cj-echo)
add_subdirectory(tools/cj-validate)

install(TARGETS cj-echo cj-validate DESTINATION .)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(test)
  add_test(NAME cj-test-csp COMMAND $<TARGET_FILE:cj-test-csp>)
  add_test(NAME cj-test-csp-io COMMAND $<TARGET_FILE:cj-test-csp-io>)
  add_test(NAME cj-test-roundtrip COMMAND $<TARGET_FILE:cj-test-roundtrip> ${PROJECT_SOURCE_DIR}/data)
  add_test(NAME cj-test-validation COMMAND $<TARGET_FILE:cj-test-validation> ${PROJECT_SOURCE_DIR})
endif()
