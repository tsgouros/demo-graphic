 
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>

// Include GLM
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Some miscellaneous dependencies.
#include <png.h>

namespace bsg {

typedef enum {
  GLDATA_VERTICES   = 0,
  GLDATA_COLORS     = 1,
  GLDATA_NORMALS    = 2,
  GLDATA_TEXCOORDS  = 3
} GLDATATYPE;

typedef enum {
  GLSHADER_VERTEX   = 0,
  GLSHADER_FRAGMENT = 1,
  GLSHADER_GEOMETRY = 2
} GLSHADERTYPE;

typedef enum {
  GLMATRIX_MODEL    = 0,
  GLMATRIX_VIEW     = 1,
  GLMATRIX_PROJECTION = 2
} GLMATRIXTYPE;
   

/// \mainpage Shader Manager
///
/// A set of classes to manage a collection of shaders and objects to
/// make it easy to define an OpenGL 3D object drawn by them.  This is
/// very specifically meant to support OpenGL version 2.1 and GLSL
/// 1.2. I know there are newer and better things out there, but this
/// is designed to support the lowest common denominator among the set
/// of machines that our group works on: student-owned laptops, CCV
/// linux workstations, and the Brown University YURT virtual reality
/// environment, to name a few.
///
/// In structure, this is very much like a Baby Scene Graph package,
/// with a scene consisting of a graph of objects.  This package is a
/// teaching tool, as much as it is a package with which to get useful
/// things done.  If you run out of capacity here, consider using Open
/// Scene Graph before extending this one.
///
/// The important member classes are these:
///
///   lightList -- A list of light positions and colors.
///
///   shaderMgr -- Holds the code for the pieces of a shader
///                collection.  Use this to add a vertex or fragment
///                shader.  Has switches to compile them, and returns
///                the program index when it's needed.  Controls the
///                text of the shader to adjust to match the number of
///                lights and so on.
///
///   bsgPtr -- A smart pointer.  We use it for shaderMgr, so that
///                multiple objects can use the same shader object.
///                We use it from drawableCompound so that you can
///                sub-class those and still have an object you can
///                include in a scene.
///
///   textureMgr -- A class to hold a texture and take care of loading
///                it into the OpenGL slots where it belongs.
///
///   drawableObj -- Contains a set of vertices, colors, normals,
///                texture coordinates, whatever.  Points to a
///                particular shaderMgr for drawing the object.
///
///   drawableCompound -- A collection of drawableObj objects to be
///                considered a single object.  This is where the
///                model matrix is applied, so it's ok if it's a
///                compound of just one drawableObj.
///
///   scene -- A collection of drawableCompound objects that make
///                up a scene.
///

/// \brief A reference counter for a smart pointer to bsg objects.
class bsgPtrRC {
 private:
  int count; // Reference count

 public:
 bsgPtrRC(int start) : count(start) {};

  /// Increment the reference count.
  void addRef() { count++; }
  
  // Decrement and return count.
  int release() { return --count; }
};  

/// \brief A smart pointer to a bsg object.
///
/// A smart pointer to the bsgPtr so that multiple objects can use
/// the same shader object.
///
template <class T>
class bsgPtr {
 private:
  T* _pData;       // The pointer.
  bsgPtrRC* _reference; // The reference count.

 public:
 bsgPtr() : _pData(0) {

    _reference = new bsgPtrRC(1); };
 bsgPtr(T* pValue) : _pData(pValue) { _reference = new bsgPtrRC(1); };
  
  /// Copy constructor
 bsgPtr(const bsgPtr &sp) : _pData(sp._pData), _reference(sp._reference) {
    _reference->addRef();
  }

  /// Destructor.  Decrement the reference count.  If the count
  /// becomes zero, delete the data.
  ~bsgPtr() {
    if (_reference->release() == 0) {
      delete _pData;
      delete _reference;
    }
  }

  T& operator*() { return *_pData; };
  T* operator->() { return _pData; };

  /// Assignment operator.
  bsgPtr<T>& operator=(const bsgPtr<T> &sp) {
    if (this != &sp) {
      // Decrement the old reference count.  If references become
      // zero, delete the data.
      if (_reference->release() == 0) {
        delete _pData;
        delete _reference;
      }

      // Copy the data and reference pointer and increment the
      // reference count.
      _pData = sp._pData;
      _reference = sp._reference;
      _reference->addRef();
    }
    return *this;
  }
};

/// \brief Just a place to park some random utilities.
class bsgUtils {
 public:
  static void printMat(const std::string &name, const glm::mat4 &mat);
};

/// \brief Some data for an object.
///
/// We package the data needed for a piece of data belonging to an
/// object.  The name is the same as the variable name in the shader
/// that will use it, and the ID is the index number by which we can
/// refer to that buffer in the C++ code.
template <class T>
class drawableObjData {
 private:
  std::vector<T> _data;
  
