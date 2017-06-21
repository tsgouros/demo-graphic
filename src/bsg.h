#ifndef BSGHEADER
#define BSGHEADER

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
#include <map>
#include <iostream>
#include <fstream>

// Include GLM
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

// Some miscellaneous dependencies.
//#include <jpeglib.h>
//#include <jerror.h>

#ifndef M_PI
#define M_PI 3.141592653589793238
#endif

namespace bsg {

typedef enum {
  GLDATA_VERTICES   = 0,  //! For vertex data.
  GLDATA_COLORS     = 1,  //! Color data.
  GLDATA_NORMALS    = 2,  //! Normal data.  Should be normalized.
  GLDATA_TEXCOORDS  = 3   //! Texture coordinates, also called UVs.
} GLDATATYPE;

typedef enum {
  GLSHADER_VERTEX   = 0,  //! This is a vertex shader.
  GLSHADER_FRAGMENT = 1,  //! This is a fragment shader.
  GLSHADER_GEOMETRY = 2   //! This is a geometry shader.
} GLSHADERTYPE;

typedef enum {
  GLMATRIX_MODEL    = 0,   //! This is a model matrix.
  GLMATRIX_VIEW     = 1,   //! This is a view matrix.
  GLMATRIX_PROJECTION = 2, //! This is a projection matrix.
  GLMATRIX_NORMAL = 3      //! This is a 'normal' matrix, or an inverse model matrix.
} GLMATRIXTYPE;


/// \mainpage Baby Scene Graph
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
/// In structure, this is very much a "Baby" Scene Graph package,
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
///                compound of just one drawableObj.  There is no
///                capacity for independent control of the member
///                objects of a drawableCompound.  If that's what you
///                want, use drawableCollection.
///
///   drawableCollection -- A collection of drawableCompound objects
///                that make up a scene.  Can also contain a bunch of
///                drawableCollection objects, making this into a
///                scene graph API.  You can access the member objects
///                independently, by name or hash.
///
///   \ref scene -- Contains a "root" drawableCollection, along with some
///                facilities for managing view and projection
///                matrices and for controlling the rendering of the
///                whole scene.  Use this as the basis of a scene, and
///                add objects and groups of objects to it.
///
///  For an introduction to this library, start with the \ref scene object.

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
 bsgPtr() : _pData(0) { _reference = new bsgPtrRC(1); };
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

  operator bool() const { return _pData != 0; };

  T& operator*() { return *_pData; };
  T* operator->() const { return _pData; };
  T* ptr() const { return _pData; }; // Use this for casts.

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

#define bPtr(T,P) dynamic_cast<T*>(P.ptr())

/// \brief Just a place to park some random utilities.
class bsgUtils {
 public:
  static void printMat(const std::string &name, const glm::mat4 &mat);
  static void printVec3(const std::string intro, const glm::vec3 in) {
    std::cout << intro << "(" << in.x << "," << in.y << "," << in.z << ")" << std::endl;}
  static void printVec4(const std::string intro, const glm::vec4 in) {
    std::cout << intro << "(" << in.x << "," << in.y << "," << in.z << "," << in.w << ")" << std::endl;}

};

/// \brief Some data for an OpenGL object.
///
/// For most OpenGL objects referencing data used in a shader, there
/// are three important pieces of information.  First, there is the
/// name, the string of characters used in the shader itself.  Then
/// there is the number with which OpenGL C++ function calls refer to
/// that name.  We call this the ID, but the OpenGL documentation
/// refers to this as the "name" (which seems a quite unfortunate
/// hijacking of a formerly useful word).  The third important piece
/// is the buffer ID, the number by which we refer to the memory in
/// which the data is placed. And there's the data, too.
///
/// This object is meant to group these three pieces of information
/// with the data to which it all refers.  It's mostly convenience,
/// augmented by a bunch of convenience methods to get the start
/// address of the data and the length in different units and so on.
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
  void setData(const std::vector<T> data) { _data = data; };

  T* beginAddress() { return &_data[0]; };

  T operator[](const int i) { return _data[i]; };

  // The ID that goes with that name.
  GLint ID;

  /// The ID of the buffer containing that data.
  GLuint bufferID;

  /// Is there any data in here?
  bool empty() { return _data.empty(); };

  /// A size calculator. Total number of bytes.
  size_t byteSize() { return _data.size() * sizeof(T); };

  /// Another size calculator.
  size_t size() { return _data.size(); };

