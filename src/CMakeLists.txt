project (demo-graphic)

# This is the path where cmake will look for files like "FindPNG.cmake".
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(FreeGLUT REQUIRED)

set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/glm-0.9.7.1)

message("-- GLM includes:    " ${GLM_INCLUDE_DIR})
message("-- OpenGL includes: " ${OPENGL_INCLUDE_DIR})
message("-- OpenGL library:  " ${OPENGL_LIBRARY})
message("-- GLEW library:    " ${GLEW_LIBRARY})
message("-- GLEW includes:   " ${GLEW_INCLUDE_DIRS})

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

