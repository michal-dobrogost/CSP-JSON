cmake_minimum_required(VERSION 3.5)

project(csp-json)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

add_subdirectory(tools/cj-echo)
add_subdirectory(tools/cj-gen-urbcsp)
add_subdirectory(tools/cj-is-solved)
add_subdirectory(tools/cj-validate)

install(TARGETS cj-echo cj-gen-urbcsp cj-is-solved cj-validate DESTINATION .)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(test)
  add_test(NAME cj-test-cpp-compilation COMMAND $<TARGET_FILE:cj-test-cpp-compilation>)
  add_test(NAME cj-test-csp COMMAND $<TARGET_FILE:cj-test-csp>)
  add_test(NAME cj-test-csp-io COMMAND $<TARGET_FILE:cj-test-csp-io>)
  add_test(NAME cj-test-roundtrip COMMAND $<TARGET_FILE:cj-test-roundtrip> ${PROJECT_SOURCE_DIR}/data)
  add_test(NAME cj-test-validation COMMAND $<TARGET_FILE:cj-test-validation> ${PROJECT_SOURCE_DIR})

  find_program(PYTHON3 NAMES "python3")
  if(PYTHON3)
    add_test(NAME cj-echo COMMAND pytest -v "${CMAKE_CURRENT_SOURCE_DIR}/test-exe/test_cj-echo.py" --exe $<TARGET_FILE:cj-echo>)
    add_test(NAME cj-is-solved COMMAND pytest -v "${CMAKE_CURRENT_SOURCE_DIR}/test-exe/test_cj-is-solved.py" --exe $<TARGET_FILE:cj-is-solved>)
    add_test(NAME cj-validate COMMAND pytest -v "${CMAKE_CURRENT_SOURCE_DIR}/test-exe/test_cj-validate.py" --exe $<TARGET_FILE:cj-validate>)
    add_test(NAME cj-gen-urbcsp COMMAND pytest -v "${CMAKE_CURRENT_SOURCE_DIR}/test-exe/test_cj-gen-urbcsp.py" --exe $<TARGET_FILE:cj-gen-urbcsp>)
  else()
    message(WARNING "python3 not found in your environment. Tools will not be tested.")
  endif()
endif()
