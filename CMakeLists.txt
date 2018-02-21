cmake_minimum_required (VERSION 3.0)

project (Optparse LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)
set (CUSTOM_TEST_DRIVER "${CMAKE_CURRENT_LIST_DIR}/lib/tap")
set (PROJECT_HEADERS "${CMAKE_CURRENT_LIST_DIR}/lib/include")

include_directories (${PROJECT_HEADERS})

add_subdirectory (lib/src)
add_subdirectory (t)

install (FILES "${PROJECT_HEADERS}/options_parsing.h"
         DESTINATION include)
