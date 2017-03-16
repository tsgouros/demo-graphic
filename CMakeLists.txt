# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.2)

project (demo-graphic)

# The project version number.
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   1   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   1   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

make_directory(${CMAKE_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)")

if(BUILD_DOCUMENTATION)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation.")
  endif()

  message("-- Configured to build documentation.  Use 'make doc'.")
  
  # Process the config file and put it in the build directory.
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/demo-graphic.doxygen.in)
  set(doxyfile ${CMAKE_BINARY_DIR}/demo-graphic.doxygen)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)

  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux specific code
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS_DEBUG "-lsvml -lirc ${CMAKE_CXX_FLAGS_DEBUG}")

#  set(CMAKE_C_COMPILER gcc)
#  set(CMAKE_CXX_COMPILER g++)

endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Apple-specific code
  set(CMAKE_CXX_FLAGS "-DOSX")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

set(img_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(src)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set (CMAKE_INSTALL_PREFIX "${${PROJECT_NAME}_SOURCE_DIR}/build/install"
    CACHE PATH "default install path" FORCE )
endif()


