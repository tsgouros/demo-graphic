# Use this file for compiling the examples.

add_executable (demo demo.cpp)

target_link_libraries(demo PUBLIC
  ${FREEGLUT_LIBRARY}
  ${OPENGL_LIBRARY}
  ${GLEW_LIBRARY}
)

find_package(Freetype REQUIRED)

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${OPENGL_INCLUDE_DIR}
  ${FREEGLUT_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS})

#if(PNG_FOUND)

  # This is a goofy fix for a strange libpng linking problem on OSCAR.
  #if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  #  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${PNG_LIBRARY_PATH}")
  #endif()

  add_executable(demo2 demo2.cpp)

  target_link_libraries(demo2 PUBLIC bsg freetypegl
   ${FREEGLUT_LIBRARY}
   ${OPENGL_LIBRARY}
   ${GLEW_LIBRARY}
   ${FREETYPE_LIBRARIES})

  add_executable(textureDemo textureDemo.cpp)
  add_dependencies(textureDemo freetypegl-download)

  target_link_libraries(textureDemo PUBLIC bsg freetypegl
   ${FREEGLUT_LIBRARY}
   ${OPENGL_LIBRARY}
   ${GLEW_LIBRARY}
   ${FREETYPE_LIBRARIES})

  add_executable(textDemo textDemo.cpp)

  target_link_libraries(textDemo PUBLIC bsg freetypegl
   ${FREEGLUT_LIBRARY}
   ${OPENGL_LIBRARY}
   ${GLEW_LIBRARY}
   ${FREETYPE_LIBRARIES})

  add_dependencies(textDemo freetypegl-download)

  add_executable(treeDemo treeDemo.cpp)

  target_link_libraries(treeDemo PUBLIC bsg freetypegl
    ${FREEGLUT_LIBRARY}
    ${OPENGL_LIBRARY}
    ${GLEW_LIBRARY}
    ${FREETYPE_LIBRARIES})

  add_executable(treeDemo2 treeDemo2.cpp)

  target_link_libraries(treeDemo2 PUBLIC bsg freetypegl
    ${FREEGLUT_LIBRARY}
    ${OPENGL_LIBRARY}
    ${GLEW_LIBRARY}
    ${FREETYPE_LIBRARIES})

  add_executable(selectDemo selectDemo.cpp)

  target_link_libraries(selectDemo PUBLIC bsg freetypegl
    ${FREEGLUT_LIBRARY}
    ${OPENGL_LIBRARY}
    ${GLEW_LIBRARY}
    ${FREETYPE_LIBRARIES})


  if(MinVR_FOUND)

    # Redefine the include directories to include MinVR.
    include_directories(
      ${OPENGL_INCLUDE_DIR}
      ${FREEGLUT_INCLUDE_DIR}
      ${GLM_INCLUDE_DIR}
      ${GLEW_INCLUDE_DIRS}
      ${MINVR_INCLUDE_DIR})

    add_executable(demo3 demo3.cpp)

    target_link_libraries(demo3 PUBLIC bsg freetypegl MinVR::MinVR
      ${FREEGLUT_LIBRARY}
      ${OPENGL_LIBRARY}
      ${GLEW_LIBRARY}
      ${FREETYPE_LIBRARIES})

    add_executable(pointDemo pointDemo.cpp)

    target_link_libraries(pointDemo PUBLIC bsg freetypegl MinVR::MinVR
      ${FREEGLUT_LIBRARY}
      ${OPENGL_LIBRARY}
      ${GLEW_LIBRARY}
      ${FREETYPE_LIBRARIES})

    add_executable(demo4 demo4.cpp)

    target_link_libraries(demo4 PUBLIC freetypegl bsg MinVR::MinVR
      ${FREEGLUT_LIBRARY}
      ${OPENGL_LIBRARY}
      ${GLEW_LIBRARY}
      ${FREETYPE_LIBRARIES})

    add_executable(textureDemoMinVR textureDemoMinVR.cpp)


    target_link_libraries(textureDemoMinVR PUBLIC bsg freetypegl MinVR::MinVR
      ${FREEGLUT_LIBRARY}
      ${OPENGL_LIBRARY}
      ${GLEW_LIBRARY}
      ${FREETYPE_LIBRARIES})

    add_executable(textDemoMinVR textDemoMinVR.cpp)
    add_dependencies(textDemoMinVR freetypegl-download)

    target_link_libraries(textDemoMinVR bsg freetypegl MinVR::MinVR
     ${MINVR_LIBRARY}
     ${FREEGLUT_LIBRARY}
     ${OPENGL_LIBRARY}
     ${GLEW_LIBRARY}
     ${GLEW_LIBRARY}
     ${FREETYPE_LIBRARIES})

    add_executable(objDemoMinVR objDemoMinVR.cpp)

    target_link_libraries(objDemoMinVR PUBLIC freetypegl bsg MinVR::MinVR
      ${FREEGLUT_LIBRARY}
      ${OPENGL_LIBRARY}
      ${GLEW_LIBRARY}
      ${FREETYPE_LIBRARIES})

    add_executable(kbDemoMinVR kbDemoMinVR.cpp)

    target_link_libraries(kbDemoMinVR PUBLIC bsg freetypegl MinVR::MinVR
      ${FREEGLUT_LIBRARY}
      ${OPENGL_LIBRARY}
      ${GLEW_LIBRARY}
      ${FREETYPE_LIBRARIES})

  else(MinVR_FOUND)

    message("** No MinVR for you!")

  endif(MinVR_FOUND)
#endif(PNG_FOUND)



