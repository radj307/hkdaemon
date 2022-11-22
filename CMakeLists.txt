# hkdaemon/
cmake_minimum_required (VERSION 3.22)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/307lib/307modules")

set(ENV{hkdaemon_VERSION} "0.0.0")
include(VersionTag)
GET_VERSION_TAG("${CMAKE_CURRENT_SOURCE_DIR}" "hkdaemon")

project("hkdaemon" VERSION "${hkdaemon_VERSION}" LANGUAGES CXX)

add_subdirectory("307lib")
add_subdirectory("hkdaemon")
