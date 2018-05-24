# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 3.9)

project (demo-graphic)

# This is for the new-style cmake includes.
if (MINVR_INSTALL_DIR) 
  list(APPEND CMAKE_PREFIX_PATH ${MINVR_INSTALL_DIR})
else(MINVR_INSTALL_DIR)
  message(WARNING "MINVR_INSTALL_DIR not specified, so I will probably not be able to find MinVR.  But I'll look for it and install everything else for you.")
endif(MINVR_INSTALL_DIR)

# The project version number.
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   1   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   1   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

make_directory(${CMAKE_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# This is the path where cmake will look for files like "FindGLEW.cmake".
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# If you don't set one of these build types on the command line, with
# 'cmake -DCMAKE_BUILD_TYPE=Debug' or something, the default is 'Debug'.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
    Debug CACHE STRING
    "The build options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
endif()

# add a target to generate API documentation with Doxygen
find_package(Doxygen)

option(BUILD_DOCUMENTATION "If enabled, will create and install the HTML based API documentation (requires Doxygen)")

if(BUILD_DOCUMENTATION)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation.")
  endif()

  message("-- Configured to build documentation.  Use 'make doc'.")

  # Process the config file and put it in the build directory.
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/demo-graphic.doxygen.in)
  set(doxyfile ${CMAKE_BINARY_DIR}/demo-graphic.doxygen)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)

  add_custom_target(doc ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)

else()
  message("-- NOT building BSG library documentation.")
  message("   Use 'cmake -DBUILD_DOCUMENTATION=on' to build.")

endif()

find_package(OpenGL REQUIRED)
message("-- OpenGL includes:  " ${OPENGL_INCLUDE_DIR})
message("-- OpenGL library:   " ${OPENGL_LIBRARY})

find_package(GLEW REQUIRED)
message("-- GLEW library:     " ${GLEW_LIBRARY})
message("-- GLEW includes:    " ${GLEW_INCLUDE_DIRS})

set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/glm-0.9.7.1)
message("-- GLM includes:     " ${GLM_INCLUDE_DIR})

find_package(FreeGLUT REQUIRED)
message("-- FreeGLUT includes:" ${FREEGLUT_INCLUDE_DIR})
message("-- FreeGLUT library: " ${FREEGLUT_LIBRARY})

find_package(MinVR)
# message("-- MinVR includes:   " ${MINVR_INCLUDE_DIR})
# message("-- MinVR library:    " ${MINVR_LIBRARY})
# message("-- 2d MinVR library: " ${MINVR_LIBRARY_DEBUG})

if (MinVR_FOUND)
  message(STATUS "MinVR found at: ${MinVR_DIR}")
else (MinVR_FOUND)
  message(STATUS "MinVR not found, my apologies.  Try specifying MINVR_INSTALL_DIR like this:\n   $ cmake .. -DMINVR_INSTALL_DIR=/path/to/your/MinVR/install")
endif(MinVR_FOUND)

# TODO @martha & tom: this is a hack, for now, because there is some problem with the linking
# set(MINVR_LIBRARY /gpfs/runtime/opt/minvr/beta-bleeding/lib/MinVR-2.0/libMinVR.a)
# set(MINVR_INCLUDE_DIR ~/Dropbox/ccv/MinVR/build/install/include)

# set(MINVR_LIBRARY_DEBUG ${MINVR_LIBRARY})

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux specific code -- Not sure why pthreads or dl is required.  This
  # is not necessary on all systems.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl -fPIC -lpthread")

#  set(CMAKE_C_COMPILER gcc)
#  set(CMAKE_CXX_COMPILER g++)

endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Apple-specific code
  set(CMAKE_CXX_FLAGS "-DOSX")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set (CMAKE_INSTALL_PREFIX "${${PROJECT_NAME}_SOURCE_DIR}/build/install"
    CACHE PATH "default install path" FORCE )
endif()

