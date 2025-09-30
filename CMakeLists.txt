cmake_minimum_required(VERSION 3.15)

project(cmdparse VERSION 0.3.2 LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(CMDPARSE_BUILD_TESTS "Build libcmdparse tests" ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(cmdparse_CUSTOM_TEST_DRIVER "${CMAKE_CURRENT_SOURCE_DIR}/lib/tap")

add_library(cmdparse)

add_subdirectory(lib/src)

target_compile_features(cmdparse PUBLIC cxx_std_17)
target_include_directories(cmdparse
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(cmdparse PROPERTIES
  EXPORT_NAME cmdparse
  OUTPUT_NAME cmdparse
)

if(BUILD_SHARED_LIBS AND WIN32)
  set_target_properties(cmdparse PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(CMDPARSE_BUILD_TESTS)
  include(CTest)
  add_subdirectory(t)
endif()

install(TARGETS cmdparse
  EXPORT cmdparseTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY lib/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(cmdparse_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/cmdparse" CACHE PATH "CMake config install dir")

install(EXPORT cmdparseTargets
  FILE cmdparseTargets.cmake
  NAMESPACE cmdparse::
  DESTINATION ${cmdparse_INSTALL_CMAKEDIR}
)

configure_package_config_file(
  cmake/cmdparseConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/cmdparseConfig.cmake"
  INSTALL_DESTINATION ${cmdparse_INSTALL_CMAKEDIR}
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmdparseConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/cmdparseConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/cmdparseConfigVersion.cmake"
  DESTINATION ${cmdparse_INSTALL_CMAKEDIR}
)

configure_file(cmake/cmdparse.pc.in "${CMAKE_CURRENT_BINARY_DIR}/cmdparse.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cmdparse.pc" DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

add_custom_target(tags COMMAND ctags "." WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
