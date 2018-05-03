#include "bsg.h"
#include "bsgMenagerie.h"
#include <api/MinVR.h>
#include <map>
class DemoVRApp: public MinVR::VRApp {

  // Data values that were global in the demo2.cpp file are defined as
  // private members of the VRApp.
private:

  // The scene and the objects in it must be available from the main()
  // function where it is created and the renderScene() function where
  // it is drawn.  The scene object contains all the drawable objects
  // that make up the scene.
  bsg::scene _scene;
glm::mat4 projMatrix;
   glm::mat4 viewMatrix ;

   const float * _pm;
   const float * _vm;
  // These are the shapes that make up the scene.  They are out here
  // in the variables global to this object so they can be available
  // in both the run() function and the renderScene() function.
  bsg::drawableCompound* _tetrahedron;
  bsg::drawableCompound* _axesSet;

  // These are part of the animation stuff, and again are out here with
  // the big boy global variables so they can be available to both the
  // interrupt handler and the render function.
  float _oscillator;

  // These variables were not global before, but their scope has been
  // divided into several functions here, so they are class-wide
  // private data objects.
  bsg::bsgPtr<bsg::shaderMgr> _shader;
  bsg::bsgPtr<bsg::lightList> _lights;

  // Here are the drawable objects that make up the compound object
  // that make up the scene.
  bsg::bsgPtr<bsg::drawableObj> _axes;
  bsg::bsgPtr<bsg::drawableObj> _topShape;
  bsg::bsgPtr<bsg::drawableObj> _bottomShape;

  std::string _vertexFile;
  std::string _fragmentFile;

  

  float _oscillationStep;

  std::vector<bsg::drawableRectangle*> rects;
  bsg::drawableCollection* rectGroup;


  std::vector<glm::vec3>   velocities;
  std::vector<glm::vec3>   angVelocities;

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
    for (int i = 0; i < 2; i++) {

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

    // This is the root of the scene graph.
    bsg::scene _scene = bsg::scene();

    // These are tracked separately because multiple objects might use
    // them.
    _shader = new bsg::shaderMgr();
    _lights = new bsg::lightList();

    _oscillator = 0.0f;
    _oscillationStep = 0.03f;
    _vertexFile = std::string(argv[1]);
    _fragmentFile = std::string(argv[2]);


  }

	/// The MinVR apparatus invokes this method whenever there is a new
	/// event to process.
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

  /// \brief Set the render context.
  ///
  /// The onVRRender methods are the heart of the MinVR rendering
  /// apparatus.  Some render calls are shared among multiple views,
  /// for example a stereo view has two renders, with the same render
  /// context.
  void onVRRenderGraphicsContext(const MinVR::VRGraphicsState &renderState) {

    // Check if this is the first call.  If so, do some initialization. 
    if (renderState.isInitialRenderCall()) {
      _checkContext();
      _initializeScene();
      _scene.prepare();
    }
  }

  glm::mat4 getProjMatrix(){
    return projMatrix;
  }

  const float* getProjMatrix2(){
    return _pm;
  }

  glm::mat4 getViewMatrix(){
    return viewMatrix;
  }

  bsg::drawableCollection* getGroup(){
    return rectGroup;
  }

  void project(){
  //   //start at 0, do from there up to z
  //   //iterate through objects, if x, y is 0 then check distance for z

    int bestDist = std::numeric_limits<int>::max();
    bsg::bsgPtr<bsg::drawableMulti> best;
    for (bsg::drawableCollection::iterator it = rectGroup->begin();
            it != rectGroup->end(); it++) {

         glm::vec3 pos = it->second->getPosition();
         if ((fabs(pos.x) > 0 && fabs(pos.x) < 1)|| (fabs(pos.y) > 0 && fabs(pos.y)<1)) {
            int dist = fabs(pos.z) * fabs(pos.z);
            if (dist < bestDist){ 
              best = it->second;
              bestDist = dist;
            }
         }
        
       }
       std::cout << best->getName() << std::endl;
   }

  /// This is the heart of any graphics program, the render function.
  /// It is called each time through the main graphics loop, and
  /// re-draws the scene according to whatever has changed since the
  /// last time it was drawn.
   
	void onVRRenderGraphics(const MinVR::VRGraphicsState &renderState) {
		// Only draw if the application is still running.
		if (isRunning()) {
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
      //project();
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
      _pm = pm;
      projMatrix = glm::mat4( pm[0],  pm[1], pm[2], pm[3],
                                        pm[4],  pm[5], pm[6], pm[7],
                                        pm[8],  pm[9],pm[10],pm[11],
                                        pm[12],pm[13],pm[14],pm[15]);
      //bsg::bsgUtils::printMat("proj", projMatrix);
      _scene.load();

      // The draw step.  We let MinVR give us the view matrix.
      const float* vm = renderState.getViewMatrix();
      viewMatrix = glm::mat4( vm[0],  vm[1], vm[2], vm[3],
                                        vm[4],  vm[5], vm[6], vm[7],
                                        vm[8],  vm[9],vm[10],vm[11],
                                        vm[12],vm[13],vm[14],vm[15]);

    
    
      //bsg::bsgUtils::printMat("view", viewMatrix);
      _scene.draw(viewMatrix, projMatrix);

      //glutSwapBuffers();
    }
  }


 
};

DemoVRApp *_app;