 public:
 drawableObjData(): name("") {
    _data.reserve(50); 
    ID = 0; bufferID = 0;
  };
 drawableObjData(const std::string inName, const std::vector<T> inData) :
  name(inName), _data(inData) {}

  // Copy constructor
 drawableObjData(const drawableObjData &objData) :
  _data(objData.getData()), name(objData.name), ID(objData.ID),
    bufferID(objData.bufferID) {};
    
  /// The name of that data inside a shader.
  std::string name;

  std::vector<T> getData() const { return _data; };
  void addData(T d) { _data.push_back(d); };
  
  // The ID that goes with that name.
  GLuint ID;
  
  /// The ID of the buffer containing that data.
  GLuint bufferID;

  /// A size calculator.
  size_t size() { return _data.size() * sizeof(T); };

  /// Another size calculator.
  int intSize() { return sizeof(T) / sizeof(float); };
};

/// \class lightList
/// \brief A class to manage a list of lights in a scene.
///
/// lightList is a class for managing a list of lights to go with some
/// shader.  The lights are communicated with the shader in two blocks
/// of data, one for the light positions and the other for the light
/// colors.  Since the shaders depend on the specific number of lights
/// in the list, we think of this list as an integral part of the
/// shader's data, even if a few different shaders might refer to the
/// same list.
///
/// The load() and draw() methods of this class will be invoked in the
/// load() and draw() methods of the shaders that depend on them.
/// There may be several shaders that use the same lights, so this may
/// result in multiple loads of the same data.  Optimizing that
/// redundancy out of the system is an exercise left for the reader.
///
class lightList {
 private:

  /// The positions of the lights in the list.
  drawableObjData<glm::vec3> _lightPositions;
  /// The colors of the lights in the list.
  drawableObjData<glm::vec3> _lightColors;

  /// The default names of things in the shaders, put here for easy
  /// comparison or editing.  If you're mucking around with the
  /// shaders, don't forget that these are names of arrays inside the
  /// shader, and that the size of the arrays is set with 'XX', see
  /// the shader constructors below.
  void setupDefaultNames() {
    setNames("lightPosition", "lightColor");
  }

 public:
  lightList() {
    setupDefaultNames();
  };

  /// Set the names of the light positions and colors to be used inside
  /// the shaders.
  void setNames(const std::string &positionName, const std::string &colorName) {
    _lightPositions.name = positionName;
    _lightColors.name = colorName;
  };
    
  /// \brief Add lights to the list.
  ///
  /// Since the shaders are compiled and linked after the number of
  /// lights is set, this is pretty much a one-way street.  Add
  /// lights, but don't subtract them.  If you want to extinguish one,
  /// just move it far away, or dial its intensity way down.
  int addLight(const glm::vec3 &position, const glm::vec3 &color) {
    _lightPositions.addData(position);
    _lightColors.addData(color);
    return _lightPositions.size();
  };
  int addLight(const glm::vec3 &position) {
    glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
    return addLight(position, white);
  };
  
  int getNumLights() { return _lightPositions.getData().size(); };

  // We have mutators and accessors for all the pieces...
  std::vector<glm::vec3> getPositions() { return _lightPositions.getData(); };
  void setPositions(const std::vector<glm::vec3> positions) {
    _lightPositions.getData() = positions;
  };
  GLuint getPositionID() { return _lightPositions.ID; };

  std::vector<glm::vec3> getColors() { return _lightColors.getData(); };
  void setColors(const std::vector<glm::vec3> &colors) { _lightColors.getData() = colors; };
  GLuint getColorID() { return _lightColors.ID; };

  /// ... and also for individual lights.
  void setPosition(const int &i, const glm::vec3 &position) {
    _lightPositions.getData()[i] = position;
  };
  glm::vec3 getPosition(const int &i) { return _lightPositions.getData()[i]; };

