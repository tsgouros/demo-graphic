project(bsg)

include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${FREEGLUT_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIRS}
  ${PNG_INCLUDE_DIRS}
  )

set(bsg_headers bsg.h bsgMenagerie.h bsgObjModel.h)
set(bsg_sources bsg.cpp bsgMenagerie.cpp bsgObjModel.cpp)
set(bsg_files ${bsg_headers} ${bsg_sources})

add_library(bsg ${bsg_files})


install(TARGETS bsg
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${bsg_headers}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

set(demoheaders MVRDemo.h)

# add_library(MVRDemo ${demoheaders})
# set_target_properties(MVRDemo PROPERTIES LINKER_LANGUAGE CXX)

# install(TARGETS MVRDemo
#   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
# 	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${demoheaders}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

