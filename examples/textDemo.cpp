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
bsg::drawableAxes* axes;
bsg::drawableText* text1;
bsg::drawableText* text2;
bsg::drawableTextBox* textBox;
bsg::drawableTextRect* textRect;
bsg::drawableRectangle* rect;
bsg::drawableRectangleOutline* outline;
bsg::drawableCollection* collection;

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
    break;
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
  makeWindow(100, 100, 1000, 700);

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

  // Do the same for the axes shader, and add the axes to the scene
  bsg::bsgPtr<bsg::shaderMgr> axesShader = new bsg::shaderMgr();
  axesShader->addShader(bsg::GLSHADER_VERTEX, "../shaders/shader2.vp");
  axesShader->addShader(bsg::GLSHADER_FRAGMENT, "../shaders/shader.fp");
  axesShader->compileShaders();

  axes = new bsg::drawableAxes(axesShader, 100.0f);
  scene.addObject(axes);

  // Make the drawableText texture shader
  bsg::bsgPtr<bsg::shaderMgr> textShader = new bsg::shaderMgr();
  textShader->addLights(lights);
  std::string vertexFile = std::string("../shaders/textShader.vp");
  textShader->addShader(bsg::GLSHADER_VERTEX, vertexFile);
  std::string fragmentFile = std::string("../shaders/textShader.fp");
  textShader->addShader(bsg::GLSHADER_FRAGMENT, fragmentFile);
  textShader->compileShaders();

  // Draw the first text object, which creates its own fontTextureMgr.
  text1 = new bsg::drawableText(textShader, "Hello", 0.3,
      "../external/freetype-gl/fonts/Vera.ttf",
      glm::vec4(1.0, 1.0, 1.0, 1.0));
  text1->setPosition(1.f, 0.f, -1.0f);
  scene.addObject(text1);

  // Retrieve that fontTextureMgr and reuse it for the second texture object.
  bsg::bsgPtr<bsg::fontTextureMgr> texture = text1->getFontTexture();

  // Even though we're reusing the textureMgr from earlier, we can feed the text
  // object a different font, and it'll handle that. So we can use just one
  // fontTextureMgr throughout the whole project, provided that the atlas
  // doesn't run out of space (see comments in bsgMenagerie.cpp for details).
  text2 = new bsg::drawableText(textShader, "Goodbye", 0.3,
      "../external/freetype-gl/fonts/Vera.ttf",
      glm::vec4(1.0, 1.0, 1.0, 1.0), texture);
  text2->setPosition(-1.0f, 0.0f, -1.0f);
  scene.addObject(text2);

  // This next part shows the creation of a drawableTextRect or drawableTextBox.
  // These are drawableCollections that can be used as buttons or menu items.

  // First, create a separate shaderMgr for objects that just use solid colors.
  bsg::bsgPtr<bsg::shaderMgr> backgroundShader = new bsg::shaderMgr();
  backgroundShader->addLights(lights);
  std::string vertexFile2 = std::string("../shaders/shader2.vp");
  backgroundShader->addShader(bsg::GLSHADER_VERTEX, vertexFile2);
  std::string fragmentFile2 = std::string("../shaders/shader.fp");
  backgroundShader->addShader(bsg::GLSHADER_FRAGMENT, fragmentFile2);
  backgroundShader->compileShaders();

  // Here's a drawableTextBox. Creating a drawableTextRect is the same,
  // except that it doesn't have an extrusion (the last arg).
  // drawableTextBox and drawableTextRect have a bunch more arguments that you
  // can use to fine-tune the background color, border width, border color,
  // and box height and width, etc. Here, though, we just use the default args.
  textBox = new bsg::drawableTextBox(textShader, backgroundShader, "hello",
    "../external/freetype-gl/fonts/Lobster-Regular.ttf", texture, 0.2f);
  scene.addObject(textBox);

  // Set some initial positions for the camera and where it's looking.
  scene.setLookAtPosition(glm::vec3(0.0f, 0.0f, 0.0f));
  scene.setCameraPosition(glm::vec3(0, 0, 1));
  
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