  /// \brief Change a light's color.
  void setColor(const int &i, const glm::vec3 &color) {
    _lightColors.getData()[i] = color; };
  glm::vec3 getColor(const int &i) { return _lightColors.getData()[i]; };

  /// Load these lights for use with this program.  This should be
  /// called inside the load() method of the manager object of the
  /// shader that uses them.
  void load(const GLuint programID);

  /// \brief "Draw" these lights.
  ///
  /// Obviously, we don't draw the lights, but we use this method to
  /// update the positino and color of the lights, in case they have
  /// changed since the last scene render.
  void draw(const GLuint programID);  
};

typedef enum {
  texturePNG = 0,
  textureDDS = 1,
  textureBMP = 2
} textureType;


/// \brief A manager of textures and texture files.
///
///  A class to hold a texture and take care of loading it into the
///  OpenGL slots where it belongs.
///
class textureMgr {
 private:
  GLfloat _width, _height;

  GLuint _textureAttribID;
  std::string _textureAttribName;

  void setupDefaultNames() {
    _textureAttribName = std::string("textureSampler");
  };

  GLuint _textureBufferID;

  GLuint loadPNG(const std::string imagePath);
  
 public:
  textureMgr(const textureType &type, const std::string &fileName);
  textureMgr() {};

  void load(const GLuint programID);
  void draw(const GLuint programID);

  GLuint getTextureID() { return _textureBufferID; };

  GLfloat getWidth() { return _width; };
  GLfloat getHeight() { return _height; };
};


///  /brief A collection of shaders that work together as a shader program.
///
///  Holds the code for the pieces of a shader collection.  Use this
///  to add a vertex or fragment shader.  Has switches to compile
///  them, and returns the program index when it's needed.  Controls
///  the text of the shader to adjust to match the number of lights
///  and so on.
class shaderMgr {
 private:
  /// The shader text and compilation log together are stored here, 
  /// using the GLSHADERTYPE as an index to keep them straight.
  std::vector<std::string> _shaderText;
  std::vector<std::string> _shaderLog;
  std::vector<GLuint> _shaderIDs;
  
  std::string _linkLog;
  
  GLuint _programID;

  /// Tells us whether the shaders have been loaded and compiled yet.
  bool _compiled;
  
  bsgPtr<lightList> _lightList;

  std::string _getShaderInfoLog(GLuint obj);
  std::string _getProgramInfoLog(GLuint obj);

 public:
  shaderMgr() {
    // Easiest way to initialize a non-static three-element
    // std::vector.  Dumb, but simple and it works.
    _shaderIDs.push_back(999);
    _shaderIDs.push_back(999);
    _shaderIDs.push_back(999);
    _shaderText.push_back("");
    _shaderText.push_back("");
    _shaderText.push_back("");
    _shaderLog.push_back("");
    _shaderLog.push_back("");
    _shaderLog.push_back("");
    _compiled = false;
  };
  ~shaderMgr() {
    if (_compiled) glDeleteProgram(_programID);
  }

  
  /// \brief Add lights to the shader.
  ///
  /// This should be done before adding the shader code itself, unless
  /// the shader does not depend on the number of lights.  The shader
  /// manager class will edit any 'XX' string in the shader and
  /// replace it with the number of lights in this list.  If your
  /// shader ignores lighting, as many do, this will not do anything
  /// besides issue a polite warning that the shader doesn't care.
  void addLights(const bsgPtr<lightList> lightList);

  /// \brief Add a shader to the program.
  ///
  /// You must specify at least a vertex and fragment shader.  The
  /// geometry shader is optional.
  void addShader(const GLSHADERTYPE type, const std::string &shaderFile);

  /// \brief Compile and link the loaded shaders.
  ///
  /// You need to have specified at least a vertex and fragment
  /// shader.  The geometry shader is optional.
  void compileShaders();

  /// Get the ID number for an attribute name that appears in a shader.
  GLuint getAttribID(const std::string &attribName);

  /// Get the ID number for a uniform name that appears in a shader.
  GLuint getUniformID(const std::string &unifName);

  /// \brief Returns the program ID of the compiled shader.
  GLuint getProgram() { return _programID; };

