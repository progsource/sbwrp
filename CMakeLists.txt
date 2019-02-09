# Copyright 2019 Petra Baranski (progsource)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.6)

if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

# ------------------------------------------------------------------------------

project(sbwrp
  VERSION 0.1.0
  DESCRIPTION "SDL with bgfx wrapper"
  HOMEPAGE_URL "http://github.com/progsource/sbwrp"
)

message(STATUS "- Build sbwrp version ${CMAKE_PROJECT_VERSION}")
message(STATUS "- CMake build type ${CMAKE_BUILD_TYPE}")

# ------------------------------------------------------------------------------

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(SBWRP_ISROOT true)
  message(STATUS "- sbwrp is build as main project")
else()
  set(SBWRP_ISROOT false)
  message(STATUS "- sbwrp is build as sub project")
endif()

# ------------------------------------------------------------------------------

if (${SBWRP_ISROOT} OR NOT ${CMAKE_CXX_STANDARD})
  set(CMAKE_CXX_STANDARD 14)
endif()

if (${CMAKE_CXX_STANDARD} LESS "14")
  message(ERROR "sbwrp needs at least C++14 to compile")
endif()

# ------------------------------------------------------------------------------

if (UNIX AND NOT APPLE)
  execute_process(COMMAND ${CMAKE_CXX_COMPILER}
    -fuse-ld=gold -Wl,--version
    ERROR_QUIET OUTPUT_VARIABLE ld_version
  )

  if ("${ld_version}" MATCHES "GNU gold")
    message(STATUS "- Found Gold linker, use faster linker")
    set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold"
    )
    set(CMAKE_SHARED_LINKER_FLAGS
      "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold "
    )
  endif()
endif()

# ------------------------------------------------------------------------------

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# ------------------------------------------------------------------------------

set(sbwrp_SDL_VERSION 2.0.9)

message(STATUS "- add dependencies")
add_subdirectory(libs)
message(STATUS "- dependencies done")

# ------------------------------------------------------------------------------

message(STATUS "- configure sbwrp")

add_library(sbwrp STATIC
  ${PROJECT_SOURCE_DIR}/src/sbwrp.cpp
)

set_target_properties(sbwrp PROPERTIES COMPILE_FLAGS
  "-fno-rtti -fno-exceptions -fno-omit-frame-pointer -Wall -Wpedantic -Wextra -Wno-ignored-qualifiers"
)

target_include_directories(sbwrp PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(sbwrp PUBLIC
  SDL2-static
  bgfx
)

set_target_properties(sbwrp PROPERTIES FOLDER "sbwrp")

# ------------------------------------------------------------------------------

add_executable(sbwrp-example
  ${PROJECT_SOURCE_DIR}/example/main.cpp
)

set_target_properties(sbwrp-example PROPERTIES COMPILE_FLAGS
  "-fno-rtti -fno-exceptions -fno-omit-frame-pointer -Wall -Wpedantic -Wextra -Wno-ignored-qualifiers"
)

target_link_libraries(sbwrp-example PRIVATE
  sbwrp
)

set_target_properties(sbwrp-example PROPERTIES FOLDER "sbwrp-example")
