#include "bsg.h"

bsg::scene scene = bsg::scene();

void init(int argc, char** argv) {

  // Initialize the graphics display, in all the ways required.
  // You'll often see this as "creating a graphics *context*".  The
  // funny thing about OpenGL is that its functions don't even exist
  // if there is no graphics context.  This means that glBufferData()
  // doesn't merely fail, it pretends not to exist.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

}

// This is the heart of any graphics program, the render function.  It
// is called each time through the main graphics loop, and re-draws
// the scene according to whatever has changed since the last time it
// was drawn.
void renderScene() {

  glm::mat4 viewMatrix, projMatrix;
  
  // First clear the display.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // First the load() step.  For a simple desktop display, it is a bit
  // mysterious to have separate load and draw steps, but it makes
  // sense when you have to render to a stereo display, where you only
  // need to load things once, but then draw it twice with slightly
  // different view matrices.
  scene.load();
  
  scene.draw();

  glutSwapBuffers();
}

// This function is called when the window changes size.  It
// adjusts the projection matrix, and resets an OpenGL value for
// the size of the viewport.
void resizeWindow(int width, int height) {

  // Prevent a divide by zero, when window is too short (you cant make
  // a window of zero width).
  if(height == 0) height = 1;
 
  // Set the viewport to be the entire window
  glViewport(0, 0, width, height);

  // We reset the aspect ratio and leave _fov, _nearClip, _farClip unchanged.
  scene.setAspect( (1.0f * width) / height );

  // The projection matrix will be recalculated on the next load()
  // during the renderScene function.
  //  _projMatrix = glm::perspective(_fov, _aspect, _nearClip, _farClip);
}

void processNormalKeys(unsigned char key, int x, int y) {

  float step = 0.5f;
  
  // This function processes only the 'normal' keys.  The arrow keys
  // don't appear here, nor mouse events.
  switch (key) {
  case 27:
    exit(0);

    // These next few are for steering the position of the viewer.
  case 'a':
    scene.addToCameraPosition(glm::vec3(-step, 0.0f, 0.0f));
    break;
  case 'q':
    scene.addToCameraPosition(glm::vec3( step, 0.0f, 0.0f));
    break;
  case 's':
    scene.addToCameraPosition(glm::vec3( 0.0f,-step, 0.0f));
    break;
  case 'w':
    scene.addToCameraPosition(glm::vec3( 0.0f, step, 0.0f));
    break;
  case 'd':
    scene.addToCameraPosition(glm::vec3( 0.0f, 0.0f, -step));
    break;
  case 'e':
    scene.addToCameraPosition(glm::vec3( 0.0f, 0.0f,  step));
    break;

    // These next are for steering the position of where you're looking.
  case 'j':
    scene.addToLookAtPosition(glm::vec3(-step, 0.0f, 0.0f));
    break;
  case 'u':
    scene.addToLookAtPosition(glm::vec3( step, 0.0f, 0.0f));
    break;
  case 'k':
    scene.addToLookAtPosition(glm::vec3( 0.0f,-step, 0.0f));
    break;
  case 'i':
    scene.addToLookAtPosition(glm::vec3( 0.0f, step, 0.0f));
    break;
  case 'l':
    scene.addToLookAtPosition(glm::vec3( 0.0f, 0.0f, -step));
    break;
  case 'o':
    scene.addToLookAtPosition(glm::vec3( 0.0f, 0.0f,  step));
    break;
  }

  // Uncomment these to see where you are (where the camera is) and where
  // you're looking.
  std::cout << "location:("
            << scene.getCameraPosition().x << ", "
            << scene.getCameraPosition().y << ", "
            << scene.getCameraPosition().z << ")" << std::endl; 
  std::cout << "_lookAtPosition.:("
            << scene.getLookAtPosition().x << ", "
            << scene.getLookAtPosition().y << ", "
            << scene.getLookAtPosition().z << ")" << std::endl; 
}
    
void processSpecialKeys(int key, int x, int y) {

  float stepAngle = 5.0f / 360.0f;

  switch(key) {    
  case GLUT_KEY_UP:
    scene.addToCameraViewAngle(0.0f,  stepAngle);
    break;
  case GLUT_KEY_DOWN:
    scene.addToCameraViewAngle(0.0f, -stepAngle);
    break;
  case GLUT_KEY_LEFT:
    scene.addToCameraViewAngle( stepAngle, 0.0f);
    break;
  case GLUT_KEY_RIGHT:
    scene.addToCameraViewAngle(-stepAngle, 0.0f);
    break;
  }

  // Uncomment these to see where you are (where the camera is) and where
  // you're looking.
  std::cout << "location:("
            << scene.getCameraPosition().x << ", "
            << scene.getCameraPosition().y << ", "
            << scene.getCameraPosition().z << ")" << std::endl; 
  std::cout << "_lookAtPosition.:("
            << scene.getLookAtPosition().x << ", "
            << scene.getLookAtPosition().y << ", "
            << scene.getLookAtPosition().z << ")" << std::endl; 
}

