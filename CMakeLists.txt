cmake_minimum_required(VERSION 2.8.7)

set(THE_PROJECT_NAME SignalX)
project(${THE_PROJECT_NAME})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)  # for organizing code to folders

# Look for our CMake modules paths
set(CMAKER_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/external/cmaker)
if(NOT EXISTS "${CMAKER_ROOT}/cmake/CMakerCommonUtil.cmake")
  message(FATAL_ERROR "cmaker folder not exist")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKER_ROOT}/cmake
                              ${CMAKER_ROOT}/cmake/modules
                              ${CMAKE_CURRENT_SOURCE_DIR}/cmake
                              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
                              ${CMAKE_CURRENT_BINARY_DIR})

set(SIGNALX_ROOT ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
if(NOT THE_PROJECT_ROOT OR THE_PROJECT_ROOT STREQUAL "")
  set(THE_PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
endif()

include(CMakerCommonUtil)  # include useful macros
include(CMakerCommonSettings)

set(SignalX_WORLD_VERSION 1)
set(SignalX_MAJOR_VERSION 0)
set(SignalX_MINOR_VERSION 0)
cmaker_print_status("Welcome! SignalX version ${SignalX_COLOR_BLUE}${SignalX_WORLD_VERSION}.${SignalX_MAJOR_VERSION}.${SignalX_MINOR_VERSION}${SignalX_COLOR_RESET}")
cmaker_print_status("SignalX cmake module paths: ${SignalX_COLOR_BLUE}${CMAKE_MODULE_PATH}${SignalX_COLOR_RESET}")
cmaker_detect_os()

set(SignalX_CXX_FLAGS)
add_definitions(-DSIGNALX_ROOT="${SIGNALX_ROOT}")

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(SignalX_CXX_FLAGS " -stdlib=libstdc++ ")  # prevent default libc++ on new mac
endif()
if(NOT WIN32)
  set(SignalX_CXX_FLAGS " ${SignalX_CXX_FLAGS} -std=c++11 ")
endif()

set(CMAKE_CXX_FLAGS ${SignalX_CXX_FLAGS})
cmaker_common_build_setting()

## SignalX library ============================================================
set(SignalX_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${SignalX_INCLUDE_DIRS})
file(GLOB signalx_src ${CMAKE_CURRENT_SOURCE_DIR}/signalx/*.h)
set(ALL_LIBRARIES )
add_library(SignalX ${signalx_src})
set_property(TARGET SignalX PROPERTY FOLDER "signalx_libs")
set_property(TARGET SignalX PROPERTY LINKER_LANGUAGE CXX)

add_subdirectory(test)