  /// Yet another size calculator.
  size_t componentsPerVertex() { return sizeof(T) / sizeof(float); };
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
  drawableObjData<glm::vec4> _lightPositions;
  /// The colors of the lights in the list.
  drawableObjData<glm::vec4> _lightColors;

  /// The default names of things in the shaders, put here for easy
  /// comparison or editing.  If you're mucking around with the
  /// shaders, don't forget that these are names of arrays inside the
  /// shader, and that the size of the arrays is set with 'XX', see
  /// the shader constructors below.
  void _setupDefaultNames() {
    setNames("lightPositionWS", "lightColor");
  }

 public:
  lightList() {
    _setupDefaultNames();
  };

  /// \brief Control the lighting names used in the shader.
  ///
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
  int addLight(const glm::vec4 &position, const glm::vec4 &color) {
    _lightPositions.addData(position);
    _lightColors.addData(color);
    return _lightPositions.size();
  };
  int addLight(const glm::vec4 &position) {
    glm::vec4 white = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    return addLight(position, white);
  };

  int getNumLights() { return _lightPositions.size(); };

  // We have mutators and accessors for all the pieces...
  std::vector<glm::vec4> getPositions() { return _lightPositions.getData(); };
  void setPositions(const std::vector<glm::vec4> positions) {
    _lightPositions.setData(positions);
  };
  GLuint getPositionID() { return _lightPositions.ID; };

  std::vector<glm::vec4> getColors() { return _lightColors.getData(); };
  void setColors(const std::vector<glm::vec4> &colors) {
    _lightColors.setData(colors);
  };
  GLuint getColorID() { return _lightColors.ID; };

  /// ... and also for individual lights.
  void setPosition(const int &i, const glm::vec4 &position) {
    _lightPositions[i] = position;
  };
  glm::vec4 getPosition(const int &i) { return _lightPositions[i]; };

  /// \brief Change a light's color.
  void setColor(const int &i, const glm::vec4 &color) {
    _lightColors[i] = color; };
  glm::vec4 getColor(const int &i) { return _lightColors[i]; };

  /// \brief Link the light data with whatever shader is in use.
  ///
  /// Load these lights for use with this program.  This should be
  /// called inside the load() method of the manager object of the
  /// shader that uses them.
  //
  // This must be preceded by a glUseProgram(programID) call.
  void load(const GLint programID);

  /// \brief "Draw" these lights.
  ///
  /// Obviously, we don't draw the lights, but we use this method to
  /// update the position and color of the lights, in case they have
  /// changed since the last scene render, and this is where the light
  /// positions and colors are loaded into the shader's uniforms.
  //
  // This must be preceded by a glUseProgram(programID) call.
  void draw();
};

