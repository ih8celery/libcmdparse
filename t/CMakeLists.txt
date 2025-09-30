set(cmdparse_tests
  number            10-option-number.cpp
  assignment        20-option-assignment.cpp
  collection        30-option-collection.cpp
  handles           40-option-handles-name.cpp
  data              50-option-data.cpp
  merged            60-merged-option.cpp
  bsd               70-bsd-option.cpp
  subcommand        80-subcommands.cpp
  stuck             90-stuck-assignment.cpp
)

set(test_targets)
set(test_paths)

list(LENGTH cmdparse_tests test_count)
while(test_count GREATER 0)
  list(POP_FRONT cmdparse_tests test_name test_source)

  add_executable(${test_name} "${test_source}")
  target_link_libraries(${test_name} PRIVATE tap++ cmdparse)

  add_test(NAME ${test_name} COMMAND ${test_name})

  list(APPEND test_targets ${test_name})
  list(APPEND test_paths "$<TARGET_FILE:${test_name}>")

  list(LENGTH cmdparse_tests test_count)
endwhile()

if(cmdparse_CUSTOM_TEST_DRIVER)
  add_custom_target(debug
    COMMAND ${cmdparse_CUSTOM_TEST_DRIVER} ${test_paths}
    DEPENDS ${test_targets}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
endif()
