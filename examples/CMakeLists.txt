# Use this file for compiling the examples.

add_executable (demo demo.cpp)

target_link_libraries(demo
  ${FREEGLUT_LIBRARY}
  ${OPENGL_LIBRARY}
  ${GLEW_LIBRARY}
)

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  )

if(PNG_FOUND)

  # This is a goofy fix for a strange libpng linking problem on OSCAR.
  if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${PNG_LIBRARY_PATH}")
  endif()

  add_executable(demo2 demo2.cpp)

  target_link_libraries(demo2 bsg
   ${FREEGLUT_LIBRARY}
   ${OPENGL_LIBRARY}
   ${GLEW_LIBRARY}
   ${PNG_LIBRARIES})

  add_executable(textureDemo textureDemo.cpp)

  target_link_libraries(textureDemo bsg
   ${FREEGLUT_LIBRARY}
   ${OPENGL_LIBRARY}
   ${GLEW_LIBRARY}
   ${PNG_LIBRARIES})
  
  add_executable(treeDemo treeDemo.cpp)

  target_link_libraries(treeDemo bsg
    ${FREEGLUT_LIBRARY}
    ${OPENGL_LIBRARY}
    ${GLEW_LIBRARY}
    ${PNG_LIBRARIES})
  
  add_executable(treeDemo2 treeDemo2.cpp)

  target_link_libraries(treeDemo2 bsg
    ${FREEGLUT_LIBRARY}
    ${OPENGL_LIBRARY}
    ${GLEW_LIBRARY}
    ${PNG_LIBRARIES})

  add_executable(selectDemo selectDemo.cpp)

  target_link_libraries(selectDemo bsg
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
      ${PNG_INCLUDE_DIRS}
      ${MINVR_INCLUDE_DIR}
      )

    add_executable(demo3 demo3.cpp)
  
    target_link_libraries(demo3 bsg
      ${MINVR_LIBRARY}
      ${FREEGLUT_LIBRARY}
      ${OPENGL_LIBRARY}
      ${GLEW_LIBRARY}
      ${PNG_LIBRARIES})

    add_executable(demo4 demo4.cpp)

    target_link_libraries(demo4 bsg
     ${MINVR_LIBRARY}
     ${FREEGLUT_LIBRARY}
     ${OPENGL_LIBRARY}
     ${GLEW_LIBRARY}
     ${PNG_LIBRARIES})
    
    add_executable(textureDemoMinVR textureDemoMinVR.cpp)


    target_link_libraries(textureDemoMinVR bsg
     ${MINVR_LIBRARY}
     ${FREEGLUT_LIBRARY}
     ${OPENGL_LIBRARY}
     ${GLEW_LIBRARY}
     ${PNG_LIBRARIES})

    add_executable(objDemoMinVR objDemoMinVR.cpp)

    target_link_libraries(objDemoMinVR bsg
      ${MINVR_LIBRARY}
      ${PNG_LIBRARIES}
      ${FREEGLUT_LIBRARY}
      ${OPENGL_LIBRARY}
      ${GLEW_LIBRARY}
      )

    add_executable(kbDemoMinVR kbDemoMinVR.cpp)

    target_link_libraries(kbDemoMinVR bsg
      ${MINVR_LIBRARY}
      ${PNG_LIBRARIES}
      ${FREEGLUT_LIBRARY}
      ${OPENGL_LIBRARY}
      ${GLEW_LIBRARY}
      )

  else(MINVR_FOUND)

    message("** No MinVR for you!")
    message("** See instructions and advice on the topic above.")

  endif(MINVR_FOUND)
endif(PNG_FOUND)


                    