typedef enum {
  texturePNG = 0, //! Use for a PNG file.
  textureDDS = 1, //! Not implemented.
  textureBMP = 2, //! Not implemented.
  textureCHK = 3, //! Will provide a checkerboard texture.
  textureJPG = 4  //! Not implemented.
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

  void _setupDefaultNames() {
    _textureAttribName = std::string("textureImage");
  };

  GLuint _textureBufferID;

  GLuint _loadPNG(const std::string imagePath);
  GLuint _loadCheckerBoard (const int size, int numFields);
  
 public:
  textureMgr() { _setupDefaultNames(); };

  /// \brief Reads a texture from an image file.
  ///
  /// The type can be texturePNG, in which case fileName better be a
  /// PNG file name, or textureCHK, in which case you get a
  /// checkerboard pattern, and the file name is ignored.
  void readFile(const textureType &type, const std::string &fileName);

  /// \brief Prepare the texture to be rendered.
  ///
  /// Meant to be used during the shaderMgr.load() step.
  void load(const GLuint programID);

  /// \brief Call this just before the draw.
  ///
  /// Binds the texture for use by OpenGL.  This is meant to be used
  /// during the shaderMgr.draw() step.
  void draw();

  /// \brief Return the ID of the texture buffer.
  GLuint getTextureID() { return _textureBufferID; };

  /// \brief Return the texture width.
  GLfloat getWidth() { return _width; };
  /// \brief Return the texture height.
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
  std::vector<std::string> _shaderFiles;
  std::vector<std::string> _shaderLog;
  std::vector<GLint> _shaderIDs;

  std::string _linkLog;

  GLuint _programID;

  /// Tells us whether the shaders have been loaded and compiled yet.
  bool _compiled;

  bsgPtr<lightList> _lightList;

  bsgPtr<textureMgr> _texture;
  bool _textureLoaded;

  std::string _getShaderInfoLog(GLuint obj);
  std::string _getProgramInfoLog(GLuint obj);

 public:
  shaderMgr() {
    // Easiest way to initialize a non-static three-element
    // std::vector.  Dumb, but simple and it works.
    _shaderIDs.push_back(-1);
    _shaderIDs.push_back(-1);
    _shaderIDs.push_back(-1);
    _shaderText.push_back("");
    _shaderText.push_back("");
    _shaderText.push_back("");
    _shaderLog.push_back("");
    _shaderLog.push_back("");
    _shaderLog.push_back("");
    _shaderFiles.push_back("");
    _shaderFiles.push_back("");
    _shaderFiles.push_back("");
    _lightList = new lightList();
    _compiled = false;
    _textureLoaded = false;
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

  /// \brief Add a texture to the shader.
  ///
  /// This will make a single 2D texture available as an option to the
  /// fragment shader.  If you want something more elaborate, you
  /// probably don't want to be using this package.
  void addTexture(const bsgPtr<textureMgr> texture) {
    _texture = texture;
    _textureLoaded = true;
  };

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

  /// \brief Sanity check could go here.
  ///
  /// It would be nice to have a real sanity check of a shader --
  /// comparing its text to the data in the object to be drawn --
  /// before it is used.  That could be done here.
  void prepare() {};

  /// \brief Prepare shader data to be used in a draw.
  ///
  /// Gets things like the light list ready to be used in a shader.
  void load();

  /// \brief Use shader data in a render.
  ///
  /// Actually loads data like the light list to be used in the shader.
  void draw();
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
 protected:

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

  bool _loadedIntoBuffer;

  /// Some data for selectability and managing of bounding boxes.
  bool _selectable;
  bool _haveBoundingBox;
  glm::vec4 _vertexBoundingBoxLower, _vertexBoundingBoxUpper;
  float _boundingBoxMin;

  // This data is for taking the component data and creating an
  // interleaved buffer with an index array.  This is supposed to be
  // an optimization.  The vertex position is always zero, and the
  // vertices array is not optional, so there is no vertexPos variable.
  bool _interleaved;
  GLshort _colorPos, _normalPos, _uvPos, _stride;
  drawableObjData<float> _interleavedData;

  void _getAttribLocations(GLuint programID);
  void _prepareSeparate(GLuint programID);
  void _prepareInterleaved(GLuint programID);
  void _loadSeparate();
  void _loadInterleaved();
  void _drawSeparate();
  void _drawInterleaved();

 public:
 drawableObj() :
  _loadedIntoBuffer(false),
    _interleaved(false),
    _selectable(true),
    _boundingBoxMin(0.1),
    _haveBoundingBox(false) {};

  /// \brief Set up the buffers to be interleaved,
  void setInterleaved(bool interleaved) { _interleaved = interleaved; };

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
  ///
  /// The count refers here to the number of vertices, *not* the
  /// number of triangles, line segments, quads, whatever.
  void setDrawType(const GLenum drawType, const GLsizei count) {
    _drawType = drawType;
    _count = count;
  };

  /// \brief Set bounding box minimum dimension.
  ///
  /// This is for less-than-3D objects, like rectangles, points, or
  /// lines, so their bounding box has some width.
  void setBoundingBoxMin(const float &min) { _boundingBoxMin = min; };

  /// \brief Get bounding box minimum dimension.
  float getBoundingBoxMin() { return _boundingBoxMin; };

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

  /// \brief Change the underlying data of an object.
  ///
  /// Use this to reset the vec4 data inside an object.
  void setData(const GLDATATYPE type, const std::vector<glm::vec4>& data);

  /// \brief Change the underlying data of an object.
  ///
  /// Use this to reset the vec2 data inside an object.
  void setData(const GLDATATYPE type, const std::vector<glm::vec2>& data);

  /// \brief Set whether the object is selectable.
  ///
  /// Often used for things like axes that you probably don't want to
  /// select, but that might get in the way.
  void setSelectable(const bool &selectable) { _selectable = selectable; };

  /// \brief Find a bounding box for the object.
  ///
  /// Scans the vertex array to come up with a bounding box.
  void findBoundingBox();

  /// \brief Returns the upper limit of the bounding box.
  glm::vec4 getBoundingBoxUpper() {
      if (!_haveBoundingBox) findBoundingBox();
      return _vertexBoundingBoxUpper;
  }
  /// \brief Returns the lower limit of the bounding box.
  glm::vec4 getBoundingBoxLower() {
    if (!_haveBoundingBox) findBoundingBox();
    return _vertexBoundingBoxLower;
  }

  /// \brief Test whether a test point is inside the bounding box.
  ///
  /// There are two arguments here.  Because the test is done in world
  /// space, the vertices of the bounding box must be transformed with
  /// the model matrix before testing.
  bool insideBoundingBox(const glm::vec4 &testPoint,
                         const glm::mat4 &modelMatrix);

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

/// \brief The name of an object as it exists in the scene hierarchy.
///
/// A bsgName is a name that specifies an object that is possibly
/// situated within a complicated scene hierarchy.  So a "name"
/// consists of a list of strings, one for each level of the
/// hierarchy.  This was a typdef for a list of strings, and it is
/// pretty much still just that, but was turned into its own class in
/// order to have a prettier print() method.  One name specifies an
/// object.  See scene.getObject.
class bsgName : public std::list<std::string> {
 private:
  std::string printName() const;
  friend std::ostream &operator<<(std::ostream &os, const bsgName &name) {
    return os << name.printName();
  }
};

/// \brief A list of qualified strings to identify an object.
///
/// Use this type to specify a collection of objects throughout the
/// hierarchy of the scene graph.
typedef std::list<bsgName> bsgNameList;

/// \brief An abstract class to handle transformation matrices.
///
/// This class is the common root of drawableCompound and
/// drawableCollection objects, the leaves and branches of our scene
/// graph.  It handles the basics of a transformation matrix, which
/// both of those classes need.  Beyond the shared functionality,
/// having both those objects inherit from the same parent allows us
/// to define a pointer object (bsgPtr) that can point equally well to both.
///
/// This class is where the model matrix is handled, and it is handled
/// by combining the internal model matrix (with all the position,
/// orientation, and scale parameters) with all the model matrices of
/// the parents above it.
///
class drawableMulti {
 protected:

  // Do not use a smart pointer here.  Since it is not a copy of
  // another pointer, it will not have a correct reference count.
  drawableMulti* _parent;

  std::string _name;

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

  void _init() {
    _position = glm::vec3(0.0f, 0.0f, 0.0f);
    _scale = glm::vec3(1.0f, 1.0f, 1.0f);
    // The glm::quat constructor initializes orientation to be zero
    // rotation by default, so need not be mentioned here.
    _modelMatrixNeedsReset = true;
  };

 public:
 drawableMulti() : _parent(0), _name("") { _init(); };
 drawableMulti(std::string name) : _parent(0), _name(name) { _init(); };
  virtual ~drawableMulti() {};

  /// \brief Attach this object to a parent object.
  ///
  /// Our scene graph is doubly connected in order to provide the
  /// correct nested transformation from model space to world space.
  void setParent(drawableMulti* p) { _parent = p; }

  /// \brief Set the name of this object.
  void setName(const std::string name) { _name = name; };

  /// \brief Get the object name.
  std::string getName() { return _name; };

  /// \brief Calculate the model matrix.
  ///
  /// Uses the current position, rotation, and scale to calculate a
  /// new model matrix.  There is an internal flag used to set whether
  /// the model matrix needs to be recalculated or not.
  glm::mat4 getModelMatrix();

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
  /// \brief Set the rotation with Euler angles.
  ///
  /// Specifies the pitch (x), yaw (y), and roll (z) rotations
  /// individually, in radians.
  void setRotation(GLfloat pitch, GLfloat yaw, GLfloat roll) {
    _orientation = glm::quat(glm::vec3(pitch, yaw, roll));
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

  /// \brief Returns a list of object names if the given point is
  /// within some object's bounding box.
  ///
  /// The calculation is to be done in world space, using all the
  /// available transformation matrices in place, but not the view or
  /// projection matrix.
  virtual bsgNameList insideBoundingBox(const glm::vec4 &testPoint) = 0;

  /// \brief Returns the names involved in this object.
  ///
  ///
  virtual bsgNameList getNames() = 0;

  /// \brief Retrieve an object by name.
  ///
  /// Used in drawableCollection.  If there is no name found, returns
  /// NULL.  So be wary of the return value, unless you like hunting
  /// segfaults.
  virtual bsgPtr<drawableMulti> getObject(const std::string &name) {
    return NULL;
  }

  /// \brief Retrieve an object by a fully-qualified names.
  ///
  /// Used in drawableCollection.  If there is no object found, returns
  /// NULL.  So be wary of segfaults.
  virtual bsgPtr<drawableMulti> getObject(bsgName name) {
    return NULL;
  }

  /// \brief Delete an object by name.
  ///
  /// Used in drawableCollection.  If there is no name found, returns
  /// NULL.  So be wary of segfaults.
  virtual bsgPtr<drawableMulti> delObject(const std::string &name) {
    return NULL;
  }

  /// \brief Delete an object by a fully-qualified names.
  ///
  /// Used in drawableCollection.  If there is no object found, returns
  /// NULL.  So be wary of segfaults.
  virtual bsgPtr<drawableMulti> delObject(bsgName name) {
    return NULL;
  }

  /// \brief A dopey static method to generate a random name.
  static std::string randomName(const std::string &nameRoot);

  /// \brief Returns a printable version of the object.
  virtual std::string printObj(const std::string &prefix) const = 0;

  /// \brief Gets ready for the drawing sequence.
  ///
  virtual void prepare() = 0;

  /// \brief Loads an object, gives it a transformation matrix to use.
  ///
  /// Prepares an object to be drawn, including updating its model
  /// matrix with whatever position or orientation changes have been
  /// made, and multiplying it by the input matrix.
  virtual void load() = 0;

  /// \brief Draws an object.
  ///
  /// Just executes draw() using the given view and projection matrices.
  virtual void draw(const glm::mat4 &viewMatrix,
                    const glm::mat4 &projMatrix) = 0;

};


/// \brief A collection of drawableObj objects.
///
/// A compound drawable object is made of a bunch of drawableObj
/// objects but can be considered to be a single object.  It might
/// consist of just one object, which might seem a waste, but that's
/// ok, since this is also where the objects are placed in model
/// space.  If there are multiple component objects, they must specify
/// their vertex coordinates in the same coordinate system as each
/// other, and they are pretty much stuck there.  They are not
/// designed to move relative to each other.  If you want to move
/// objects independently of each other, use multiple drawableCompound
/// objects within a drawableCollection.  (Or consider using a real
/// scene graph API, like OSG.)  The view matrix and the projection
/// matrix are used here, though they are generated and managed at the
/// scene level.
///
/// Important: We expect one of these will be the leaf nodes to every
/// scene graph branch.
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
class drawableCompound : public drawableMulti {
 protected:

  /// The list of objects that make up this compound object.
  typedef std::list<bsgPtr<drawableObj> > DrawableObjList;
  DrawableObjList _objects;

  /// The shader that will be used to render all the pieces of this
  /// compound object.  Or at least the one they will start with.  You
  /// can always go back and change the shader for an individual
  /// object.
  bsgPtr<shaderMgr> _pShader;

  /// This is the model matrix of this object, but also all its
  /// parents, multiplied into one matrix.
  glm::mat4 _totalModelMatrix;

  /// We also keep around the inverse transpose model matrix, for
  /// texture processing.
  glm::mat4 _normalMatrix;

  /// These are pairs of ways to reference the matrices that include
  /// the matrix name (used in the shader) and the ID (used in the
  /// OpenGL code).
  std::string _modelMatrixName;
  GLuint _modelMatrixID;

  std::string _normalMatrixName;
  GLuint _normalMatrixID;

  std::string _viewMatrixName;
  GLuint _viewMatrixID;

  std::string _projMatrixName;
  GLuint _projMatrixID;

  friend std::ostream &operator<<(std::ostream &os,
                                  const drawableCompound &comp) {
    return os << comp.printObj("");  }

 public:
 drawableCompound(bsgPtr<shaderMgr> pShader) :
  drawableMulti(),
    _pShader(pShader),
    // Set the default names for our matrices.
    _modelMatrixName("modelMatrix"),
    _normalMatrixName("normalMatrix"),
    _viewMatrixName("viewMatrix"),
    _projMatrixName("projMatrix") {
    _name = randomName("obj");
  };
 drawableCompound(const std::string name, bsgPtr<shaderMgr> pShader) :
  drawableMulti(name),
    _pShader(pShader),
    // Set the default names for our matrices.
    _modelMatrixName("modelMatrix"),
    _normalMatrixName("normalMatrix"),
    _viewMatrixName("viewMatrix"),
    _projMatrixName("projMatrix") {
  };

  // The equipment to allow us to define an iterator over this class.
  typedef DrawableObjList::iterator iterator;
  typedef DrawableObjList::const_iterator const_iterator;

  /// \brief Returns an iterator to the first element of the object.
  iterator begin() { return _objects.begin(); }

  /// \brief Returns an iterator to the end of the object.
  iterator end() { return _objects.end(); }


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
    case(GLMATRIX_NORMAL):
      _normalMatrixName = name;
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
  void addObject(bsgPtr<drawableObj> &pObj) {
    _objects.push_back(pObj);
  };

  /// \brief Add an object's bounding box to a compound object.
  ///
  /// Does not add the object, but just an outline of its bounding box.
  void addObjectBoundingBox(bsgPtr<drawableObj> &obj);

  /// \brief How many objects are in this compound?
  int getNumObjects() { return _objects.size(); };

  /// \brief Returns the name of this object.
  bsgNameList getNames() { bsgNameList out; return out;}

  /// \brief Is a given test point within the bounding box of this object?
  ///
  /// If the test point is inside any of the bounding boxes of the
  /// sub-objects, this will return a non-empty name list.  (It's not
  /// empty, but the bsgName it contains is empty.)
  bsgNameList insideBoundingBox(const glm::vec4 &testPoint);

  /// \brief A printable representation of the object.
  std::string printObj(const std::string &prefix) const {
    return prefix + "<drawableCompound:" + _name + ">"; }

  /// \brief Gets ready for the drawing sequence.
  ///
  void prepare();

  /// \brief Loads an object, gives it a transformation matrix to use.
  ///
  /// Prepares an object to be drawn, including updating its model
  /// matrix with whatever position or orientation changes have been
  /// made, and multiplying it by the input matrix.
  void load();

  /// \brief Draws an object.
  ///
  /// Just executes draw() using the given view and projection matrices.
  void draw(const glm::mat4 &viewMatrix,
            const glm::mat4 &projMatrix);

};

/// \brief A collection of drawable objects.
///
/// This is the heart of a scene graph: a collection of drawable
/// objects that can be nested arbitrarily deeply.  Each object
/// manages a transformation matrix that it applies to all of its
/// children, as well as a pointer to a parent object so the
/// transformations can be applied in the right order.  The objects
/// are named, so they can be addressed and modified individually.
///
/// To use a scene graph, create a collection object like this one to
/// be the root, making sure it has no parent.  Add objects, or groups
/// of objects to it, at will.  If you don't assign names to the added
/// objects, it will come up with a hash-y sort of random-looking name
/// for you.  There is a getNames() method so you can learn these
/// random names, but it might be better just to pick sensible names
/// if you intend to manipulate the scene after creation.
///
class drawableCollection : public drawableMulti {

  /// We use a pointer to the drawableCompound objects so you can
  /// create an object that inherits from drawableCompound and still
  /// use it here.
  typedef std::map<std::string, bsgPtr<drawableMulti> > CollectionMap;
  CollectionMap _collection;

  friend std::ostream &operator<<(std::ostream &os,
                                  const drawableCollection &coll) {
    return os << coll.printObj("  ");
  }

 public:
  /// \brief The default constructor.
  ///
  /// If you use this constructor, you'll get a randomish name.
  drawableCollection();
  /// \brief A constructor with a specified name for the collection.
  drawableCollection(const std::string name);

  // The equipment to allow us to define an iterator over this class.
  // The iterators are each a std::pair with a name and a pointer to a
  // drawableMulti object.
  typedef CollectionMap::iterator iterator;
  typedef CollectionMap::const_iterator const_iterator;

  /// \brief Returns an iterator to the first element of the object.
  iterator begin() { return _collection.begin(); }

  /// \brief Returns an iterator to the end of the object.
  iterator end() { return _collection.end(); }

  /// \brief Add an object to our list.
  ///
  /// Using the given name.  You can add objects without a name, too,
  /// in which case the name is randomly assigned.  Returns the name
  /// assigned to the object.  If there is already a name for the
  /// object, it is overwritten.
  std::string addObject(const std::string name,
                        const bsgPtr<drawableMulti> &pMultiObject);

  /// \brief Add an object to our list.
  ///
  /// Add an object to the list using the name it already has.  This
  /// might have been a randomly-assigned name, so if you intend to
  /// manipulate the object in the scene later, you might consider
  /// giving it a better name here.
  std::string addObject(const bsgPtr<drawableMulti> &pMultiObject);

  /// \brief Remove an object from the list.
  ///
  /// Delete an object from the list. Returns a pointer to the removed
  /// object, but it's not in the list any more.
  bsgPtr<drawableMulti> delObject(const std::string &name);

  /// \brief Remove an object from somewhere in the graph.
  ///
  /// Delete an object from the subgraph represented by this
  /// list. Returns a pointer to the removed object, but it's not in
  /// the list any more.
  bsgPtr<drawableMulti> delObject(bsgName name);

  /// \brief Retrieve an object by name.
  ///
  /// You'll be getting something that might be a drawableCompound and
  /// might be a drawableCollective.  That is, it might be a leaf
  /// node, and might be a branch.  Returns NULL if no match, so be
  /// careful.
  bsgPtr<drawableMulti> getObject(const std::string &name);

  /// \brief Retrieve an object by a list of names.
  ///
  /// You'll be getting something that might be a drawableCompound and
  /// might be a drawableCollective.  That is, it might be a leaf
  /// node, and might be a branch.  Returns NULL if no match, so be
  /// careful.
  bsgPtr<drawableMulti> getObject(bsgName name);

  /// \brief Return a list of object names in the collection.
  bsgNameList getNames();

  /// \brief Returns the names of objects containing the test point.
  ///
  /// Returns a collection of the names of objects containing the test
  /// point.  Note that a "name" is actually a vector of names, one
  /// for each level of the hierarchy.
  bsgNameList insideBoundingBox(const glm::vec4 &testPoint);

  /// \brief Returns a printable display of the collection.
  std::string printObj(const std::string &prefix) const;

  /// \brief Gets ready for the drawing sequence.
  ///
  void prepare();

  /// \brief Loads an object, gives it a transformation matrix to use.
  ///
  /// Prepares an object to be drawn, including updating its model
  /// matrix with whatever position or orientation changes have been
  /// made, and multiplying it by the input matrix.
  void load();

  /// \brief Draws an object.
  ///
  /// Just executes draw() using the given view and projection
  /// matrices.
  void draw(const glm::mat4 &viewMatrix,
            const glm::mat4 &projMatrix);

};

/// \brief A collection of drawable objects that make up a scene.
///
/// A scene is a collection of objects to render, and is also where
/// the view and projection matrices are handled to turn the scene
/// into an image.  There is a root drawableCollection object to hold
/// the scene graph tree, and management capacity for things like the
/// view and projection matrices.
///
/// Like many of the other objects in this library, the scene is meant
/// to be prepared, loaded, and drawn.  The scene object will manage
/// those chores for most of the other objects, though.
///
/// \code
/// scene = bsg::scene();
///
/// scene.addObject(...) ...
///
/// scene.prepare();
///
/// scene.load();
///
/// scene.draw(scene.getViewMatrix(), scene.getProjMatrix());
/// \endcode
///
/// Use the addObject
///
///
class scene {
 private:

  drawableCollection _sceneRoot;

  glm::mat4 _viewMatrix;
  glm::mat4 _projMatrix;

  // View matrix inputs.
  glm::vec3 _cameraPosition;
  glm::vec3 _lookAtPosition;

  // Projection matrix inputs;
  float _fov, _aspect;
  float _nearClip, _farClip;

  /// \brief Returns a string representation of the scene graph.
  ///
  std::string _printTree() const { return _sceneRoot.printObj("  "); };

  friend std::ostream &operator<<(std::ostream &os, const scene &scene) {
    return os << scene._printTree();
  }

 public:
  scene() {
    _sceneRoot = drawableCollection("sceneRoot");
    _cameraPosition = glm::vec3(10.0f, 10.0f, 10.0f);
    _lookAtPosition = glm::vec3( 0.0f,  0.0f,  0.0f);
    _fov = M_PI / 2.0f;
    _aspect = 1.0f;
    _nearClip = 0.1f;
    _farClip = 100.0f;
  }

  /// \brief Where is the eye position?
  void setCameraPosition(const glm::vec3 cameraPosition) {
    _cameraPosition = cameraPosition;
  };

  /// \brief Increments the camera position.
  void addToCameraPosition(const glm::vec3 stepVec) {
    _cameraPosition += stepVec;
  }

  /// \brief Returns the camera position.
  glm::vec3 getCameraPosition() { return _cameraPosition; };

  /// \brief Sets where the camera is looking.
  void setLookAtPosition(const glm::vec3 lookAtPosition) {
    _lookAtPosition = lookAtPosition;
  };

  /// \brief Increments the location where the camera is looking.
  void addToLookAtPosition(const glm::vec3 stepVec) {
    _lookAtPosition += stepVec;
  }

  /// \brief Returns the position where the camera is looking.
  glm::vec3 getLookAtPosition() { return _lookAtPosition; };

  /// \brief Rotates the camera location around the lookat point.
  void addToCameraViewAngle(const float horizAngle, const float vertAngle);

  /// \brief Set the field of view.  In radians.
  void setFOV(float fov) { _fov = fov; };

  /// \brief Sets the aspect ratio of the view window.
  void setAspect(float aspect) { _aspect = aspect; };

  /// \brief Add an object to your scene.
  ///
  /// This is how you build a scene.  Objects can be either
  /// drawableCompound objects, or a drawableCollection object.  A
  /// compound object is made up of multiple components, but is
  /// treated as a single object.  These are the leaves of your scene
  /// graph.  A drawableCollection object is a collection of other
  /// drawable objects, which can themselves be collections of other
  /// objects.  See the treeDemo2 program for an example.
  void addObject(const bsgPtr<drawableMulti> &pMultiObject) {
    _sceneRoot.addObject(pMultiObject->getName(), pMultiObject);
  }

  /// \brief Add an object to your scene with a specific name.
  ///
  /// This is the same as the other addObject() method, but allows you
  /// to specify a name for the object.
  void addObject(const std::string name,
                 const bsgPtr<drawableMulti> &pMultiObject) {
    _sceneRoot.addObject(name, pMultiObject);
  }

  /// \brief Prepare the scene to be drawn.
  ///
  /// This does a bunch of one-time-only initializations for the
  /// member drawable elements.
  void prepare();

  /// \brief Generates a projection matrix.
  ///
  /// From the field of view and clip planes.  For use in desktop and
  /// other non-VR applications.  In a VR application, this will be
  /// provided by the VR apparatus.
  glm::mat4 getProjMatrix();

  /// \brief Generates a view matrix.
  ///
  /// Uses the internal lookat position and camera position to
  /// generate a view matrix.  For use in desktop and other non-VR
  /// applications.  In a VR application, this will be provided by the
  /// VR apparatus.
  glm::mat4 getViewMatrix();

  /// \brief Retrieve an object by name.
  ///
  /// You'll be getting something that might be a drawableCompound and
  /// might be a drawableCollection.  That is, it might be a leaf
  /// node, and might be a branch.
  ///
  /// If the name does not match anything in the scene, this will
  /// return a NULL pointer, so you should check the return before
  /// using it.
  bsgPtr<drawableMulti> getObject(const std::string &name);

  /// \brief Retrieve an object by a list of names.
  ///
  /// A hierarchy of objects requires a hierarchy of names.  Consider
  /// the following tree:
  ///
  /// \code
  /// sceneRoot
  /// | assorted
  /// | | group3
  /// | | | rectJk21Lk
  /// | | | small2
  /// | | medium
  /// | axes3fdP1X
  /// | group1
  /// | | big
  /// | | small
  /// \endcode
  ///
  /// You can access the 'small2' object with a name like this:
  /// ["assorted", "group3", "small2"].
  ///
  /// If the name does not match anything in the scene, this will
  /// return a NULL pointer, so you should check the return before
  /// using it.
  bsgPtr<drawableMulti> getObject(bsgName &name);

  /// \brief Retrieve an object name identified by a selected point.
  bsgNameList insideBoundingBox(const glm::vec4 &testPoint);

  /// \brief Loads all the compound elements.
  void load();

  /// \brief Generates a view matrix and draws all the compound elements.
  ///
  /// The view matrix generated here, like the projection matrix
  /// generated in load(), is for use when running on a desktop.  In a
  /// VR environment, the view and projection matrices are provided.
  void draw();


  /// \brief Draws using the given matrices.
  ///
  /// Use this method to execute a render using the given
  /// transformation matrices.
  void draw(const glm::mat4 &viewMatrix,
            const glm::mat4 &projMatrix);


};


}

#endif //BSGHEADER
