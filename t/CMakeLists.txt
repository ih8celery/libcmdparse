set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR})

add_executable (number "10-option-number.cpp")
target_link_libraries (number tap++ optparse)

add_executable (assignment "20-option-assignment.cpp")
target_link_libraries (assignment tap++ optparse)

add_executable (collection "30-option-collection.cpp")
target_link_libraries (collection tap++ optparse)

add_executable (handles "40-option-handles-name.cpp")
target_link_libraries (handles tap++ optparse)

add_executable (data "50-option-data.cpp")
target_link_libraries (data tap++ optparse)

set (CUSTOM_TEST_EXECUTABLES 
  "${EXECUTABLE_OUTPUT_PATH}/number" 
  "${EXECUTABLE_OUTPUT_PATH}/assignment" 
  "${EXECUTABLE_OUTPUT_PATH}/collection"
  "${EXECUTABLE_OUTPUT_PATH}/handles"
  "${EXECUTABLE_OUTPUT_PATH}/data")

add_custom_target (debug
  COMMAND ${CUSTOM_TEST_DRIVER} ${CUSTOM_TEST_EXECUTABLES})

add_test (NAME test_number COMMAND number)
add_test (NAME test_assignment COMMAND assignment)
add_test (NAME test_collection COMMAND collection)
add_test (NAME test_handles COMMAND handles)
add_test (NAME test_data COMMAND data)