  /// \brief Use this to enable the shader program.
  ///
  /// This call should appear before any of the OpenGL calls that rely
  /// on this shader program, like enabling a buffer or loading an
  /// attribute's data.  OpenGL uses "state", and this call puts the
  /// GPU in a state of being ready to use this shader.
  void useProgram() { glUseProgram(_programID); };
  
};

/// \brief The information necessary to draw an object.
///
/// This object contains a set of vertices, colors, normals, texture
/// coordinates.  This is meant to work with modern OpenGL, that uses
/// shaders, specifically OpenGL 2.1 and GLSL 1.2.  Yes, we know
/// that's old, but it's the latest version that works identically on
/// *all* the platforms we want to support.  (Thanks, Apple.)
/// 
/// All the drawableObj shapes in a compound object (see below) use the
/// same shader, and the same model matrix.
class drawableObj {
 private:

  // Specifies whether this is a triangle, a triangle strip, fan,
  // whatever.
  GLenum _drawType;
  GLsizei _count;

  // These are the components.  Apart from the vertices, they don't
  // all have to be filled in, though they have to work with the
  // shader.
  drawableObjData<glm::vec4> _vertices;
  drawableObjData<glm::vec4> _colors;
  drawableObjData<glm::vec4> _normals;
  drawableObjData<glm::vec2> _uvs;
  
  std::string print() const { return std::string("drawableObj"); };
  friend std::ostream &operator<<(std::ostream &os, const drawableObj &obj);
  
 public:
  drawableObj() {};

  /// \brief Specify the draw type of the shape.
  ///
  /// This refers to the OpenGL primitive draw types.  You can read
  /// about them here: https://www.khronos.org/opengl/wiki/Primitive
  ///
  /// This is a nice intro:
  /// http://www.falloutsoftware.com/tutorials/gl/gl3.htm
  void setDrawType(const GLenum drawType) {
    _drawType = drawType;
    _count = _vertices.size();
  };

  /// \brief Specify the draw type and the vertex count.
  void setDrawType(const GLenum drawType, const GLsizei count) {
    _drawType = drawType;
    _count = count;
  };

  /// \brief Add some vec4 data.
  ///
  /// You can add vec4 data, including vertices, colors, and normal
  /// vectors, with this method.  The name parameter is the name
  /// you'll use in the shader for the corresponding attribute.
  void addData(const GLDATATYPE type,
               const std::string &name,
               const std::vector<glm::vec4> &data);

  /// \brief Add some vec2 texture coordinates.
  ///
  /// You can add vec2 texture coordinates, with this method.  The
  /// name parameter is the name you'll use in the shader for the
  /// corresponding attribute.
  void addData(const GLDATATYPE type,
               const std::string &name,
               const std::vector<glm::vec2> &data);

  /// \brief One-time-only draw preparation.
  ///
  /// This generates the proper number of buffers for the shape data
  /// and arranges the correspondence between the attribute names used
  /// in your shaders and the ID numbers used in the OpenGL calls that
  /// load data to be used for those attributes.  For example, you'll
  /// want to know that the buffer with some specific buffer ID is the
  /// same as the attribute called "position" in the shader.  You
  /// don't have to worry about this, except to the extent you have to
  /// make sure that the names set in the addData method are the same
  /// names you're using in the shader attributes.
  ///
  /// Call this function after all the data is in place and we know
  /// whether we have colors or textures or normals to worry about.
  void prepare(GLuint programID);

  /// \brief Loads the shape about to be drawn.
  ///
  /// This binds the OpenGL buffers for the shape data and loads that
  /// data into those buffers.  The load step is separate from the
  /// draw step because you might want to draw several times, for
  /// example for a stereo display where you have to draw twice.
  void load();

  /// \brief This is the actual step of drawing the object.
  ///
  /// The method binds each OpenGL buffer, then enables the arrays.
  /// We assume the data we want to draw is already in the buffer, via
  /// the load() method.
  void draw();
};

/// \brief A collection of drawableObj objects.
///
/// A compound drawable object is made of a bunch of drawableObj
/// objects but can be considered to be a considered a single object.
/// It might consist of just one object, but that's ok, since this is
/// also where the objects are placed in model space.
///
/// This is to say that this class is where the model matrix is
/// handled.  The component objects must specify their vertex
/// coordinates in the same coordinate system as each other.  If you
/// want to move objects independently of each other, use different
/// drawableCompound objects.  (Or consider using a real scene graph
/// API, or rewriting this one.)  The view matrix and the projection
/// matrix are used here, though they are generated and managed at the
/// scene level.
///
/// The shaders are included in this object as a pointer because many
/// objects will use the same shader.  So the program that calls this
/// should keep three separate lists: the objects in the scene, the
/// shaders used to render them, and the lights used by those shaders.
///
/// This class imposes a small number of restrictions on the shader
/// code itself, mostly that the matrix names in the shader must match
/// the matrix names here.  There is a setMatrixNames() method for
/// that.  Setting things up for the number of lights is also
/// something that needs to be configured carefully.  See the
/// lightList documentation.
///
class drawableCompound {
 private:

