cmake_minimum_required(VERSION 2.8)

project(diff-match-patch-c)

set(LIBDMP_DEPS_INSTALL ${PROJECT_BINARY_DIR}/install)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBDMP_DEPS_INSTALL}/lib/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBDMP_DEPS_INSTALL}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(test)
