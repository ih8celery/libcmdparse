include_directories ("${PROJECT_SOURCE_DIR}/lib/include")

set (CMAKE_CXX_FLAGS "-fpermissive")

add_library (optparse SHARED options_parsing.cpp)