  /// The list of objects that make up this compound object.
  std::list<drawableObj> _objects;

  /// The shader that will be used to render all the pieces of this
  /// compound object.  Or at least the one they will start with.  You
  /// can always go back and change the shader for an individual
  /// object.
  bsgPtr<shaderMgr> _pShader;

  /// The position in model space.
  glm::vec3 _position;
  /// A three-dimensional scale parameter.
  glm::vec3 _scale;
  /// The orientation of the object in model space.
  glm::quat _orientation;

  /// The model matrix, along with a flag to say whether it must be
  /// recalculated.  The flag is set when the position, scale, or
  /// orientation are changed.
  glm::mat4 _modelMatrix;
  bool _modelMatrixNeedsReset;

  /// These are pairs of ways to reference the matrices that include
  /// the matrix name (used in the shader) and the ID (used in the
  /// OpenGL code).  
  std::string _modelMatrixName;
  GLuint _modelMatrixID;
  
  std::string _viewMatrixName;
  GLuint _viewMatrixID;

  std::string _projMatrixName;
  GLuint _projMatrixID;
  
 public:
 drawableCompound(bsgPtr<shaderMgr> pShader) :
  _pShader(pShader),
    // Set the default names for our matrices.
    _modelMatrixName("modelMatrix"),
    _viewMatrixName("viewMatrix"),
    _projMatrixName("projMatrix") {
    _position = glm::vec3(0.0f, 0.0f, 0.0f);
    _scale = glm::vec3(1.0f, 1.0f, 1.0f);
    // The glm::quat constructor initializes orientation to be zero
    // rotation by default, so need not be mentioned here.
  };

  /// \brief Set the name of one of the matrices.
  ///
  /// This is the name by which this matrix will be known inside your
  /// shaders.  In other words, these strings must match the "uniform"
  /// declarations in your shaders.
  void setMatrixName(GLMATRIXTYPE type, const std::string name) {
    switch(type) {
    case(GLMATRIX_MODEL):
      _modelMatrixName = name;
      break;
    case(GLMATRIX_VIEW):
      _viewMatrixName = name;
      break;
    case(GLMATRIX_PROJECTION):
      _projMatrixName = name;
      break;
    }
  };
 
  /// \brief Adds an object to the compound object.
  ///
  /// We set the shader to each object to be the same shader for the
  /// whole compound object.  If you find that objectionable, you are
  /// probably ready for a more elaborate set of classes to do your
  /// rendering with.
  void addObject(drawableObj obj) {
    _objects.push_back(obj);
  };    

  /// \brief Calculate the model matrix.
  ///
  /// Uses the current position, rotation, and scale to calculate a
  /// new model matrix.  There is an internal flag used to set whether
  /// the model matrix needs to be recalculated or not.
  glm::mat4 getModelMatrix();
  int getNumObjects() { return _objects.size(); };

  /// \brief Set the model position using a vector.
  void setPosition(glm::vec3 position) {
    _position = position;
    _modelMatrixNeedsReset = true;
  };
  /// \brief Set the model position using three floats.
  void setPosition(GLfloat x, GLfloat y, GLfloat z) {
    setPosition(glm::vec3(x, y, z));
  };
  /// \brief Set the scale using a vector.
  void setScale(glm::vec3 scale) {
    _scale = scale;
    _modelMatrixNeedsReset = true;
  };
  /// \brief Set the scale using a single float, applied in three dimensions.
  void setScale(float scale) {
    _scale = glm::vec3(scale, scale, scale);
    _modelMatrixNeedsReset = true;
  };
  /// \brief Set the rotation with a quaternion.
  void setOrientation(glm::quat orientation) {
    _orientation = orientation;
    _modelMatrixNeedsReset = true;
  };
  /// \brief Set the rotation with Euler angles.
  ///
  /// Uses a 3-vector of (pitch, yaw, roll) in radians.
  void setRotation(glm::vec3 pitchYawRoll) {
    _orientation = glm::quat(pitchYawRoll);      
    _modelMatrixNeedsReset = true;
  };