void try_this(int x, int y){
  int window_width = glutGet(GLUT_WINDOW_WIDTH);
  int window_height = glutGet(GLUT_WINDOW_HEIGHT);

//   GLbyte color[4];
//   GLfloat depth;
//   GLuint index;
//   glm::vec4 viewport = glm::vec4(0, 0, window_width, window_height);
//   glm::vec3 wincoord = glm::vec3(x, window_height - y - 1, depth);
  
//   glm::vec3 startcoord = glm::vec3(x, window_height - y - 1, 0.0);

//   glm::vec3 endcoord = glm::vec3(x, window_height - y - 1, 1.0);
//   glm::vec3 rayStart =glm::unProject(startcoord,_app->getViewMatrix(), _app->getProjMatrix(), viewport);
//   glm::vec3 rayEnd = glm::unProject(endcoord, _app->getViewMatrix(), _app->getProjMatrix(), viewport);

// for (bsg::drawableCollection::iterator it = rectGroup->begin();
//            it != rectGroup->end(); it++) {

//         bsg::bsgPtr<bsg::drawableMulti> curr = it->second;

//         glm::vec3 pos = curr->getPosition();
//         float x = (fabs(pos.x) - objcoord.x)* (fabs(pos.x) - objcoord.x);
//         float y = (fabs(pos.y) - objcoord.y) * (fabs(pos.y) - objcoord.y);
//         float z = (fabs(pos.z) - objcoord.z) * (fabs(pos.z) - objcoord.z);
        
//         float squared = x + y + z;

//         if(squared < max){
//           max = squared;
//           best = curr;
//         }
//       }

}

glm::vec3 closestPoint(const glm::vec3 A, const glm::vec3 B, const glm::vec3 P, double *t){
  glm::vec3 AB = B - A;
  double ab_square = AB[0] * AB[0] + AB[1] * AB[1] + AB[2] * AB[2];

  glm::vec3 AP = P - A;
  double ap_dot_ab = AP[0] * AB[0] + AP[1] * AB[1] + AP[2] * AB[2];
  *t = ap_dot_ab/ab_square;
  return A +  AB;
}
void mouseMove(int button, int state, int x, int y) 
{
  int window_width = glutGet(GLUT_WINDOW_WIDTH);
  int window_height = glutGet(GLUT_WINDOW_HEIGHT);

  GLbyte color[4];
  GLfloat depth;
  GLuint index;
  
  glReadPixels(x, window_height - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
  glReadPixels(x, window_height - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
  glReadPixels(x, window_height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

  printf("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u\n",
         x, y, color[0], color[1], color[2], color[3], depth, index);
   
  glm::vec4 viewport = glm::vec4(0, 0, window_width, window_height);
  glm::vec3 wincoord = glm::vec3(x, window_height - y - 1, depth);
  glm::vec3 objcoord = glm::unProject(wincoord, _app->getViewMatrix(), _app->getProjMatrix(), viewport);

 

  printf("Coordinates in object space: %f, %f, %f\n",
         objcoord.x, objcoord.y, objcoord.z); 

  float max = std::numeric_limits<int>::max();
  bsg::bsgPtr<bsg::drawableMulti> best;
  bsg::bsgPtr<bsg::drawableObj> topShape;
    bsg::drawableCollection* rectGroup = _app->getGroup();
  for (bsg::drawableCollection::iterator it = rectGroup->begin();
           it != rectGroup->end(); it++) {

        bsg::bsgPtr<bsg::drawableMulti> curr = it->second;
        // Move the rectangles, but confine them to a cube.
      //std::cout << it* <<std::endl;
        glm::vec3 pos = curr->getPosition();
        float x = (fabs(pos.x) - objcoord.x)* (fabs(pos.x) - objcoord.x);
        float y = (fabs(pos.y) - objcoord.y) * (fabs(pos.y) - objcoord.y);
        float z = (fabs(pos.z) - objcoord.z) * (fabs(pos.z) - objcoord.z);
        
        float squared = x + y + z;
        best->getName();
        if(squared < max){
          max = squared;
          best = curr;
          //topShape = rectGroup->getObject(best->getName());
        }
      }

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


    //best->addData(bsg::GLDATA_COLORS, "color", topShapeColors);

    std::cout << "point is closest to rectangle " << best->getName();
    std::cout << std::endl;
    

}

// The main function is just a shell of its former self.  Just
// initializes a MinVR graphics object and runs it.
int main(int argc, char **argv) {

  // Let's see what arguments the user invoked this program with.
  std::cout << "Invoked with argc=" << argc << " arguments." << std::endl;
  for (int i = 0; i < argc ; i++) {
    std::cout << "argv[" << i << "]: " << std::string(argv[i]) << std::endl;
  }

  // If there weren't enough args, throw an error and explain what the
  // user should have done.
  if (argc < 4) {
    throw std::runtime_error("\nNeed three args, including the names of a vertex and fragment shader.\nTry 'bin/demo3 ../shaders/shader2.vp ../shaders/shader.fp -c ../config/desktop-freeglut.xml'");

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
	//DemoVRApp app(argc, argv);

  DemoVRApp *appObj = new DemoVRApp(argc,argv);
  _app = appObj;
  glutMouseFunc(mouseMove);
  glEnable(GL_DEPTH_TEST);
  // Run it.
	//app.run();
  appObj->run();
  // We never get here.
	return 0;
}



  

