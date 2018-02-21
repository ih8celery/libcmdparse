set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR})
set (CMAKE_CXX_FLAGS "-fpermissive")

add_library (optparse SHARED options_parsing.cpp)

install (TARGETS optparse DESTINATION lib)
