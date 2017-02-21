# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.2)

make_directory(${CMAKE_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE 
      Debug CACHE STRING
      "The build options are: None Debug Release RelWithDebInfo MinSizeRel." 
      FORCE)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux specific code
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS_DEBUG "-lsvml -lirc ${CMAKE_CXX_FLAGS_DEBUG}")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Apple-specific code
  set(CMAKE_CXX_FLAGS "-DOSX")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

set(img_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(src)

