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

# SDL ##########################################################################

if (UNIX AND NOT APPLE)
  set(VIDEO_X11 ON CACHE BOOL "enable X11 for unix")
  set(VIDEO_DIRECTFB OFF CACHE BOOL "disable directfb for unix, because bgfx doesn't have it")
endif()

if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SDL2-${sbwrp_SDL_VERSION}/include/")
  if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SDL2-${sbwrp_SDL_VERSION}.tar.gz")
    execute_process(
      COMMAND wget https://www.libsdl.org/release/SDL2-${sbwrp_SDL_VERSION}.tar.gz
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
  endif()

  execute_process(
    COMMAND tar -zvxf ${CMAKE_CURRENT_SOURCE_DIR}/SDL2-${sbwrp_SDL_VERSION}.tar.gz
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
endif()

add_subdirectory(SDL2-${sbwrp_SDL_VERSION})

# bgfx #########################################################################

if (UNIX AND NOT APPLE)
  set(bgfx_LIBRARY_IMPORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/.build/linux64_gcc/bin)
else()
  message(ERROR "Your target operating system is not yet supported - fix it in libs/CMakeListst.txt")
endif()

set(bgfx_LIBRARIES "")

function(bgfx_dep_target)
  set(options OPTIONAL)
  set(oneValueArgs TARGET)
  set(multiValueArgs INCLUDE LIBRARIES BGFX_LIBRARIES)
  cmake_parse_arguments(BGFX_DEP
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
  )

  message(STATUS "BGFX_DEP_TARGET ${BGFX_DEP_TARGET}")
  message(STATUS "BGFX_DEP_INCLUDE ${BGFX_DEP_INCLUDE}")
  message(STATUS "BGFX_DEP_LIBRARIES ${BGFX_DEP_LIBRARIES}")

  set(targetLibName ${BGFX_DEP_TARGET})
  message(STATUS "import ${BGFX_DEP_TARGET} as static ${targetLibName} library")

  if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(${BGFX_DEP_TARGET}_LIBRARY "lib${BGFX_DEP_TARGET}Debug")
  else()
    set(${BGFX_DEP_TARGET}_LIBRARY "lib${BGFX_DEP_TARGET}Release")
  endif()
  message(STATUS "${BGFX_DEP_TARGET}_LIBRARY = ${${BGFX_DEP_TARGET}_LIBRARY}")

  add_library(${targetLibName} STATIC IMPORTED GLOBAL)

  if (UNIX)
    set_target_properties(${targetLibName} PROPERTIES IMPORTED_LOCATION
      ${bgfx_LIBRARY_IMPORT_DIR}/${${BGFX_DEP_TARGET}_LIBRARY}.a
    )
  else()
    message(ERROR "Your target operating system is not yet supported - fix it in libs/CMakeListst.txt bgfx_dep_target macro")
  endif()

  if (BGFX_DEP_INCLUDE)
    target_include_directories(${targetLibName} INTERFACE
      ${BGFX_DEP_INCLUDE}
    )
  endif()

  if (BGFX_DEP_LIBRARIES)
    target_link_libraries(${targetLibName} INTERFACE
      ${BGFX_DEP_LIBRARIES}
    )
  endif()

  get_property(dirs TARGET ${targetLibName} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
  foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
  endforeach()

  list(APPEND BGFX_LIBRARIES ${targetLibName} ${bgfx_LIBRARIES})
  set(bgfx_LIBRARIES ${BGFX_LIBRARIES} PARENT_SCOPE)

  set_target_properties(${targetLibName} PROPERTIES FOLDER ${BGFX_DEP_TARGET})
endfunction()

# ------------------------------------------------------------------------------

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(bgfx_BUILD_TYPE "debug")
else()
  set(bgfx_BUILD_TYPE "release")
endif()

if (UNIX AND NOT APPLE)
  set(bgfx_LIBRARY_BUILD_VERSION "linux-${bgfx_BUILD_TYPE}64")
elseif (UNIX AND APPLE)
  set(bgfx_LIBRARY_BUILD_VERSION "osx-${bgfx_BUILD_TYPE}64")
else()
  message(ERROR "Your target operating system is not yet supported - fix it in libs/CMakeListst.txt")
endif()

# ------------------------------------------------------------------------------

if (UNIX)
  if (NOT EXISTS "${bgfx_LIBRARY_IMPORT_DIR}/")
    message(STATUS "Build bgfx with ${bgfx_LIBRARY_BUILD_VERSION}...")
    execute_process(COMMAND make ${bgfx_LIBRARY_BUILD_VERSION}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bgfx
    )
  else()
    message(STATUS "Using already build bgfx version. If you need to recompile it, remove the .build directory in libs/bgfx.")
  endif()
endif()

# ------------------------------------------------------------------------------

LIST(INSERT bimg_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/bimg/3rdparty
  ${CMAKE_CURRENT_SOURCE_DIR}/bimg/3rdparty/iqa/include
  ${CMAKE_CURRENT_SOURCE_DIR}/bimg/include ${CMAKE_CURRENT_SOURCE_DIR}/bimg/src
)
bgfx_dep_target(TARGET bimg
  INCLUDE
  ${bimg_INCLUDES}
  LIBRARIES
  bx
  BGFX_LIBRARIES ${bgfx_LIBRARIES}
)

LIST(INSERT bx_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/bx/3rdparty
  ${CMAKE_CURRENT_SOURCE_DIR}/bx/include
  ${CMAKE_CURRENT_SOURCE_DIR}/bx/src
)
bgfx_dep_target(TARGET bx
  INCLUDE
  ${bx_INCLUDES}
  BGFX_LIBRARIES ${bgfx_LIBRARIES}
)

bgfx_dep_target(TARGET bimg_decode BGFX_LIBRARIES ${bgfx_LIBRARIES})
bgfx_dep_target(TARGET bimg_encode BGFX_LIBRARIES ${bgfx_LIBRARIES})
bgfx_dep_target(TARGET fcpp BGFX_LIBRARIES ${bgfx_LIBRARIES})

bgfx_dep_target(TARGET spirv-cross
  INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/spirv-cross/include
  BGFX_LIBRARIES ${bgfx_LIBRARIES}
)

bgfx_dep_target(TARGET spirv-opt BGFX_LIBRARIES ${bgfx_LIBRARIES})

bgfx_dep_target(TARGET glslang
  INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/glslang/glslang/Include
  BGFX_LIBRARIES ${bgfx_LIBRARIES}
)

bgfx_dep_target(TARGET glsl-optimizer
  INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/glsl-optimizer/include
  BGFX_LIBRARIES ${bgfx_LIBRARIES}
)

bgfx_dep_target(TARGET example-common BGFX_LIBRARIES ${bgfx_LIBRARIES})
bgfx_dep_target(TARGET example-glue BGFX_LIBRARIES ${bgfx_LIBRARIES})

# ------------------------------------------------------------------------------

message(STATUS "configure bgfx")

add_library(bgfx STATIC IMPORTED GLOBAL)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(bgfx_LIBRARY "libbgfxDebug")
else()
  set(bgfx_LIBRARY "libbgfxRelease")
endif()

if (UNIX)
  set_target_properties(bgfx PROPERTIES IMPORTED_LOCATION
    ${bgfx_LIBRARY_IMPORT_DIR}/${bgfx_LIBRARY}.a
  )
else()
  message(ERROR "Your target operating system is not yet supported - fix it in libs/CMakeListst.txt")
endif()

if (UNIX AND NOT APPLE)
  find_path(bgfx_X_INCLUDEDIR X11/Xlib.h
    /usr/pkg/xorg/include
    /usr/X11R6/include
    /usr/X11R7/include
    /usr/local/include/X11
    /usr/include/X11
    /usr/openwin/include
    /usr/openwin/share/include
    /opt/graphics/OpenGL/include
    /opt/X11/include
  )
endif()

target_include_directories(bgfx INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty
  ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/khronos
  ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/dxsdk/include
  ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/scintilla/include
  ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/spirv-headers/include
  ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/3rdparty/spirv-tools/include

  ${bgfx_X_INCLUDEDIR}

  ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/examples
  ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/include
  ${CMAKE_CURRENT_SOURCE_DIR}/bgfx/src
)

message(STATUS "bgfx_LIBRARIES: ${bgfx_LIBRARIES}")

target_link_libraries(bgfx INTERFACE
  ${bgfx_LIBRARIES}
  X11
  GL
)

get_property(linkedLibs TARGET bgfx PROPERTY INTERFACE_LINK_LIBARIES)

foreach(linkedlib ${linkedLibs})
  message(STATUS "linkedlib='${linkedlib}'")
endforeach()
