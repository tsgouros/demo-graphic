#include "bsg.h"
#include "bsgMenagerie.h"

#include "MVRDemo.h"

class DemoVRApp: public MVRDemo {

  // Data values that were global in the demo2.cpp file are defined as
  // private members of the VRApp.
private:

  // The scene and the objects in it must be available from the main()
  // function where it is created and the renderScene() function where
  // it is drawn.  The scene object contains all the drawable objects
  // that make up the scene.
  bsg::scene _scene;

  // These are the shapes that make up the scene.  They are out here in
  // the global variables so they can be available in both the main()
  // function and the renderScene() function.
  bsg::drawableText* _helloWorld;
  bsg::drawableCompound* _axesSet;

  // These are part of the animation stuff, and again are out here with
  // the big boy global variables so they can be available to both the
  // interrupt handler and the render function.
  float _oscillator;

  // These variables were not global before, but their scope has been
  // divided into several functions here, so they are class-wide
  // private data objects.
  bsg::bsgPtr<bsg::shaderMgr> _shader;
  bsg::bsgPtr<bsg::shaderMgr> _axesShader;
  bsg::bsgPtr<bsg::lightList> _lights;

  // Here are the drawable objects that make up the compound object
  // that make up the scene.
  bsg::drawableObj _axes;

  std::string _vertexFile;
  std::string _fragmentFile;


  // These functions from demo2.cpp are not needed here:
  //
  //    init()
  //    makeWindow()
  //    resizeWindow()
  //    ... also most of the processKeys() methods.
  //
  // The functionality of these methods is assumed by the MinVR apparatus.

  // This contains a bunch of sanity checks from the graphics
  // initialization of demo2.cpp.  They are still useful with MinVR.
  void _checkContext() {

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

    // Now that we have a graphics context, let's look at what's inside.
    std::cout << "Hardware check: "
              << glGetString(GL_RENDERER)  // e.g. Intel 3000 OpenGL Engine
              << " / "
              << glGetString(GL_VERSION)    // e.g. 3.2 INTEL-8.0.61
              << std::endl;

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
    glLineWidth(4);
    glEnable(GL_LINE_SMOOTH);

  }

  // Just a little debug function so that a user can see what's going on
  // in a non-graphical sense.
  void _showCameraPosition() {

    std::cout << "Camera is at ("
              << _scene.getCameraPosition().x << ", "
              << _scene.getCameraPosition().y << ", "
              << _scene.getCameraPosition().z << ")... ";
    std::cout << "looking at ("
              << _scene.getLookAtPosition().x << ", "
              << _scene.getLookAtPosition().y << ", "
              << _scene.getLookAtPosition().z << ")." << std::endl;
  }

  void _initializeScene() {

    // Create a list of lights.  If the shader you're using doesn't use
    // lighting, and the shapes don't have textures, this is irrelevant.
    _lights->addLight(glm::vec4(0.0f, 0.0f, 3.0f, 1.0f),
                      glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));

    // Create a shader manager and load the light list.
    _shader->addLights(_lights);

    _vertexFile = std::string("../shaders/textShader.vp");
    _shader->addShader(bsg::GLSHADER_VERTEX, _vertexFile);

    _fragmentFile = std::string("../shaders/textShader.fp");
    _shader->addShader(bsg::GLSHADER_FRAGMENT, _fragmentFile);

    // The shaders are loaded, now compile them.
    _shader->compileShaders();

    // Add the text to the scene. For an example of how to draw several
    // drawableText objects using the same texture, see textDemo.cpp.
    _helloWorld = new bsg::drawableText(_shader, "hello world!", 2.0, 
      "../external/freetype-gl/fonts/Vera.ttf",
      glm::vec4(0.0, 1.0, 1.0, 1.0));
    _helloWorld->setPosition(0.0f, 0.0f, 0.0f);
    _scene.addObject(_helloWorld);

    _axesShader->addShader(bsg::GLSHADER_VERTEX, "../shaders/shader2.vp");
    _axesShader->addShader(bsg::GLSHADER_FRAGMENT, "../shaders/shader.fp");
    _axesShader->compileShaders();

    _axesSet = new bsg::drawableAxes(_axesShader, 100.0f);

    // Now add the axes.
    _scene.addObject(_axesSet);

    // All the shapes are now added to the scene.
    std::cout << "Here is your scene graph:" << std::endl << _scene << std::endl;
  }


