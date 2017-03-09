project (demo-graphic)

# This is the path where cmake will look for files like "FindPNG.cmake".
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(FreeGLUT REQUIRED)
message("-- FreeGLUT includes:" ${FREEGLUT_INCLUDE_DIR})
message("-- FreeGLUT library: " ${FREEGLUT_LIBRARY})

set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/glm-0.9.7.1)

message("-- GLM includes:     " ${GLM_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
message("-- OpenGL includes:  " ${OPENGL_INCLUDE_DIR})
message("-- OpenGL library:   " ${OPENGL_LIBRARY})

find_package(GLEW REQUIRED)
message("-- GLEW library:     " ${GLEW_LIBRARY})
message("-- GLEW includes:    " ${GLEW_INCLUDE_DIRS})

find_package(PNG 1.4 MODULE)
message("-- PNG includes:     " ${PNG_INCLUDE_DIRS})
message("-- PNG library:      " ${PNG_LIBRARIES})

find_package(MinVR MODULE)
message("-- MinVR includes:   " ${MINVR_INCLUDE_DIR})
message("-- MinVR library:    " ${MINVR_LIBRARY})

include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${FREEGLUT_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIRS}
  )

set(bsg_headers bsg.h bsgMenagerie.h)
set(bsg_sources bsg.cpp bsgMenagerie.cpp)
set(bsg_files ${bsg_headers} ${bsg_sources})

add_library(bsg ${bsg_files})

# Source files go here.
add_executable (demo demo.cpp)

target_link_libraries(demo
  ${FREEGLUT_LIBRARY}
  ${OPENGL_LIBRARY}
  ${GLEW_LIBRARY}
)

if(PNG_FOUND)
  add_executable(demo2 demo2.cpp ${bsg_files})

  target_link_libraries(demo2
    ${FREEGLUT_LIBRARY}
    ${OPENGL_LIBRARY}
    ${GLEW_LIBRARY}
    ${PNG_LIBRARIES})

  if(MINVR_FOUND)

    # Redefine the include directories to include MinVR.
    include_directories(
      ${OPENGL_INCLUDE_DIR}
      ${FREEGLUT_INCLUDE_DIR}
      ${GLM_INCLUDE_DIR}
      ${GLEW_INCLUDE_DIRS}
      ${MINVR_INCLUDE_DIR}
      )

    add_executable(demo3 demo3.cpp ${bsg_files})
  
    target_link_libraries(demo3
      ${MINVR_LIBRARY}
      ${FREEGLUT_LIBRARY}
      ${OPENGL_LIBRARY}
      ${GLEW_LIBRARY}
      ${PNG_LIBRARIES})

    add_executable(demo4 demo4.cpp ${bsg_files})

    target_link_libraries(demo4
      ${MINVR_LIBRARY}
      ${FREEGLUT_LIBRARY}
      ${OPENGL_LIBRARY}
      ${GLEW_LIBRARY}
      ${PNG_LIBRARIES})

  else(MINVR_FOUND)

    message("** No MinVR for you!")
    message("** See instructions and advice on the topic above.")
    
  endif(MINVR_FOUND)
endif(PNG_FOUND)

install(TARGETS bsg
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${bsg_headers}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