void makeWindow(const int xOffset, const int yOffset,
                const int xWidth, const int yWidth) {

  // Create the window, at this position and with this size, and heading.
  glutInitWindowPosition(xOffset, yOffset);
  glutInitWindowSize(xWidth, yWidth);
  glutCreateWindow("OpenGL Demo");

  // These next few functions tell glut what to do under certain
  // conditions.  This is where the render function (it's called
  // renderScene) is nominated, and where the keyboard handler
  // (processNormalKeys) is, too.
  glutDisplayFunc(renderScene);
  glutIdleFunc(renderScene);
  glutKeyboardFunc(processNormalKeys);
  glutSpecialFunc(processSpecialKeys);
  // This function is called when the user changes the size of the window.
  glutReshapeFunc(resizeWindow);

  // Now that we have a graphics context, let's look at what's inside.
  std::cout << "Hardware check: "
            << glGetString(GL_RENDERER)  // e.g. Intel 3000 OpenGL Engine
            << " / "
            << glGetString(GL_VERSION)    // e.g. 3.2 INTEL-8.0.61
            << std::endl;

  // There is one more graphics library used here, called GLEW.  This
  // library sorts through the various OpenGL updates and changes and
  // allows a user to pretend that it's all a consistent and simple
  // system.  The 'core profile' refers to some modern OpenGL
  // enhancements that are not so modern as of 2017.  Set this to true
  // to get those enhancements.
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("Failed to initialize GLEW");
  }

  if (glewIsSupported("GL_VERSION_2_1")) {
    std::cout << "Software check: Ready for OpenGL 2.1." << std::endl;
  } else {
    throw std::runtime_error("Software check: OpenGL 2.1 not supported.");
  }

  // Now we're ready to start issuing OpenGL calls.  Start by enabling
  // the modes we want.  The DEPTH_TEST and CULL_FACE are how you get
  // hidden faces.
  glEnable(GL_DEPTH_TEST | GL_PROGRAM_POINT_SIZE | GL_CULL_FACE);

  // This is the background color of the viewport.
  glClearColor(0.1 , 0.0, 0.4, 1.0);
}


int main(int argc, char **argv) {

  init(argc, argv);
  makeWindow(100, 100, 400, 400);
  
  bsg::lightList* lights = new bsg::lightList();
  lights->addLight(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(1.0f, 1.0f, 0.0f));
  lights->addLight(glm::vec3(10.0f,-10.0f, 10.0f), glm::vec3(0.0f, 1.0f, 1.0f));

  if (argc < 3) {
    throw std::runtime_error("\nNeed two args: the names of a vertex and fragment shader.\nTry 'bin/demo2 ../src/shader2.vp ../src/shader.fp\n'");
  }

  bsg::shaderMgr* shader = new bsg::shaderMgr();
  shader->addLights(lights);
  
  std::string vertexFile = std::string(argv[1]);
  shader->addShader(bsg::GLSHADER_VERTEX, vertexFile);
  std::string fragmentFile = std::string(argv[2]);
  shader->addShader(bsg::GLSHADER_FRAGMENT, fragmentFile);

  shader->compileShaders();

  bsg::drawableObj shape = bsg::drawableObj(shader);
  std::vector<glm::vec4> shapeVertices;
  shapeVertices.push_back(glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f));
  shapeVertices.push_back(glm::vec4( 5.0f, 0.0f, 0.0f, 1.0f));
  shapeVertices.push_back(glm::vec4( 0.0f, 5.0f, 0.0f, 1.0f));

  shapeVertices.push_back(glm::vec4( 5.0f, 0.0f, 0.0f, 1.0f));
  shapeVertices.push_back(glm::vec4( 0.0f, 0.0f, 5.0f, 1.0f));
  shapeVertices.push_back(glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f));

  shapeVertices.push_back(glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f));
  shapeVertices.push_back(glm::vec4( 0.0f, 5.0f, 0.0f, 1.0f));
  shapeVertices.push_back(glm::vec4( 0.0f, 0.0f, 5.0f, 1.0f));

  shapeVertices.push_back(glm::vec4( 0.0f, 5.0f, 0.0f, 1.0f));
  shapeVertices.push_back(glm::vec4( 0.0f, 0.0f, 5.0f, 1.0f));
  shapeVertices.push_back(glm::vec4( 5.0f, 0.0f, 0.0f, 1.0f));

  shape.addData(bsg::GLDATA_VERTICES, "position", shapeVertices);

  std::vector<glm::vec4> shapeColors;
  shapeColors.push_back(glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f));
  shapeColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));
  shapeColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));

  shapeColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
  shapeColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));
  shapeColors.push_back(glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f));

  shapeColors.push_back(glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f));
  shapeColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
  shapeColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));

  shapeColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
  shapeColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));
  shapeColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));

  shape.addData(bsg::GLDATA_COLORS, "color", shapeColors);

  // One triangle is described by these vertices.
  shape.setDrawType(GL_TRIANGLES);  

  bsg::drawableCompound* compoundShape =
    new bsg::drawableCompound(shader);
  compoundShape->addObject(shape);

  scene.addCompound(compoundShape);

  scene.setLookAtPosition(glm::vec3(2.5f, 2.5f, 0.0f));
  scene.setCameraPosition(glm::vec3(0.0f, 2.0f, 7.5f));
  
  // All the shapes are added to the scene.

  // Do the one-time things.  The every-render operations are done
  // inside the renderScene function defined above, but executed
  // within the mainloop, below.
  scene.prepare();

  // This loop never exits.
  glutMainLoop();

  // We never get here, but the compiler is annoyed when you don't
  // exit from a function.
  return(0); 
}
