project (demo-graphic)

# This is the path where cmake will look for files like "FindPNG.cmake".
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PNG 1.4 MODULE)
find_package(FreeGLUT REQUIRED)

set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/glm-0.9.7.1)

message("-- GLM includes:    " ${GLM_INCLUDE_DIR})
message("-- OpenGL includes: " ${OPENGL_INCLUDE_DIR})
message("-- OpenGL library:  " ${OPENGL_LIBRARY})
message("-- GLEW library:    " ${GLEW_LIBRARY})
message("-- GLEW includes:   " ${GLEW_INCLUDE_DIRS})
message("-- PNG includes:    " ${PNG_INCLUDE_DIRS})
message("-- PNG library:     " ${PNG_LIBRARIES})

# ``PNG_LIBRARIES``
#   the libraries to link against to use PNG.
# ``PNG_DEFINITIONS``
#   You should add_definitons(${PNG_DEFINITIONS}) before compiling code
#   that includes png library files.
# ``PNG_FOUND``
#   If false, do not try to use PNG.


include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${FREEGLUT_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIRS}
  )

# Source files go here.
add_executable (demo demo.cpp)


target_link_libraries(demo
  ${FREEGLUT_LIBRARY}
  ${OPENGL_LIBRARY}
  ${GLEW_LIBRARY}
)

if(PNG_FOUND)
add_executable(demo2 demo2.cpp bsg.h bsg.cpp)

target_link_libraries(demo2
  ${FREEGLUT_LIBRARY}
  ${OPENGL_LIBRARY}
  ${GLEW_LIBRARY}
  ${PNG_LIBRARIES}
  )
endif()
