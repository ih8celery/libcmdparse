set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR})

add_executable (test_sanity "00-sanity.cpp")
target_link_libraries (test_sanity tap++ optparse)

add_executable (test_number "10-option-number.cpp")
target_link_libraries (test_number tap++ optparse)

add_executable (test_assignment "20-option-assignment.cpp")
target_link_libraries (test_assignment tap++ optparse)

add_executable (test_collection "30-option-collection.cpp")
target_link_libraries (test_collection tap++ optparse)

add_executable (test_handles "40-option-handles-name.cpp")
target_link_libraries (test_handles tap++ optparse)

add_executable (test_data "50-option-data.cpp")
target_link_libraries (test_data tap++ optparse)

set (CUSTOM_TEST_EXECUTABLES 
  "${EXECUTABLE_OUTPUT_PATH}/test_sanity" 
  "${EXECUTABLE_OUTPUT_PATH}/test_number" 
  "${EXECUTABLE_OUTPUT_PATH}/test_assignment" 
  "${EXECUTABLE_OUTPUT_PATH}/test_collection"
  "${EXECUTABLE_OUTPUT_PATH}/test_handles"
  "${EXECUTABLE_OUTPUT_PATH}/test_data")

add_custom_target (run_tests
  COMMAND ${CUSTOM_TEST_DRIVER} ${CUSTOM_TEST_EXECUTABLES})
