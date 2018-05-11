set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR})

add_executable (number "10-option-number.cpp")
target_link_libraries (number tap++ cmdparse)

add_executable (assignment "20-option-assignment.cpp")
target_link_libraries (assignment tap++ cmdparse)

add_executable (collection "30-option-collection.cpp")
target_link_libraries (collection tap++ cmdparse)

add_executable (handles "40-option-handles-name.cpp")
target_link_libraries (handles tap++ cmdparse)

add_executable (data "50-option-data.cpp")
target_link_libraries (data tap++ cmdparse)

add_executable (merged "60-merged-option.cpp")
target_link_libraries (merged tap++ cmdparse)

add_executable (bsd "70-bsd-option.cpp")
target_link_libraries (bsd tap++ cmdparse)

add_executable (subcommand "80-subcommands.cpp")
target_link_libraries (subcommand tap++ cmdparse)

add_executable (stuck "90-stuck-assignment.cpp")
target_link_libraries (stuck tap++ cmdparse)

set (CUSTOM_TEST_EXECUTABLES 
  "${EXECUTABLE_OUTPUT_PATH}/number" 
  "${EXECUTABLE_OUTPUT_PATH}/assignment" 
  "${EXECUTABLE_OUTPUT_PATH}/collection"
  "${EXECUTABLE_OUTPUT_PATH}/handles"
  "${EXECUTABLE_OUTPUT_PATH}/data"
  "${EXECUTABLE_OUTPUT_PATH}/merged"
  "${EXECUTABLE_OUTPUT_PATH}/bsd"
  "${EXECUTABLE_OUTPUT_PATH}/subcommand"
  "${EXECUTABLE_OUTPUT_PATH}/stuck"
  )

add_custom_target (debug
  COMMAND ${CUSTOM_TEST_DRIVER} ${CUSTOM_TEST_EXECUTABLES})

add_test (NAME test_number COMMAND number)
add_test (NAME test_assignment COMMAND assignment)
add_test (NAME test_collection COMMAND collection)
add_test (NAME test_handles COMMAND handles)
add_test (NAME test_data COMMAND data)
add_test (NAME test_merged COMMAND merged)
add_test (NAME test_bsd COMMAND bsd)
add_test (NAME test_sub COMMAND subcommand)
add_test (NAME test_stuck COMMAND stuck)