  /// \brief Returns the vector position.
  glm::vec3 getPosition() { return _position; };
  /// \brief Returns the vector scale.
  glm::vec3 getScale() { return _scale; };
  /// \brief Returns the orientation as a quaternion.
  glm::quat getOrientation() { return _orientation; };
  /// \brief Returns the orienation as Euler angles.
  glm::vec3 getPitchYawRoll() { return glm::eulerAngles(_orientation); };

  /// \brief Gets ready for the drawing sequence.
  void prepare();
  
  /// \brief Load a compound object.
  ///
  /// Prepares a compound object to be drawn, including bringing up a
  /// refreshed model matrix, in case we've moved its position or
  /// orientation.
  void load();

  /// \brief Draws a compound object.
  ///
  /// Just executes draw() for all the component objects.
  void draw(const glm::mat4 &viewMatrix,
            const glm::mat4 &projMatrix);

};

/// \brief A collection of drawableCompound objects that make up a
/// scene.
///
/// A scene is a collection of objects to render, and is also where
/// the view and projection matrices are handled to turn the scene
/// into an image.
///
class scene {
 private:
  /// We use a pointer to the drawableCompound objects so you can
  /// create an object that inherits from drawableCompound and still
  /// use it here.
  typedef std::list<bsgPtr<drawableCompound> > compoundList;
  compoundList _compoundObjects;

  glm::mat4 _viewMatrix;
  glm::mat4 _projMatrix;

  // View matrix inputs.
  glm::vec3 _cameraPosition;
  glm::vec3 _lookAtPosition;

  // Projection matrix inputs;
  float _fov, _aspect;
  float _nearClip, _farClip;
  
  glm::mat4 _calculateViewMatrix();
  glm::mat4 _calculateProjMatrix();
  
 public:
  scene() {
    _cameraPosition = glm::vec3(10.0f, 10.0f, 10.0f);
    _lookAtPosition = glm::vec3( 0.0f,  0.0f,  0.0f);
    _fov = M_PI / 2.0f;
    _aspect = 1.0f;
    _nearClip = 0.1f;
    _farClip = 100.0f;
  }

  void setCameraPosition(const glm::vec3 cameraPosition) {
    _cameraPosition = cameraPosition;
  };
  void addToCameraPosition(const glm::vec3 stepVec) {
    _cameraPosition += stepVec;
  }
  glm::vec3 getCameraPosition() { return _cameraPosition; };

  void setLookAtPosition(const glm::vec3 lookAtPosition) {
    _lookAtPosition = lookAtPosition;
  };
  void addToLookAtPosition(const glm::vec3 stepVec) {
    _lookAtPosition += stepVec;
  }
  glm::vec3 getLookAtPosition() { return _lookAtPosition; };

  /// \brief Rotates the camera location around the lookat point.
  void addToCameraViewAngle(const float horizAngle, const float vertAngle);

  /// \brief Return the view matrix.
  ///
  /// This function recalculates the view matrix, based on the camera
  /// position and lookat point.
  glm::mat4 getViewMatrix() {
    _viewMatrix = _calculateViewMatrix();
    return _viewMatrix;
  }

  /// \brief Return a projection matrix.
  ///
  /// This function recalculates a projection matrix based on the
  /// window size (aspect ratio) and clip parameters.
  glm::mat4 getProjMatrix() {
    _projMatrix = _calculateProjMatrix();
    return _projMatrix;
  }

  /// \brief Set the field of view.  In radians.
  void setFOV(float fov) { _fov = fov; };

  /// \brief Sets the aspect ratio of the view window.
  void setAspect(float aspect) { _aspect = aspect; };

  /// \brief Add a compound object to our scene.
  void addCompound(const bsgPtr<drawableCompound> pCompoundObject) {
      _compoundObjects.push_back( pCompoundObject);
  }

  /// \brief Prepare the scene to be drawn.
  ///
  /// This does a bunch of one-time-only initializations for the
  /// member compound elements.
  void prepare();

  /// \brief Generates a projection matrix and loads all the compound elements.
  void load();
  /// \brief Loads all the compound elements.
  ///
  /// But you supply the projection matrix.
  void load(glm::mat4 &projMatrix);

  /// \brief Generates a view matrix and draws all the compound elements.
  void draw();

  /// \brief Draws all the compound elements.
  ///
  /// But you supply the view matrix.
  void draw(glm::mat4 &viewMatrix);
  
};


}
