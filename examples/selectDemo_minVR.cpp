#include "bsg.h"
#include "bsgMenagerie.h"

#include <api/MinVR.h>

class DemoVRApp: public MinVR::VRApp {
private:
  bsg::scene _scene;

  float _oscillator;
  float _oscillationStep;

  std::vector<bsg::drawableRectangle*> rects;
  bsg::drawableCollection* rectGroup;

  bsg::drawableCompound* _axesSet;
  bsg::bsgPtr<bsg::shaderMgr> _shader;
  bsg::bsgPtr<bsg::lightList> _lights;
  std::string _vertexFile;
  std::string _fragmentFile;
  bsg::bsgPtr<bsg::drawableObj> _axes;
  std::vector<glm::vec3>   velocities;
  std::vector<glm::vec3>   angVelocities;

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

  }

  // Just a little debug function so that a user can see what's going on
// in a non-graphical sense.
  void showCameraPosition() {

    std::cout << "Camera is at ("
              << _scene.getCameraPosition().x << ", "
              << _scene.getCameraPosition().y << ", "
              << _scene.getCameraPosition().z << ")... ";
    std::cout << "looking at ("
              << _scene.getLookAtPosition().x << ", "
              << _scene.getLookAtPosition().y << ", "
              << _scene.getLookAtPosition().z << ")." << std::endl;
  }

  void _initializeScene(){

    _lights->addLight(glm::vec4(10.0f, 10.0f, 10.0f, 1.0f),
                      glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));
    _lights->addLight(glm::vec4(10.0f,-10.0f, 10.0f, 1.0f),
                      glm::vec4(0.0f, 1.0f, 1.0f, 0.0f));

  // Now we load the shaders.  First check to see if any have been
  // specified on the command line.
  

  // Create a shader manager and load the light list.
  
    _shader->addLights(_lights);

  // Add the shaders to the manager, first the vertex shader...
    std::string vertexFile = std::string("../shaders/shader2.vp");
    _shader->addShader(bsg::GLSHADER_VERTEX, vertexFile);

  // ... then the fragment shader.  You could potentially add a
  // geometry shader at this point.
    std::string fragmentFile = std::string("../shaders/shader.fp");
    _shader->addShader(bsg::GLSHADER_FRAGMENT, fragmentFile);

  // The shaders are loaded, now compile them.
    _shader->compileShaders();

  // Add a texture to our shader manager object.
  
  // Do the same for the axes shader:
    _axes = new bsg::drawableObj();

    std::vector<glm::vec4> axesVertices;
    axesVertices.push_back(glm::vec4( -100.0f, 0.0f, 0.0f, 1.0f));
    axesVertices.push_back(glm::vec4( 100.0f, 0.0f, 0.0f, 1.0f));
  
    axesVertices.push_back(glm::vec4( 0.0f, -100.0f, 0.0f, 1.0f));
    axesVertices.push_back(glm::vec4( 0.0f, 100.0f, 0.0f, 1.0f));

    axesVertices.push_back(glm::vec4( 0.0f, 0.0f, -100.0f, 1.0f));
    axesVertices.push_back(glm::vec4( 0.0f, 0.0f, 100.0f, 1.0f));

    _axes->addData(bsg::GLDATA_VERTICES, "position", axesVertices);
    std::vector<glm::vec4> axesColors;
    axesColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));
    axesColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));

    axesColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
    axesColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));

    axesColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));
    axesColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));

    _axes->addData(bsg::GLDATA_COLORS, "color", axesColors);

    // The axes are not triangles, but lines.
    _axes->setDrawType(GL_LINES);
  // Here are the drawable objects that make up the compound object
  // that make up the scene.

  // Make a group for all our rectangles.
    rectGroup = new bsg::drawableCollection("rectangles");

  // Now generate them and add them to the group.
    for (int i = 0; i < 10; i++) {

      bsg::drawableCompound* b = new bsg::drawableCompound(_shader); //new bsg::drawableRectangle(shader, 2.0f, 2.0f, 2);

      bsg::bsgPtr<bsg::drawableObj> topShape = new bsg::drawableObj();
      
  // Specify the vertices of the shapes we're drawing.  Note that the
  // faces are specified with a *counter-clockwise* winding order, the
  // OpenGL default.  You can make your faces wind the other
  // direction, but have to adjust the OpenGL expectations with
  // glFrontFace().
    std::vector<glm::vec4> topShapeVertices;

    // These would take many fewer vertices if they were specified as a
    // triangle strip.
    topShapeVertices.push_back(glm::vec4( 4.3f, 4.3f, 4.3f, 1.0f));
    topShapeVertices.push_back(glm::vec4( 6.1f, 1.1f, 1.1f, 1.0f));
    topShapeVertices.push_back(glm::vec4( 1.1f, 6.1f, 1.1f, 1.0f));

    topShapeVertices.push_back(glm::vec4( 6.1f, 1.1f, 1.1f, 1.0f));
    topShapeVertices.push_back(glm::vec4( 4.3f, 4.3f, 4.3f, 1.0f));
    topShapeVertices.push_back(glm::vec4( 1.1f, 1.1f, 6.1f, 1.0f));

    topShapeVertices.push_back(glm::vec4( 4.3f, 4.3f, 4.3f, 1.0f));
    topShapeVertices.push_back(glm::vec4( 1.1f, 6.1f, 1.1f, 1.0f));
    topShapeVertices.push_back(glm::vec4( 1.1f, 1.1f, 6.1f, 1.0f));

    topShapeVertices.push_back(glm::vec4( 1.1f, 6.1f, 1.1f, 1.0f));
    topShapeVertices.push_back(glm::vec4( 6.1f, 1.1f, 1.1f, 1.0f));
    topShapeVertices.push_back(glm::vec4( 1.1f, 1.1f, 6.1f, 1.0f));

    topShape->addData(bsg::GLDATA_VERTICES, "position", topShapeVertices);

    // Here are the corresponding colors for the above vertices.
    std::vector<glm::vec4> topShapeColors;
    topShapeColors.push_back(glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f));
    topShapeColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));
    topShapeColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));

    topShapeColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));
    topShapeColors.push_back(glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f));
    topShapeColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));

    topShapeColors.push_back(glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f));
    topShapeColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
    topShapeColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));

    topShapeColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
    topShapeColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));
    topShapeColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));

    topShape->addData(bsg::GLDATA_COLORS, "color", topShapeColors);

    // The vertices above are arranged into a set of triangles.
    topShape->setDrawType(GL_TRIANGLES);  

  // Same thing for the other tetrahedron.
  
  
    b->addObject(topShape);
  


    b->setPosition(-5.0f + 0.1f * (rand()%100),
                   -5.0f + 0.1f * (rand()%100),
                   -5.0f + 0.1f * (rand()%100));
    b->setRotation(0.0f, 0.0f, 0.0f);

    rectGroup->addObject(b);

    // Generate some velocities to play with these objects.
    velocities.push_back(glm::vec3(-0.05 + 0.001 * (rand()%100),
                                   -0.05 + 0.001 * (rand()%100),
                                   -0.05 + 0.001 * (rand()%100)));
    angVelocities.push_back(glm::vec3(-0.05 + 0.001 * (rand()%100),
                                      -0.05 + 0.001 * (rand()%100),
                                      -0.05 + 0.001 * (rand()%100)));

  }

  // You can define an iterator on a drawableCollection object.  Just
  // print a few of the names in the object.
  int i = 0;
  for (bsg::drawableCollection::iterator it = rectGroup->begin();
       it != rectGroup->end(); it++) {
    if (++i < 10) std::cout << "name: " << it->first << " for "
                            << it->second->getName() << std::endl;
  }


  _scene.addObject(rectGroup);
  _axesSet = new bsg::drawableCompound(_shader);
  _axesSet->addObject(_axes);

    // Now add the axes.
  _scene.addObject(_axesSet);

  }