public:
  DemoVRApp(int argc, char** argv) :
    MVRDemo(argc, argv) {

    // This is the root of the scene graph.
    bsg::scene _scene = bsg::scene();

    // These are tracked separately because multiple objects might use
    // them.
    _shader = new bsg::shaderMgr();
    _axesShader = new bsg::shaderMgr();
    _lights = new bsg::lightList();

    _oscillator = 0.0f;

  }

  /// The MinVR apparatus invokes this method whenever there is a new
  /// event to process.
  void onVREvent(const MinVR::VREvent &event) {

    // event.print();

    // This heartbeat event recurs at regular intervals, so you can do
    // animation with the model matrix here, as well as in the render
    // function.
    // if (event.getName() == "FrameStart") {
    //   const double time = event.getDataAsDouble("ElapsedSeconds");
    //   return;
    // }

    float step = 0.5f;
    float stepAngle = 5.0f / 360.0f;

    // Quit if the escape button is pressed
    if (event.getName() == "KbdEsc_Down") {
      shutdown();
    } else if (event.getName() == "FrameStart") {
      _oscillator = event.getValue("ElapsedSeconds");
    }

    // Print out where you are (where the camera is) and where you're
    // looking.
    // _showCameraPosition();

  }

  /// \brief Set the render context.
  ///
  /// The onVRRender methods are the heart of the MinVR rendering
  /// apparatus.  Some render calls are shared among multiple views,
  /// for example a stereo view has two renders, with the same render
  /// context.
  void onVRRenderContext(const VRState &renderState) {

    // Check if this is the first call.  If so, do some initialization.
   if ((int)renderState.getValue("InitRender") == 1) {
      _checkContext();
      _initializeScene();

      // Make any initializations necessary for the scene and its shaders.
      _scene.prepare();
    }

   // Load the scene models to the GPU.
   _scene.load();
  }

  /// This is the heart of any graphics program, the render function.
  /// It is called each time through the main graphics loop, and
  /// re-draws the scene according to whatever has changed since the
  /// last time it was drawn.
  void onVRRenderScene(const VRState &renderState) {

      // If you want to adjust the positions of the various objects in
      // your scene, you can do that here.
      // glm::vec3 pos = _rectangle->getPosition();
      // pos.x = 2.0f * sin(_oscillator);
      // pos.y = 2.0f * cos(_oscillator);
      // pos.z = -4.0f;
      // _rectangle->setPosition(pos);

    // Move the cube forward and have it circle counterclockwise
    // pos.z = 0.0f;
    // float temp = pos.x;
    // pos.x = pos.y;
    // pos.y = temp;
    // _cube->setPosition(pos);

      // _cube->setRotation(glm::vec3(cos(_oscillator), cos(_oscillator) * M_PI, 0.0f));
      // Now the preliminaries are done, on to the actual drawing.

      // First clear the display.
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      // We let MinVR give us the projection matrix from the render
      // state argument to this method.
      std::vector<float> pm = renderState.getValue("ProjectionMatrix");
      glm::mat4 projMatrix = glm::mat4( pm[0],  pm[1], pm[2], pm[3],
                                        pm[4],  pm[5], pm[6], pm[7],
                                        pm[8],  pm[9],pm[10],pm[11],
                                        pm[12],pm[13],pm[14],pm[15]);

      // The draw step.  We let MinVR give us the view matrix.
      std::vector<float> vm = renderState.getValue("ViewMatrix");
      glm::mat4 viewMatrix = glm::mat4( vm[0],  vm[1], vm[2], vm[3],
                                        vm[4],  vm[5], vm[6], vm[7],
                                        vm[8],  vm[9],vm[10],vm[11],
                                        vm[12],vm[13],vm[14],vm[15]);

      //bsg::bsgUtils::printMat("view", viewMatrix);
      _scene.draw(viewMatrix, projMatrix);

      // We let MinVR swap the graphics buffers.
      // glutSwapBuffers();
    }
};

// The main function is just a shell of its former self.  Just
// initializes a MinVR graphics object and runs it.
int main(int argc, char **argv) {

  // Let's see what arguments the user invoked this program with.
  std::cout << "Invoked with argc=" << argc << " arguments." << std::endl;
  for (int i = 0; i < argc ; i++) {
    std::cout << "argv[" << i << "]: " << std::string(argv[i]) << std::endl;
  }

  // If no config file, throw an error.
  if (argc < 2) {
    throw std::runtime_error("\nNeed to specify a config file.\nTry 'bin/textDemoMinVR -c ../config/desktop-freeglut.xml'");
  }

  // Is the MINVR_ROOT variable set?  MinVR usually needs this to find
  // some important things.
  if (getenv("MINVR_ROOT") == NULL) {
    std::cout << "***** No MINVR_ROOT -- MinVR might not be found *****" << std::endl
              << "MinVR is found (at runtime) via the 'MINVR_ROOT' variable."
              << std::endl << "Try 'export MINVR_ROOT=/my/path/to/MinVR'."
              << std::endl;
  }

  // Initialize the app.
  DemoVRApp app(argc, argv);

  // Run it.
  app.run();

  // We never get here.
  return 0;
}




