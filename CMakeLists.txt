cmake_minimum_required(VERSION 2.8.9)

set(THE_PROJECT_NAME SignalX)
project(${THE_PROJECT_NAME})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)  # for organizing code to folders

# Look for our CMake modules paths
if(NOT CMAKER_ROOT OR CMAKER_ROOT STREQUAL "")
  get_filename_component(CMAKER_ROOT $ENV{CMAKER_ROOT} ABSOLUTE)
  message(STATUS "CMAKER_ROOT:${CMAKER_ROOT}")
  if(NOT CMAKER_ROOT)
    message(ERROR "Environment variable CMAKER_ROOT not set!")
  endif()
  list(APPEND CMAKE_MODULE_PATH ${CMAKER_ROOT}/cmake/modules ${CMAKER_ROOT}/cmake)
endif()

set(SignalX_DIR ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT THE_PROJECT_ROOT OR THE_PROJECT_ROOT STREQUAL "")
  set(THE_PROJECT_ROOT ${SignalX_DIR})
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
add_definitions(-DSignalX_DIR="${SignalX_DIR}")

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(SignalX_CXX_FLAGS " -stdlib=libstdc++ ")  # prevent default libc++ on new mac
endif()
if(NOT WIN32)
  set(SignalX_CXX_FLAGS " ${SignalX_CXX_FLAGS} -std=c++11 ")
endif()

set(CMAKE_CXX_FLAGS ${SignalX_CXX_FLAGS})
cmaker_common_build_setting()

## SignalX library ===============================================================
set(SignalX_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${SignalX_INCLUDE_DIRS})
file(GLOB signalx_src ${CMAKE_CURRENT_SOURCE_DIR}/signalx/*.h)
add_library(SignalX ${signalx_src})
set_property(TARGET SignalX PROPERTY FOLDER "signalx_libs")
set_property(TARGET SignalX PROPERTY LINKER_LANGUAGE CXX)

add_subdirectory(test)