public:
  DemoVRApp(int argc, char** argv) :
    MinVR::VRApp(argc, argv) {
std::cout << "hey0" << std::endl;
    // This is the root of the scene graph.
    bsg::scene _scene = bsg::scene();

    // These are tracked separately because multiple objects might use
    // them.
    _shader = new bsg::shaderMgr();
    _lights = new bsg::lightList();

    
    float _oscillator = 0.0f;
    float _oscillationStep = 0.03f;

  }

  void onVREvent(const MinVR::VREvent &event) {
        
    //event.print();
        
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
      _oscillator = event.getDataAsFloat("ElapsedSeconds");
    }

    // Print out where you are (where the camera is) and where you're
    // looking.
    // _showCameraPosition();
    
  }

  void onVRRenderGraphicsContext(const MinVR::VRGraphicsState &renderState) {

    // Check if this is the first call.  If so, do some initialization. 
    if (renderState.isInitialRenderCall()) {
      _checkContext();
      _initializeScene();
      _scene.prepare();
    }
  }

  void onVRRenderGraphics(const MinVR::VRGraphicsState &renderState){
    if(isRunning()){
      glm::vec3 pos = rectGroup->getPosition();
      _oscillator += _oscillationStep;
      pos.x = sin(_oscillator);
      pos.y = 1.0f - cos(_oscillator);
      rectGroup->setPosition(pos);

      bsg::bsgNameList rectNames = rectGroup->getNames();

  // Now generate them and add them to the group.
      std::vector<glm::vec3>::iterator jt = velocities.begin();
      std::vector<glm::vec3>::iterator kt = angVelocities.begin();
      for (bsg::drawableCollection::iterator it = rectGroup->begin();
           it != rectGroup->end(); it++, jt++, kt++) {

        glm::vec3 rot = it->second->getPitchYawRoll();
        it->second->setRotation(rot + (*kt));

        // Move the rectangles, but confine them to a cube.
        glm::vec3 pos = it->second->getPosition();
        if (fabs(pos.x) > 5.0f || fabs(pos.y) > 5.0f || fabs(pos.z) > 5.0f) {
          *jt = - (*jt);
        }
        it->second->setPosition(pos + *jt);
      }

      bsg::bsgNameList inside = rectGroup->insideBoundingBox(glm::vec4(0.0, 0.0, 0.0, 0.0));

      if (!inside.empty())
        // This is how you print out a name list.  For this example, the names
        // will only have a single element.  But you might want to change that.
        for (bsg::bsgNameList::iterator it = inside.begin();
             it != inside.end(); it++) {
          std::cout << "test point is inside rectangle " << *it;
          glm::vec3 pos = rectGroup->getObject(*it)->getPosition();
          std::cout << " at position (" << pos.x << "," << pos.y << "," << pos.z << ")";
          std::cout << std::endl;
        }

  

  // First clear the display.
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      const float* pm = renderState.getProjectionMatrix();
      glm::mat4 projMatrix = glm::mat4( pm[0],  pm[1], pm[2], pm[3],
                                        pm[4],  pm[5], pm[6], pm[7],
                                        pm[8],  pm[9],pm[10],pm[11],
                                        pm[12],pm[13],pm[14],pm[15]);
      //bsg::bsgUtils::printMat("proj", projMatrix);
      _scene.load();

      // The draw step.  We let MinVR give us the view matrix.
      const float* vm = renderState.getViewMatrix();
      glm::mat4 viewMatrix = glm::mat4( vm[0],  vm[1], vm[2], vm[3],
                                        vm[4],  vm[5], vm[6], vm[7],
                                        vm[8],  vm[9],vm[10],vm[11],
                                        vm[12],vm[13],vm[14],vm[15]);

      //bsg::bsgUtils::printMat("view", viewMatrix);
      _scene.draw(viewMatrix, projMatrix);

      glutSwapBuffers();
    }
  }
};





int main(int argc, char **argv) {

  std::cout << "This will display a bunch of floating rectangles, and when one of them" << std::endl << "happens over the worldspace origin, will print a message to that effect." << std::endl;

 std::cout << "Invoked with argc=" << argc << " arguments." << std::endl;
  for (int i = 0; i < argc ; i++) {
    std::cout << "argv[" << i << "]: " << std::string(argv[i]) << std::endl;
  }

  // If there weren't enough args, throw an error and explain what the
  // user should have done.
  if (argc < 4) {
      
  }

  // Is the MINVR_ROOT variable set?  MinVR usually needs this to find
  // some important things.
  if (getenv("MINVR_ROOT") == NULL) {
    std::cout << "***** No MINVR_ROOT -- MinVR might not be found *****" << std::endl 
              << "MinVR is found (at runtime) via the 'MINVR_ROOT' variable."
              << std::endl << "Try 'export MINVR_ROOT=/my/path/to/MinVR'."
              << std::endl;
  }
  std::cout << "hey0" << std::endl;
  // Initialize the app.
  DemoVRApp app(argc, argv);
 std::cout << "hey" << std::endl;
  // Run it.
  app.run();
  // We never get here, but the compiler is annoyed when you don't
  // exit from a function.
  return(0);
}
