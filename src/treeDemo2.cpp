#include "bsg.h"
#include "bsgMenagerie.h"

// The scene and the objects in it must be available from the main()
// function where it is created and the renderScene() function where
// it is drawn.  The scene object contains all the drawable objects
// that make up the scene.
bsg::scene scene = bsg::scene();

// These are the shapes that make up the scene.  They are out here in
// the global variables so they can be available in both the main()
// function and the renderScene() function.
bsg::drawableRectangle* bigRectangle;
bsg::drawableRectangle* smallRectangle;
bsg::drawableCollection* rectGroup1;

bsg::drawableRectangle* anotherBig;
bsg::drawableRectangle* anotherSmall;
bsg::drawableCollection* rectGroup2;

bsg::drawableRectangle* mediumRectangle;
bsg::drawableCollection* rectGroup3;
bsg::drawableAxes* axes;

// These are part of the animation stuff, and again are out here with
// the big boy global variables so they can be available to both the
// interrupt handler and the render function.
float oscillator = 0.0f;
float oscillationStep = 0.03f;

// Initialize the graphics display, in all the ways required.  You'll
// often see this as "creating a graphics *context*".  The funny thing
// about OpenGL is that its functions don't even exist if there is no
// graphics context.  This means that glBufferData() doesn't merely
// fail, it pretends not to exist.
void init(int argc, char** argv) {

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

  std::cout << "Initialize GLUT display mode." << std::endl;
}

// This is the heart of any graphics program, the render function.  It
// is called each time through the main graphics loop, and re-draws
// the scene according to whatever has changed since the last time it
// was drawn.
void renderScene() {

  // If you want to adjust the positions of the various objects in
  // your scene, this is where to do that.  You could also animate the
  // camera or lookat position here, or anything else you want to mess
  // with in the scene.
  glm::vec3 pos = rectGroup1->getPosition();
  oscillator += oscillationStep;
  pos.x = sin(oscillator);
  pos.y = 1.0f - cos(oscillator);
  rectGroup1->setPosition(pos);
  rectGroup1->getObject("small")->setPosition(cos(oscillator), 0.0, 1.0f);

  // Choosing an object or group via the name hierarchy.
  std::list<std::string> desiredNames;
  desiredNames.push_front("small2");
  desiredNames.push_front("group3");
  desiredNames.push_front("group2");

  bsg::bsgPtr<bsg::drawableMulti> p = scene.getObject(desiredNames);
  if (p) p->setPosition(2.0 * cos(oscillator), 0.0, 3.0 * sin(oscillator));

  // Using a different name, living dangerously without the if.
  desiredNames.clear();
  desiredNames.push_front("group3");
  desiredNames.push_front("group2");

  scene.getObject(desiredNames)->setRotation(0.0, 0.4, oscillator);

  bsg::ObjNameList s = scene.insideBoundingBox(glm::vec3(1.0f, 1.0f, 1.1f));
  
  // std::cout << "checking:";
  // for (bsg::ObjNameList::iterator it = s.begin(); it != s.end(); it++) {
  //   std::cout << *it << ",";
  // }
  // std::cout << std::endl;

  // scene.getObject(s)->setRotation(0.0, oscillator, 0.0);
  
  // Selecting an image with a world-space location.

  
  // Now the preliminaries are done, on to the actual drawing.
  
  // First clear the display.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // Second the load() step.  For a simple desktop display, it is a
  // bit mysterious to have separate load and draw steps, but it makes
  // sense when you have to render to a stereo display, where you only
  // need to load things once, but then draw it twice with slightly
  // different view matrices.  The load step generates the projection
  // matrix, and then loads all the subsidiary compound objects.
  scene.load();

  // The draw step loads a view matrix using the current camera
  // position, and then calls the draw() method for each of the
  // compound objects that make up the scene.
  scene.draw(scene.getViewMatrix(), scene.getProjMatrix());

  // Swap the graphics buffers.
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
}

// Just a little debug function so that a user can see what's going on
// in a non-graphical sense.
void showCameraPosition() {

  std::cout << "Camera is at ("
            << scene.getCameraPosition().x << ", "
            << scene.getCameraPosition().y << ", "
            << scene.getCameraPosition().z << ")... ";
  std::cout << "looking at ("
            << scene.getLookAtPosition().x << ", "
            << scene.getLookAtPosition().y << ", "
            << scene.getLookAtPosition().z << ")." << std::endl; 
}

// This is an event handler, designed to handle events issued by the
// user pressing a key on the keyboard.  (GLUT's version of "normal"
// keys, which is most of them, except the arrow and control keys.)
void processNormalKeys(unsigned char key, int x, int y) {

  // Each press of a key changes a dimension by this amount. If you
  // want things to go faster, increase this step.
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
  default:
    if (oscillationStep == 0.0f) {
      oscillationStep = 0.03f;
    } else {
      oscillationStep = 0.0f;
    }
  }

  // Print out where you are (where the camera is) and where you're
  // looking.
  showCameraPosition();
}

