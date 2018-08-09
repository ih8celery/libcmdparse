cmake_minimum_required (VERSION 3.5)

project (Cmdparse LANGUAGES CXX)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set (HOME ${CMAKE_CURRENT_LIST_DIR})
set (CUSTOM_TEST_DRIVER "${HOME}/lib/tap")
set (PROJECT_HEADERS "${HOME}/lib/include")
set (CTAGS_FILE "${HOME}/tags")

enable_testing ()

include_directories (${PROJECT_HEADERS})

add_subdirectory (lib/src)
add_subdirectory (t)

add_custom_target (tags COMMAND ctags "." WORKING_DIRECTORY ${HOME})

install (FILES "${PROJECT_HEADERS}/cmdparse.h"
         DESTINATION include)