set(img_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_definitions(-DDATAPATH="${CMAKE_SOURCE_DIR}")

## Need to oversee the installation and availability of freetype-gl.  It builds as
## a static library, and the include files are not installed, but are simply
## picked up from the source directory.  Code that depends on it can use
## add_dependencies(whatever freetypegl) and make sure that FREETYPEGL_INCLUDE_DIR
## is nominated in the include_directories() list.
message(STATUS "LOOKING FOR GLFW3")
find_package(glfw3)

set(FREETYPEGL_CHECKOUT_DIR ${PROJECT_SOURCE_DIR}/external/freetype-gl)
make_directory(${FREETYPEGL_CHECKOUT_DIR})
include(ExternalProject)

set(FREETYPEGL_BUILD_DIR ${CMAKE_BINARY_DIR}/freetypegl)
set(FREETYPEGL_STAMP_DIR ${FREETYPEGL_BUILD_DIR}/stamp)
set(FREETYPEGL_TMP_DIR ${FREETYPEGL_BUILD_DIR}/tmp)

set(FREETYPEGL_PREFIX "${FREETYPEGL_CHECKOUT_DIR}")
set(FREETYPEGL_INSTALL_DIR "${FREETYPEGL_BUILD_DIR}/install")

set(FREETYPEGL_CMAKE_ARGS  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -Dfreetype-gl_BUILD_DEMOS=OFF -Dfreetype-gl_BUILD_APIDOC=OFF -Dfreetype-gl_BUILD_MAKEFONT=OFF -Dfreetype-gl_BUILD_TESTS=OFF)

ExternalProject_add(freetypegl-download
    PREFIX ${FREETYPEGL_PREFIX}
    GIT_REPOSITORY https://github.com/rougier/freetype-gl.git
    GIT_TAG master
    INSTALL_DIR ${FREETYPEGL_INSTALL_DIR}
    CMAKE_ARGS ${FREETYPEGL_CMAKE_ARGS}
    UPDATE_COMMAND ""
    DOWNLOAD_DIR ${FREETYPEGL_CHECKOUT_DIR}
    SOURCE_DIR ${FREETYPEGL_CHECKOUT_DIR}
    TMP_DIR ${FREETYPEGL_TMP_DIR}
    STAMP_DIR ${FREETYPEGL_STAMP_DIR}
    BINARY_DIR ${FREETYPEGL_BUILD_DIR}
)

message(STATUS "Building freetypegl from: ${FREETYPEGL_PREFIX}.")

set(FREETYPEGL_INCLUDE_DIR ${FREETYPEGL_PREFIX} CACHE INTERNAL "Where the FreetypeGL include files are supposed to be.")
set(FREETYPEGL_LIBRARY_DIR ${FREETYPEGL_BUILD_DIR} CACHE INTERNAL "Where the FreetypeGL library is supposed to be.")

add_library(freetypegl STATIC IMPORTED GLOBAL)
set_target_properties(freetypegl PROPERTIES IMPORTED_LOCATION ${FREETYPEGL_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}freetype-gl${CMAKE_STATIC_LIBRARY_SUFFIX})
get_target_property(TMP_VAR freetypegl IMPORTED_LOCATION)

message(STATUS "Freetype-GL library:  ${TMP_VAR}")
message(STATUS "Freetype-GL includes: ${FREETYPEGL_INCLUDE_DIR}")

add_subdirectory(src)
add_subdirectory(examples)

## ************************************************ HOW ARE WE BUILDING?


# The following sorts through all the cmake variables, and stores the ones
# whose help strings include the magic string "If enabled", plus the cmake
# build type (Debug, Release, etc.) and install directory.  It then stores a
# simulacrum of the cmake invocation in a shell script, so you can run it
# again if you like.
#
# This also means that if you add options, make their help strings include
# the magic string.

# Get a list of all the cmake variables.
get_cmake_property(CACHE_VARS CACHE_VARIABLES)

# Start our invocation string.
set(CMAKE_INVOCATION "cmake ${CMAKE_SOURCE_DIR} ")
cmake_policy(SET CMP0054 NEW)

# Loop through all the variables, and if they match the magic string, or if
# they are the build type, add them to the invocation string.
foreach(CACHE_VAR ${CACHE_VARS})
  get_property(CACHE_VAR_HELPSTRING CACHE ${CACHE_VAR} PROPERTY HELPSTRING)
  get_property(CACHE_VAR_TYPE CACHE ${CACHE_VAR} PROPERTY TYPE)
  if (CACHE_VAR_HELPSTRING MATCHES "If enabled" OR
      # There are also a few commonly-used cmake variables worth recording.
      CACHE_VAR STREQUAL "CMAKE_BUILD_TYPE" OR
      CACHE_VAR STREQUAL "CMAKE_INSTALL_PREFIX" OR
      CACHE_VAR MATCHES "MINVR|MinVR" OR
      CACHE_VAR_TYPE STREQUAL "UNINITIALIZED")
    if(CACHE_VAR_TYPE STREQUAL "UNINITIALIZED")
      set(CACHE_VAR_TYPE)
    else()
      set(CACHE_VAR_TYPE :${CACHE_VAR_TYPE})
    endif()
    set(CMAKE_INVOCATION "${CMAKE_INVOCATION} -D${CACHE_VAR}${CACHE_VAR_TYPE}=\"${${CACHE_VAR}}\"")
  endif()
endforeach()
file(WRITE ${CMAKE_BINARY_DIR}/cmake-repeat.sh "#!/usr/bin/env bash\n# Feel free to edit this file before executing it. The file will be\n# overwritten with each cmake invocation, but the saved version will\n# reflect your changes.\n${CMAKE_INVOCATION}\n")

message(STATUS "CMAKE invocation stored in ${CMAKE_BINARY_DIR}/cmake-repeat.sh.\n   Feel free to edit, then execute that file to re-run this cmake.")