// This is also an event handler, but for events caused by pressing
// the "special" keys.  These are the arrow keys, and some others.
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

  // Print out where you are (where the camera is) and where you're
  // looking.
  showCameraPosition();
}

// This function contains the basics of getting a window set up and
// ready for drawing.
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

  // This is the background color of the viewport.
  glClearColor(0.1 , 0.0, 0.4, 1.0);

  // Now we're ready to start issuing OpenGL calls.  Start by enabling
  // the modes we want.  The DEPTH_TEST is how you get hidden faces.
  glEnable(GL_DEPTH_TEST);

  if (glIsEnabled(GL_DEPTH_TEST)) {
    std::cout << "Depth test enabled" << std::endl;
  } else {
    std::cout << "No depth test enabled" << std::endl;
  }

  // This is just a performance enhancement that allows OpenGL to
  // ignore faces that are facing away from the camera.
  glEnable(GL_CULL_FACE);

}


int main(int argc, char **argv) {

  // Initialize the graphics context and...
  init(argc, argv);

  // ... make a window for drawing things.
  makeWindow(100, 100, 400, 400);

  // Create a list of lights.  If the shader you're using doesn't use
  // lighting, and the shapes don't have textures, this is irrelevant.
  bsg::bsgPtr<bsg::lightList> lights = new bsg::lightList();
  lights->addLight(glm::vec4(0.0f, 0.0f, 3.0f, 1.0f),
                   glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

  // Now we load the shaders.  First check to see if any have been
  // specified on the command line.
  if (argc > 1) {
    throw std::runtime_error("\nShader file names are hard-coded in this one.\n'");
  }

  // Create a shader manager and load the light list.
  bsg::bsgPtr<bsg::shaderMgr> shader = new bsg::shaderMgr();
  shader->addLights(lights);

  // Add the shaders to the manager, first the vertex shader...
  std::string vertexFile = std::string("../src/textureShader.vp");
  shader->addShader(bsg::GLSHADER_VERTEX, vertexFile);

  // ... then the fragment shader.  You could potentially add a
  // geometry shader at this point.
  std::string fragmentFile = std::string("../src/textureShader.fp");
  shader->addShader(bsg::GLSHADER_FRAGMENT, fragmentFile);

  // The shaders are loaded, now compile them.
  shader->compileShaders();

  // Add a texture to our shader manager object.
  bsg::bsgPtr<bsg::textureMgr> texture = new bsg::textureMgr();
  texture->readFile(bsg::texturePNG, "../data/gladiolas-sq.png");
  shader->addTexture(texture);

  // Do the same for the axes shader:
  bsg::bsgPtr<bsg::shaderMgr> axesShader = new bsg::shaderMgr();
  axesShader->addShader(bsg::GLSHADER_VERTEX, "../src/shader2.vp");
  axesShader->addShader(bsg::GLSHADER_FRAGMENT, "../src/shader.fp");
  axesShader->compileShaders();
  
  // Here are the drawable objects that make up the compound object
  // that make up the scene.

  // We could put the axes and the rectangle in the same compound
  // shape, but we leave them separate so they can be moved
  // separately.
  bigRectangle = new bsg::drawableRectangle(shader, 9.0f, 9.0f, 4);
  smallRectangle = new bsg::drawableRectangle(shader, 3.0f, 5.0f, 2);

  smallRectangle->setPosition(1.0f, 1.0f, 0.5f);
  
  rectGroup1 = new bsg::drawableCollection("rectangles");

  rectGroup1->addObject("big", bigRectangle);
  rectGroup1->addObject("small", smallRectangle);

  scene.addObject("group1", rectGroup1);

  anotherBig = new bsg::drawableRectangle(shader, 6.0f, 3.0f, 3);
  anotherBig->setPosition(0.0, 1.0, -1.0);
  
  anotherSmall = new bsg::drawableRectangle(shader, 2.0f, 2.0f, 2);

  rectGroup3 = new bsg::drawableCollection("more");
  
  rectGroup3->addObject("big2", anotherBig);
  rectGroup3->addObject("small2", anotherSmall);
  rectGroup3->setPosition(2.3f, 3.2f, -2.0f);
  
  mediumRectangle = new bsg::drawableRectangle(shader, 4.0f, 4.0f, 3);

  rectGroup2 = new bsg::drawableCollection("assorted");
  
  rectGroup2->addObject("medium", mediumRectangle);
  rectGroup2->addObject("group3", rectGroup3);
  rectGroup2->setPosition(1.0f, 1.0f, 1.1f);
  
  scene.addObject("group2", rectGroup2);
  
  axes = new bsg::drawableAxes(axesShader, 100.0f);

  scene.addObject(axes);

  // Set some initial positions for the camera and where it's looking.
  scene.setLookAtPosition(glm::vec3(0.0f, 0.0f, 0.0f));
  scene.setCameraPosition(glm::vec3(1.0f, 2.0f, 7.5f));

  std::cout << "Your Scene Graph:" << std::endl << scene << std::endl;
  
  // All the shapes are now added to the scene.

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
