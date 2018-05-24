#include "bsg.h"
#include "../external/freetype-gl/freetype-gl.h"

// Stb Image library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <time.h>
#include <stdlib.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace bsg {

void bsgUtils::printMat(const std::string& name, const glm::mat4& mat) {

  std::cout << name << std::endl;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      printf("%8.4f ", mat[j][i]);
    }
    std::cout << std::endl;
  }
}

// Get a handle for our lighting uniforms.  We are not binding the
// attribute to a known location, just asking politely for it.  Note
// that what is going on here is that OpenGL is actually matching
// the _lightPositionName string to a variable in the shader.
//
// This must be preceded by a glUseProgram(programID) call.
void lightList::load(const GLint programID) {

  // If there aren't any lights, don't bother.
  if (_lightPositions.size() > 0) {

    _lightPositions.ID = glGetUniformLocation(programID,
                                              _lightPositions.name.c_str());
    _lightColors.ID = glGetUniformLocation(programID,
                                           _lightColors.name.c_str());
  }
}

// Update any changes to the light's position and color.  This must be
// preceded by a glUseProgram(programID) call.
void lightList::draw() {

  // If there aren't any lights, don't bother.
  if (_lightPositions.size() > 0) {
    glUniform4fv(_lightPositions.ID,
                 _lightPositions.size(),
                 &_lightPositions.getData()[0].x);
    glUniform4fv(_lightColors.ID,
                 _lightColors.size(),
                 &_lightColors.getData()[0].x);
  }
}

void textureMgr::readFile(const textureType& type, const std::string& fileName) {

  switch(type) {
  case textureDDS:
    throw std::runtime_error("still working on DDS, try PNG");
    break;

  case textureBMP:
    throw std::runtime_error("still working on BMP, try PNG");
    break;

  case texturePNG:
    _textureBufferID = _loadPNG(fileName);
    break;

  case textureCHK:
    _textureBufferID = _loadCheckerBoard (64, 8);
    break;

  case textureTTF:
    throw std::runtime_error("Perhaps you meant to use a fontTextureMgr?");

  default:
    throw std::runtime_error("What texture type is this?");
  }
}

fontTextureMgr::fontTextureMgr(): textureMgr() {
  // Texture atlas stores individual glyphs. The point of the atlas is to pack
  // as many glyphs as possible into a small space. See this blog post 
  // http://wdobbie.com/post/gpu-text-rendering-with-vector-textures/
  // if you want to visualize what this is doing.
  // I chose this size atlas somewhat arbitrarily. This number is big enough
  // to make a texture that doesn't look pixellated. But if you bump up the
  // font size (below), you will likely need to bump up the size of the
  // atlas, as well.
  _width = 2048;
  _height = 2048;
  _atlas = texture_atlas_new(_width, _height, 1);
}

void fontTextureMgr::readFile(const textureType& type, const std::string& fileName) {
  switch (type) {
  case textureTTF:
    _textureBufferID = _loadTTF(fileName);
    break;
  default:
    throw std::runtime_error("What texture type is this?");
  }
}

// If this texture manager is a font, we need to be able to return a
// texture_font_t object so that somebody can access crucial info
// such as the location of a glyph in the text atlas (the texture), and how much
// kerning it should get, for the specific font in question (indicated by
// fileName). If this texture manager doesn't have that particular font loaded
// alread, it returns null to let the user know they should load it in.
texture_font_t *fontTextureMgr::getFont(const std::string &fileName) {
  return _fontsMap[fileName];
}

GLuint fontTextureMgr::_loadTTF(const std::string ttfPath) {
  // std::cout << "loading ttf " << ttfPath << std::endl;

  // Build a new texture font from its description and size. This texture_font_t
  // object will be in charge of building bitmap glyphs, putting them in the
  // atlas, and storing information about their coordinates in the atlas
  // and how much kerning each individual glyph gets, etc.
  // Same as above, I chose a font size arbitrarily, just using one that seemed
  // to work and allow us to avoid pixellation.
  // #TODO: eventually in the future, it would be ideal to choose both the font
  // size and the atlas size intelligently, based on the size of the text the
  // user of this interface requested. For now, we don't handle that yet.
  // On the other hand, however, note that we are planning to allow users to
  // reuse font atlases since they can fit multiple fonts. Does this change the
  // design choices we will make around this?
  texture_font_t *font = texture_font_new_from_file(_atlas, 200, ttfPath.c_str());
  _fontsMap[ttfPath] = font;

  // Cache some glyphs to speed things up.
  texture_font_load_glyphs(font,
                          (std::string(" !\"#$%&'()*+,-./0123456789:;<=>?") +
                          std::string("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_") +
                          std::string("`abcdefghijklmnopqrstuvwxyz{|}~")).c_str());

  // Generate one texture and store its ID in the texture variable. Then, since
  // OpenGL is a state machine, bind that texture so OpenGL knows to use it
  // until it's told otherwise.
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // These are some preferences we set, instructing OpenGL how to use the
  // currently bound texture. Setting WRAP to CLAMP is sort of like setting
  // CSS background-repeat: no-repeat. It means tex coords will be clamped to
  // one repetition of the texture, rather than allowing the texture to
  // repeat in a tiled way.
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  // And setting MIN_FILTER and MAG_FILTER to GL_NEAREST means that when
  // deciding what color to make a particular pixel in world space, based upon
  // the texture we are working with, it just chooses the nearest pixel
  // rather than using any kind of linear interpolation.
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  // Sets up how the texture image is defined in memory. We give it a width
  // and a height, and send it the data present in _atlas->data.
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, _atlas->width, _atlas->height,
                0, GL_RED, GL_UNSIGNED_BYTE, _atlas->data );
  return texture;
}

GLuint textureMgr::_loadCheckerBoard (const int size, int numFields) {
  const int boardSize = 64;
  _width = size;
  _height = size;
  int fieldWidth = size/numFields;

  GLubyte image[boardSize][boardSize][3];

  // Create a checkerboard pattern
  for ( int i = 0; i < size; i++ ) {
    for ( int j = 0; j < size; j++ ) {
      GLubyte c = 31;
      if ((i/fieldWidth)%2 == (j/fieldWidth)%2) {c = 255;}
        image[i][j][0]  = c;
        image[i][j][1]  = c;
        image[i][j][2]  = c;
      }
    }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height,
               0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  return texture;
}


GLuint textureMgr::_loadPNG(const std::string imagePath) {

  // This function was originally written by David Grayson for
  // https://github.com/DavidEGrayson/ahrs-visualizer



  /* load an image file directly as a new OpenGL texture */
  /*
  png_byte header[8];

  FILE *fp = fopen(imagePath.c_str(), "rb");
  if (fp == 0) {
    perror(imagePath.c_str());
    return 0;
  }

  // read the header
  fread(header, 1, 8, fp);

  if (png_sig_cmp(header, 0, 8)) {
    fprintf(stderr, "error: %s is not a PNG.\n", imagePath.c_str());
    fclose(fp);
    return 0;
  }

  png_structp png_ptr =
    png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    fprintf(stderr, "error: png_create_read_struct returned 0.\n");
    fclose(fp);
    return 0;
  }

  // create png info struct
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    fprintf(stderr, "error: png_create_info_struct returned 0.\n");
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    fclose(fp);
    return 0;
  }

  // create png info struct
  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info) {
    fprintf(stderr, "error: png_create_info_struct returned 0.\n");
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
    fclose(fp);
    return 0;
  }

  // the code in this if statement gets called if libpng encounters an error
  if (setjmp(png_jmpbuf(png_ptr))) {
    fprintf(stderr, "error from libpng\n");
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  // init png reading
  png_init_io(png_ptr, fp);

  // let libpng know you already read the first 8 bytes
  png_set_sig_bytes(png_ptr, 8);

  // read all the info up to the image data
  png_read_info(png_ptr, info_ptr);

  // variables to pass to get info
  int bit_depth, color_type;
  png_uint_32 temp_width, temp_height;

  // get info about png
  png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height,
               &bit_depth, &color_type,
               NULL, NULL, NULL);

  _width = temp_width;
  _height = temp_height;

  //printf("%s: %lux%lu %d\n", imagePath, temp_width, temp_height, color_type);

  if (bit_depth != 8) {
    fprintf(stderr, "%s: Unsupported bit depth %d.  Must be 8.\n", imagePath.c_str(), bit_depth);
    return 0;
  }

  GLint format;
  switch(color_type) {
  case PNG_COLOR_TYPE_RGB:
    format = GL_RGB;
    break;
  case PNG_COLOR_TYPE_RGB_ALPHA:
    format = GL_RGBA;
    break;
  default:
    fprintf(stderr, "%s: Unknown libpng color type %d.\n", imagePath.c_str(), color_type);
    return 0;
  }

  // Update the png info struct.
  png_read_update_info(png_ptr, info_ptr);

  // Row size in bytes.
  int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  // glTexImage2d requires rows to be 4-byte aligned
  rowbytes += 3 - ((rowbytes-1) % 4);

  // Allocate the image_data as a big block, to be given to opengl
  png_byte * image_data = (png_byte *)malloc(rowbytes * temp_height * sizeof(png_byte)+15);
  if (image_data == NULL) {
    fprintf(stderr, "error: could not allocate memory for PNG image data\n");
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  // row_pointers is for pointing to image_data for reading the png with libpng
  png_byte ** row_pointers = (png_byte **)malloc(temp_height * sizeof(png_byte *));
  if (row_pointers == NULL) {
    fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    fclose(fp);
    return 0;
  }

  // set the individual row_pointers to point at the correct offsets of image_data
  for (unsigned int i = 0; i < temp_height; i++) {
    row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
  }

  // read the png into image_data through row_pointers
  png_read_image(png_ptr, row_pointers);
  */

  // Neccessary as stb loads images upside down (as OpenGL sees it)
  stbi_set_flip_vertically_on_load(true);

  int width, height, components;
  unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &components, STBI_rgb_alpha);//STBI_default);
  if (!data) {
	  throw(stbi_failure_reason());
  }
  // Generate the OpenGL texture object
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  if (components == 3) {
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		  0, GL_RGB, GL_UNSIGNED_BYTE, data);
  } else if (components == 4) {
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		  0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  }
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  return texture;
}

void textureMgr::load(const GLuint programID) {

  // Get a handle for the texture uniform.
  _textureAttribID = glGetUniformLocation(programID,
                                          _textureAttribName.c_str());
}

void textureMgr::draw() {

  // Bind the texture in Texture Unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textureBufferID);

  // Set our "myTextureSampler" sampler to user Texture Unit 0
  glUniform1i(_textureAttribID, 0);

  // The data is actually loaded into the buffer in the loadXX() method.
}

std::string shaderMgr::_getShaderInfoLog(GLuint obj) {
  int infoLogLength = 0;
  int charsWritten  = 0;
  char *infoLog;
  GLint result = GL_FALSE;
  glGetShaderiv(obj, GL_COMPILE_STATUS, &result);
  glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);

  if (infoLogLength > 1) {
    infoLog = (char *)malloc(infoLogLength);
    glGetShaderInfoLog(obj, infoLogLength, &charsWritten, infoLog);
    return std::string(infoLog);
  } else {
    return std::string("");
  }
}

std::string shaderMgr::_getProgramInfoLog(GLuint obj) {

  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;
  glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

  if (infologLength > 1) {
    infoLog = (char *)malloc(infologLength);
    glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
    return std::string(infoLog);
  } else {
    return std::string("");
  }
}

void shaderMgr::addShader(const GLSHADERTYPE type,
                          const std::string& shaderFile) {

  // Read the shader code from the given file.
  std::ifstream shaderStream(shaderFile.c_str(), std::ios::in);
  if (shaderStream.is_open()) {
    std::string line = "";

    // Suck all the text into the appropriate _shaderText slot.
    while(getline(shaderStream, line)) _shaderText[type] += "\n" + line;

    // Close the stream.
    shaderStream.close();

  } else {
    throw std::runtime_error("Cannot open: " + shaderFile);
  }

  _shaderFiles[type] = shaderFile;

  if (_lightList->getNumLights() > 0) {
    // Edit the shader source to reflect the input number of lights.  If
    // there is no 'XX' in the shader code, this will cause an ugly
    // error we have to catch.
    char numLightsAsString[5];
    sprintf(numLightsAsString, "%d", _lightList->getNumLights());
    try {
      _shaderText[type].replace(_shaderText[type].find("XX"), 2,
                                numLightsAsString);
    } catch (...) {
      std::cerr << "Caution: Shader ("
                << shaderFile
                << ") does not care about number of lights." << std::endl;
    }
  } else {
    if (_shaderText[type].find("XX") != std::string::npos) {
      std::cerr << "Caution: Shader ("
                << shaderFile
                << ") is meant to use lights, and you have added no lights."
                << std::endl;
    }
  }
}

void shaderMgr::compileShaders() {

  // geom is true if there *is* a geometry shader in place.
  bool geom = (!_shaderText[GLSHADER_GEOMETRY].empty());

  _shaderIDs[GLSHADER_VERTEX] = glCreateShader(GL_VERTEX_SHADER);
  _shaderIDs[GLSHADER_FRAGMENT] = glCreateShader(GL_FRAGMENT_SHADER);
  if (geom) _shaderIDs[GLSHADER_GEOMETRY] = glCreateShader(GL_GEOMETRY_SHADER);

  // The OpenGL calls don't really like the modern C++ types, so we
  // convert back to old-fashioned char*.
  const char* vs = _shaderText[GLSHADER_VERTEX].c_str();
  const char* fs = _shaderText[GLSHADER_FRAGMENT].c_str();
  const char* gs;
  if (geom) gs = _shaderText[GLSHADER_GEOMETRY].c_str();

  // Feed the shader source to OpenGL.
  glShaderSource(_shaderIDs[GLSHADER_VERTEX], 1, &vs, NULL);
  glShaderSource(_shaderIDs[GLSHADER_FRAGMENT], 1, &fs, NULL);
  if (geom) glShaderSource(_shaderIDs[GLSHADER_GEOMETRY], 1, &gs, NULL);

  // Compile the shader source.  If there are any errors, print them.
  std::string errorLog;
  glCompileShader(_shaderIDs[GLSHADER_VERTEX]);
  errorLog = _getShaderInfoLog(_shaderIDs[GLSHADER_VERTEX]);
  if (errorLog.size() > 1) {
    std::cerr << "** Vertex compile error in "
              << _shaderFiles[GLSHADER_VERTEX]
              << std::endl << errorLog << std::endl;
    //std::cerr << _shaderText[GLSHADER_VERTEX] << std::endl;
  }


  glCompileShader(_shaderIDs[GLSHADER_FRAGMENT]);
  errorLog = _getShaderInfoLog(_shaderIDs[GLSHADER_FRAGMENT]);
  if (errorLog.size() > 1)
    std::cerr << "** Fragment compile error in "
              << _shaderFiles[GLSHADER_FRAGMENT]
              << std::endl << errorLog << std::endl;

  if (geom) {
    glCompileShader(_shaderIDs[GLSHADER_GEOMETRY]);
    errorLog = _getShaderInfoLog(_shaderIDs[GLSHADER_GEOMETRY]);
    if (errorLog.size() > 1)
      std::cerr << "** Geometry compile error in "
                << _shaderFiles[GLSHADER_GEOMETRY]
                << std::endl << errorLog << std::endl;
  }

  // Now create a program to contain our two (or three) shaders, and
  // attach the shaders to it.
  _programID = glCreateProgram();
  glAttachShader(_programID, _shaderIDs[GLSHADER_VERTEX]);
  glAttachShader(_programID, _shaderIDs[GLSHADER_FRAGMENT]);
  if (geom) glAttachShader(_programID, _shaderIDs[GLSHADER_GEOMETRY]);

  // Assemble the shaders into a single program with 'link', which
  // will make sure that the inputs to the fragment shader correspond
  // with outputs from the vertex shader, and so on.
  glLinkProgram(_programID);
  errorLog = _getProgramInfoLog(_programID);
  if (errorLog.size() > 1) {
    std::cerr << "** Shader link error in"
              << _shaderFiles[GLSHADER_VERTEX] << ", "
              << _shaderFiles[GLSHADER_FRAGMENT] << ", "
              << _shaderFiles[GLSHADER_GEOMETRY]
              << std::endl << errorLog << std::endl;
  }

  // The shaders are linked into the program, so we can delete the raw
  // shaders.
  glDeleteShader(_shaderIDs[GLSHADER_VERTEX]);
  glDeleteShader(_shaderIDs[GLSHADER_FRAGMENT]);
  if (geom) glDeleteShader(_shaderIDs[GLSHADER_GEOMETRY]);

  _compiled = true;
}

GLuint shaderMgr::getAttribID(const std::string& attribName) {
  glUseProgram(_programID);
  return glGetAttribLocation(_programID, attribName.c_str());
}

GLuint shaderMgr::getUniformID(const std::string& unifName) {
  glUseProgram(_programID);
  return glGetUniformLocation(_programID, unifName.c_str());
}

void shaderMgr::addLights(const bsgPtr<lightList> lightList) {
  if (_compiled) {
    throw std::runtime_error("Must load lights before compiling shader.");
  } else {
    _lightList = lightList;
  }
}

void shaderMgr::load() {
  _lightList->load(_programID);
  if (_textureLoaded) _texture->load(_programID);
}

void shaderMgr::draw() {
  _lightList->draw();
  if (_textureLoaded) _texture->draw();
}

void drawableObj::addData(const GLDATATYPE type,
                          const std::string& name,
                          const std::vector<glm::vec4>& data) {

  switch(type) {
  case(GLDATA_VERTICES):
    _vertices = drawableObjData<glm::vec4>(name, data);
    break;
  case(GLDATA_COLORS):
    _colors = drawableObjData<glm::vec4>(name, data);
    break;
  case(GLDATA_NORMALS):
    _normals = drawableObjData<glm::vec4>(name, data);
    break;
  case(GLDATA_TEXCOORDS):
    throw std::runtime_error("Do not use vec4 for texture coordinates.");
    break;
  }
  _loadedIntoBuffer = false;
}

void drawableObj::addData(const GLDATATYPE type,
             const std::string& name,
             const std::vector<glm::vec2>& data) {

  switch(type) {
  case(GLDATA_TEXCOORDS):
    _uvs = drawableObjData<glm::vec2>(name, data);
    break;
  case(GLDATA_COLORS):
  case(GLDATA_NORMALS):
  case(GLDATA_VERTICES):
    throw std::runtime_error("Vec2 is only for texture coordinates.");
    break;
  }
  _loadedIntoBuffer = false;
}

void drawableObj::setRealColors(
  const std::vector<glm::vec4>& data) {
  _realColors = data;
  _loadedIntoBuffer = false;
}

std::vector<glm::vec4> drawableObj::getRealColors(){
  return _realColors;
}


void drawableObj::setFakeColors(
                          const std::vector<glm::vec4>& data) {

  
  _fakeColors = data;
  _loadedIntoBuffer = false;
}

std::vector<glm::vec4> drawableObj::getFakeColors(){
  return _fakeColors;
}


void drawableObj::setData(const GLDATATYPE type,
                          const std::vector<glm::vec4>& data) {

  switch(type) {
  case(GLDATA_VERTICES):
    _vertices.setData(data);
    break;
  case(GLDATA_COLORS):
    _colors.setData(data);
    break;
  case(GLDATA_NORMALS):
    _normals.setData(data);
    break;
  case(GLDATA_TEXCOORDS):
    throw std::runtime_error("Do not use vec4 for texture coordinates.");
    break;
  }
  _loadedIntoBuffer = false;
}

void drawableObj::setData(const GLDATATYPE type,
             const std::vector<glm::vec2>& data) {

  switch(type) {
  case(GLDATA_TEXCOORDS):
    _uvs.setData(data);
    break;
  case(GLDATA_COLORS):
  case(GLDATA_NORMALS):
  case(GLDATA_VERTICES):
    throw std::runtime_error("Vec2 is only for texture coordinates.");
    break;
  }
  _loadedIntoBuffer = false;
}

bool drawableObj::insideBoundingBox(const glm::vec4 &testPoint,
                                    const glm::mat4 &modelMatrix) {

  if (!_selectable) return false;

  glm::vec4 upper = modelMatrix * _vertexBoundingBoxUpper;
  glm::vec4 lower = modelMatrix * _vertexBoundingBoxLower;

  return
    (testPoint.x <= upper.x) &&
    (testPoint.x >= lower.x) &&
    (testPoint.y <= upper.y) &&
    (testPoint.y >= lower.y) &&
    (testPoint.z <= upper.z) &&
    (testPoint.z >= lower.z);
}

void drawableObj::_getAttribLocations(GLuint programID) {

  bool badID = false;

  _vertices.ID = glGetAttribLocation(programID, _vertices.name.c_str());

  // Check to make sure the ID awarded is sane.  If not, probably the
  // name does not match the name in the shader.
  if (_vertices.ID < 0) {
    std::cerr << "** Caution: Bad ID for vertices attribute '" << _vertices.name << "'" << std::endl;
    badID = true;
  }

  if (!_colors.empty()) {
    _colors.ID = glGetAttribLocation(programID, _colors.name.c_str());

    if (_colors.ID < 0) {
      std::cerr << "** Caution: Bad ID for colors attribute '" << _colors.name << "'" << std::endl;
      badID = true;
    }
  }
  if (!_normals.empty()) {
    _normals.ID = glGetAttribLocation(programID, _normals.name.c_str());

    if (_normals.ID < 0) {
      std::cerr << "** Caution: Bad ID for normals attribute '" << _normals.name << "'" << std::endl;
      badID = true;
    }
  }
  if (!_uvs.empty()) {
    _uvs.ID = glGetAttribLocation(programID, _uvs.name.c_str());

    if (_uvs.ID < 0) {
      std::cerr << "** Caution: Bad ID for texture attribute '" << _uvs.name << "'" << std::endl;
      badID = true;
    }
  }

  if (badID) {
    std::cerr << "This can be caused either by a spelling error, or by not using the" << std::endl << "attribute within the shader code." << std::endl;
  }
}

void drawableObj::findBoundingBox() {

  // Find the bounding box for this object.
  _vertexBoundingBoxLower = glm::vec4(1.0e35, 1.0e35, 1.0e35, 1.0f);
  _vertexBoundingBoxUpper = glm::vec4(-1.0e35, -1.0e35, -1.0e35, 1.0f);

  // Don't use a templated accessor to test the for loop (very slow).
  std::vector<glm::vec4> data = _vertices.getData();

  for (std::vector<glm::vec4>::iterator it = data.begin();
       it != data.end(); it++) {

    _vertexBoundingBoxUpper.x = fmax((*it).x, _vertexBoundingBoxUpper.x);
    _vertexBoundingBoxUpper.y = fmax((*it).y, _vertexBoundingBoxUpper.y);
    _vertexBoundingBoxUpper.z = fmax((*it).z, _vertexBoundingBoxUpper.z);

    _vertexBoundingBoxLower.x = fmin((*it).x, _vertexBoundingBoxLower.x);
    _vertexBoundingBoxLower.y = fmin((*it).y, _vertexBoundingBoxLower.y);
    _vertexBoundingBoxLower.z = fmin((*it).z, _vertexBoundingBoxLower.z);
  }

  // We don't want any zero-width bounding boxes.
  if (_vertexBoundingBoxUpper.x == _vertexBoundingBoxLower.x) {
    _vertexBoundingBoxUpper.x += _boundingBoxMin;
    _vertexBoundingBoxLower.x -= _boundingBoxMin;
  }
  if (_vertexBoundingBoxUpper.y == _vertexBoundingBoxLower.y) {
    _vertexBoundingBoxUpper.y += _boundingBoxMin;
    _vertexBoundingBoxLower.y -= _boundingBoxMin;
  }
  if (_vertexBoundingBoxUpper.z == _vertexBoundingBoxLower.z) {
    _vertexBoundingBoxUpper.z += _boundingBoxMin;
    _vertexBoundingBoxLower.z -= _boundingBoxMin;
  }

  _haveBoundingBox = true;
}

void drawableObj::prepare(GLuint programID) {

  if (!_haveBoundingBox) findBoundingBox();

  if (_interleaved) {
    _prepareInterleaved(programID);
  } else {
    _prepareSeparate(programID);
  }
}

void drawableObj::_prepareInterleaved(GLuint programID) {

  // Calculate the stride and offset for each vertex value.
  _stride = 3 * sizeof(float); // We're cheating here, assuming only x,y,z.

  if (!_colors.empty()) {
    _colorPos = _stride;  // The colors appear after the vertices.
    _stride += 3 * sizeof(float);  // The next value appears after that.
  }

  if (!_normals.empty()) {
    _normalPos = _stride;
    _stride += 3 * sizeof(float);
  }

  if (!_uvs.empty()) {
    _uvPos = _stride;
    _stride += 2 * sizeof(float);
  }
  // End of calculating all the stride values.

  // Prepare a data buffer for the interleaved data.
  glGenBuffers(1, &_interleavedData.bufferID);

  // Now interleave the data.
  for (int i = 0; i < _vertices.size(); i++) {

    // Load the x,y,z vertices.
    _interleavedData.addData(_vertices[i].x);
    _interleavedData.addData(_vertices[i].y);
    _interleavedData.addData(_vertices[i].z);

    // Now interleave the other vertex attributes, if any.
    if (!_colors.empty()) {
      _interleavedData.addData(_colors[i].r);
      _interleavedData.addData(_colors[i].g);
      _interleavedData.addData(_colors[i].b);
    }
    if (!_normals.empty()) {
      _interleavedData.addData(_normals[i].x);
      _interleavedData.addData(_normals[i].y);
      _interleavedData.addData(_normals[i].z);
    }
    if (!_uvs.empty()) {
      _interleavedData.addData(_uvs[i].s);
      _interleavedData.addData(_uvs[i].t);
    }
  }

  _getAttribLocations(programID);

  _loadInterleaved();
}

void drawableObj::_prepareSeparate(GLuint programID) {

  // Figure out which buffers we need and get IDs for them.
  glGenBuffers(1, &_vertices.bufferID);
  if (!_colors.empty()) glGenBuffers(1, &_colors.bufferID);
  if (!_normals.empty()) glGenBuffers(1, &_normals.bufferID);
  if (!_uvs.empty()) glGenBuffers(1, &_uvs.bufferID);

  _getAttribLocations(programID);

  // Put the data in its buffers, for practice.
  _loadSeparate();

}


void drawableObj::load() {

  // if (_loadedIntoBuffer)
  //   std::cout << "no need " << std::endl;
  // else
  //   std::cout << "load!!! " << std::endl;

  if (_interleaved) {
    _loadInterleaved();
  } else {
    _loadSeparate();
  }
}

void drawableObj::_loadInterleaved() {

  if (!_loadedIntoBuffer) {

    // Load it into a buffer.
    glBindBuffer(GL_ARRAY_BUFFER, _interleavedData.bufferID);
    glBufferData(GL_ARRAY_BUFFER, _interleavedData.byteSize(),
                 _interleavedData.beginAddress(), GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    _loadedIntoBuffer = true;
  }
}


void drawableObj::_loadSeparate() {

  if (!_loadedIntoBuffer) {
    glBindBuffer(GL_ARRAY_BUFFER, _vertices.bufferID);
    glBufferData(GL_ARRAY_BUFFER, _vertices.byteSize(), _vertices.beginAddress(),
                 GL_STATIC_DRAW);

    if (!_colors.empty()) {
      glBindBuffer(GL_ARRAY_BUFFER, _colors.bufferID);
      glBufferData(GL_ARRAY_BUFFER, _colors.byteSize(), _colors.beginAddress(),
                   GL_STATIC_DRAW);
    }
    if (!_normals.empty()) {
      glBindBuffer(GL_ARRAY_BUFFER, _normals.bufferID);
      glBufferData(GL_ARRAY_BUFFER, _normals.byteSize(), _normals.beginAddress(),
                   GL_STATIC_DRAW);
    }
    if (!_uvs.empty()) {
      glBindBuffer(GL_ARRAY_BUFFER, _uvs.bufferID);
      glBufferData(GL_ARRAY_BUFFER, _uvs.byteSize(), _uvs.beginAddress(),
                   GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    _loadedIntoBuffer = true;
  }
}


void drawableObj::draw() {

  // Enable all the attribute arrays we'll use.
  glEnableVertexAttribArray(_vertices.ID);
  if (!_colors.empty()) glEnableVertexAttribArray(_colors.ID);
  if (!_normals.empty()) glEnableVertexAttribArray(_normals.ID);
  if (!_uvs.empty()) glEnableVertexAttribArray(_uvs.ID);

  if (_interleaved) {
    _drawInterleaved();
  } else {
    _drawSeparate();
  }

  // Now disable the attribute arrays so they won't interfere with the
  // next draw.
  glDisableVertexAttribArray(_vertices.ID);
  if (!_colors.empty()) glDisableVertexAttribArray(_colors.ID);
  if (!_normals.empty()) glDisableVertexAttribArray(_normals.ID);
  if (!_uvs.empty()) glDisableVertexAttribArray(_uvs.ID);
}

void drawableObj::_drawInterleaved() {

  glBindBuffer(GL_ARRAY_BUFFER, _interleavedData.bufferID);

  // Since the point of the interleaving is to make the transfer of
  // data more efficient, we are cheating in the following, and
  // leaving out the w from the vec4 data and the a from rgba.  These
  // are restored with default values by OpenGL.
  glVertexAttribPointer(_vertices.ID, 3,//_vertices.componentsPerVertex() - 1,
                        GL_FLOAT, GL_FALSE, _stride, BUFFER_OFFSET(0));

  if (!_colors.empty()) {
    glVertexAttribPointer(_colors.ID, 3,//_colors.componentsPerVertex() - 1,
                            GL_FLOAT, GL_FALSE, _stride, BUFFER_OFFSET(_colorPos));
  }
  if (!_normals.empty()) {
    glVertexAttribPointer(_normals.ID, 3,//_normals.componentsPerVertex() - 1,
                            GL_FLOAT, GL_FALSE, _stride, BUFFER_OFFSET(_normalPos));
  }
  if (!_uvs.empty()) {
    glVertexAttribPointer(_uvs.ID, 2,//_uvs.componentsPerVertex(),
                            GL_FLOAT, GL_FALSE, _stride, BUFFER_OFFSET(_uvPos));
  }

  glDrawArrays(_drawType, 0, _count);
}


void drawableObj::_drawSeparate() {

  glBindBuffer(GL_ARRAY_BUFFER, _vertices.bufferID);
  glVertexAttribPointer(_vertices.ID, _vertices.componentsPerVertex(),
                        GL_FLOAT, 0, 0, 0);

  if (!_colors.empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, _colors.bufferID);
    glVertexAttribPointer(_colors.ID, _colors.componentsPerVertex(),
                          GL_FLOAT, 0, 0, 0);
  }
  if (!_normals.empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, _normals.bufferID);
    glVertexAttribPointer(_normals.ID, _normals.componentsPerVertex(),
                          GL_FLOAT, 0, 0, 0);
  }
  if (!_uvs.empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, _uvs.bufferID);
    glVertexAttribPointer(_uvs.ID, _uvs.componentsPerVertex(),
                          GL_FLOAT, 0, 0, 0);
  }

  glDrawArrays(_drawType, 0, _count);

}

std::string bsgName::printName() const {
    std::string out;
    for (std::list<std::string>::const_iterator it = this->begin();
         it != this->end(); it++) {
      out += *it + "/";
    }
    // The trailing slash is just confusing, so leave it out.
    return out.substr(0, out.length() - 1);
  }


glm::mat4 drawableMulti::getModelMatrix() {

  if (_modelMatrixNeedsReset) {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), _position);
    glm::mat4 rotationMatrix = glm::mat4_cast(_orientation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), _scale);

    _modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    _modelMatrixNeedsReset = false;

    //    std::cout << glm::to_string(_modelMatrix) << std::endl;

    // bsgUtils::printMat("trans:", translationMatrix);
    // bsgUtils::printMat("rotat:", rotationMatrix);
    // bsgUtils::printMat("scale:", scaleMatrix);
    // bsgUtils::printMat("model:", _modelMatrix);
  }

  // If there is a parent, get the parent transformation (model)
  // matrix and use it with this one.
  if (_parent)
    return _parent->getModelMatrix() * _modelMatrix;
  else
    return _modelMatrix;
}

std::string drawableMulti::randomName(const std::string &nameRoot) {

  // This is a pretty dopey method, but it seems to work, so long as
  // the number of characters in each name is big enough.
  std::string out = nameRoot;
  for(int i = 0; i < 6; i++) {
    switch(rand()%3) {
    case 0:
      out += ('0' + rand()%10);
      break;
    case 1:
      out += ('A' + rand()%26);
      break;
    case 2:
      out += ('a' + rand()%26);
      break;
    }
  }
  return out;
}

bsgNameList drawableCompound::insideBoundingBox(const glm::vec4 &testPoint) {

  bsgName out;
  bsgNameList outList;
  glm::mat4 modelMatrix = getModelMatrix();

  for (DrawableObjList::iterator it = _objects.begin();
       it != _objects.end(); it++) {

    if ((*it)->insideBoundingBox(testPoint, modelMatrix)) {

      // If we're here, the point is in the bounding box of at least
      // one of the member objects of this compound object.  Create a
      // one-element list of a zero-element name.
      outList.push_back(out);
      return outList;
    }
  }

  // If we're here, the answer is no, so return an empty name.
  return outList;
}


void drawableCompound::prepare() {

  _pShader->useProgram();
  _pShader->prepare();

  _modelMatrixID = _pShader->getUniformID(_modelMatrixName);
  _normalMatrixID = _pShader->getUniformID(_normalMatrixName);
  _viewMatrixID = _pShader->getUniformID(_viewMatrixName);
  _projMatrixID = _pShader->getUniformID(_projMatrixName);

  // Prepare each component object.
  for (DrawableObjList::iterator it = _objects.begin();
       it != _objects.end(); it++) {
    (*it)->prepare(_pShader->getProgram());
  }
}

void drawableCompound::load() {

  _pShader->useProgram();
  _pShader->load();

  // Review the current state of the transformation matrices, and pack
  // them all into the total model matrix.
  _totalModelMatrix = getModelMatrix();

  // Load each component object.
  for (DrawableObjList::iterator it = _objects.begin();
       it != _objects.end(); it++) {
    (*it)->load();
  }
}

void drawableCompound::draw(const glm::mat4& viewMatrix,
                            const glm::mat4& projMatrix) {

  _pShader->useProgram();
  _pShader->draw();

  // Load the model matrix.  This adjusts the position of each object.
  // Remember that all the objects in a compound object use the same
  // shader and the same model matrix.
  glUniformMatrix4fv(_modelMatrixID, 1, false, &_totalModelMatrix[0][0]);

  // Calculate the normal matrix to use for lighting.
  _normalMatrix = glm::transpose(glm::inverse(viewMatrix * _totalModelMatrix));
  glUniformMatrix4fv(_normalMatrixID, 1, false, &_normalMatrix[0][0]);

  // The view and projection matrices come from the scene object, above us.
  glUniformMatrix4fv(_viewMatrixID, 1, false, &viewMatrix[0][0]);
  glUniformMatrix4fv(_projMatrixID, 1, false, &projMatrix[0][0]);

  // std::cout << "view" << glm::to_string(viewMatrix) << std::endl;
  // std::cout << "normal" << glm::to_string(_normalMatrix) << std::endl;
  // std::cout << "model" << glm::to_string(_modelMatrix) << std::endl;
  // std::cout << "proj" << glm::to_string(projMatrix) << std::endl;

  for (DrawableObjList::iterator it = _objects.begin();
       it != _objects.end(); it++) {
    (*it)->draw();
  }
}

void drawableCompound::addObjectBoundingBox(bsgPtr<drawableObj> &obj) {

  obj->findBoundingBox();

  bsgPtr<drawableObj> bb = new drawableObj();
  std::vector<glm::vec4> bbCorners(24);
  std::vector<glm::vec4> bbColors(24, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

  glm::vec4 bbLower = obj->getBoundingBoxLower();
  glm::vec4 bbUpper = obj->getBoundingBoxUpper();

  bbCorners[0] = glm::vec4(bbLower.x, bbLower.y, bbLower.z, 1.0);
  bbCorners[1] = glm::vec4(bbLower.x, bbLower.y, bbUpper.z, 1.0);

  bbCorners[2] = glm::vec4(bbLower.x, bbLower.y, bbUpper.z, 1.0);
  bbCorners[3] = glm::vec4(bbLower.x, bbUpper.y, bbUpper.z, 1.0);

  bbCorners[4] = glm::vec4(bbLower.x, bbLower.y, bbLower.z, 1.0);
  bbCorners[5] = glm::vec4(bbLower.x, bbUpper.y, bbLower.z, 1.0);

  bbCorners[6] = glm::vec4(bbLower.x, bbUpper.y, bbLower.z, 1.0);
  bbCorners[7] = glm::vec4(bbUpper.x, bbUpper.y, bbLower.z, 1.0);

  bbCorners[8] = glm::vec4(bbLower.x, bbLower.y, bbLower.z, 1.0);
  bbCorners[9] = glm::vec4(bbUpper.x, bbLower.y, bbLower.z, 1.0);

  bbCorners[10] = glm::vec4(bbUpper.x, bbLower.y, bbLower.z, 1.0);
  bbCorners[11] = glm::vec4(bbUpper.x, bbLower.y, bbUpper.z, 1.0);

  bbCorners[12] = glm::vec4(bbUpper.x, bbUpper.y, bbUpper.z, 1.0);
  bbCorners[13] = glm::vec4(bbLower.x, bbUpper.y, bbUpper.z, 1.0);

  bbCorners[14] = glm::vec4(bbLower.x, bbUpper.y, bbUpper.z, 1.0);
  bbCorners[15] = glm::vec4(bbLower.x, bbUpper.y, bbLower.z, 1.0);

  bbCorners[16] = glm::vec4(bbUpper.x, bbUpper.y, bbUpper.z, 1.0);
  bbCorners[17] = glm::vec4(bbUpper.x, bbLower.y, bbUpper.z, 1.0);

  bbCorners[18] = glm::vec4(bbUpper.x, bbLower.y, bbUpper.z, 1.0);
  bbCorners[19] = glm::vec4(bbLower.x, bbLower.y, bbUpper.z, 1.0);

  bbCorners[20] = glm::vec4(bbUpper.x, bbUpper.y, bbUpper.z, 1.0);
  bbCorners[21] = glm::vec4(bbUpper.x, bbUpper.y, bbLower.z, 1.0);

  bbCorners[22] = glm::vec4(bbUpper.x, bbUpper.y, bbLower.z, 1.0);
  bbCorners[23] = glm::vec4(bbUpper.x, bbLower.y, bbLower.z, 1.0);

  // This is a bit hackish; should query to use the same name as in
  // obj->_vertices, etc.
  bb->addData(GLDATA_VERTICES, "position", bbCorners);
  bb->addData(GLDATA_COLORS, "color", bbColors);
  bb->setDrawType(GL_LINES);

  // std::cout << bb << std::endl;

  // for (int i = 0; i < 24; i++) {
  //   std::cout << "corner:" << bbCorners[i].x << "," << bbCorners[i].y << "," << bbCorners[i].z << " color:" << bbColors[i].r << "," << bbColors[i].g << "," << bbColors[i].b << std::endl;
  // }

  addObject(bb);
}



drawableCollection::drawableCollection() {
  // Seed a random number generator to generate default names randomly.
  #ifdef WIN32
  srand(GetTickCount());
  #else
  srand(time(NULL));
  #endif
  _name = randomName("coll");
}

drawableCollection::drawableCollection (const std::string name) :
  drawableMulti(name) {
  // Seed a random number generator to generate default names randomly.
  #ifdef WIN32
  srand(GetTickCount());
  #else
  srand(time(NULL));
  #endif
}

std::string drawableCollection::addObject(const std::string name,
                                   const bsgPtr<drawableMulti> &pMultiObject) {
  pMultiObject->setParent(this);
  _collection[name] = pMultiObject;
  pMultiObject->setName(name);

  return name;
}

std::string drawableCollection::addObject(const bsgPtr<drawableMulti> &pMultiObject) {

  // Get the name of the input object.
  if (pMultiObject->getName().empty()) {

    // All objects should have a name, so this should not happen.
    return addObject(randomName("err"), pMultiObject);

  } else {
    // Is the name already used?
    if (_collection.find(pMultiObject->getName()) != _collection.end()) {

      std::cerr << "You have already used " << pMultiObject->getName()
                << " in " << getName()
                << ".  Assigning a random name." << std::endl;

      return addObject(randomName(pMultiObject->getName()), pMultiObject);

    } else {

      // Add the object with the given name.
      return addObject(pMultiObject->getName(), pMultiObject);
    }
  }
}

bsgPtr<drawableMulti> drawableCollection::delObject(const std::string &name) {

  CollectionMap::iterator it = _collection.find(name);

  if (it == _collection.end()) {
    return NULL;
  } else {
    bsgPtr<drawableMulti> out = it->second;
    _collection.erase(it);
    return out;
  }
}

bsgPtr<drawableMulti> drawableCollection::delObject(bsgName name) {

  if (name.size() > 0) {

    CollectionMap::iterator it = _collection.find(name.front());

    if (it == _collection.end()) {

      // No match.
      return NULL;

    } else {

      if (name.size() > 1) {

        // Step down a level.
        name.pop_front();
        return it->second->delObject(name);

      } else {

        bsgPtr<drawableMulti> out = it->second;
        _collection.erase(it);
        return out;
      }
    }
  } else {

    // This was called with an empty list for some reason.
    return NULL;
  }
}


bsgPtr<drawableMulti> drawableCollection::getObject(const std::string &name) {

  CollectionMap::iterator it = _collection.find(name);

  if (it == _collection.end()) {
    return NULL;
  } else {
    return it->second;
  }
}

bsgPtr<drawableMulti> drawableCollection::getObject(bsgName name) {

  if (name.size() > 1) {

    CollectionMap::iterator it = _collection.find(name.front());

    if (it == _collection.end()) {

      // No match.
      return NULL;

    } else {

      // Step down a level.
      name.pop_front();
      return it->second->getObject(name);

    }
  } else if (name.size() > 0) {

    return getObject(name.front());

  } else {

    // This was called with an empty list for some reason.
    return NULL;
  }
}

bsgNameList drawableCollection::getNames() {

  // Our output.
  bsgNameList out;

  // Loop through the collection, running getNames() on all the members.
  for (CollectionMap::iterator it = _collection.begin();
       it != _collection.end(); it++) {
    bsgNameList sublist = it->second->getNames();

    if (!sublist.empty()) {
      for (bsgNameList::iterator jt = sublist.begin();
           jt != sublist.end(); jt++) {
        jt->push_front(it->first);
      }
    } else {
      bsgName b;
      b.push_back(it->first);
      sublist.push_back(b);
    }

    // If any of them reply, add them to the output list.
    out.splice(out.end(), sublist);
  }

  return out;
}

bsgNameList drawableCollection::insideBoundingBox(const glm::vec4 &testPoint) {

  bsgNameList out;

  for (CollectionMap::iterator it = _collection.begin();
       it != _collection.end(); it++) {
    bsgNameList sublist = it->second->insideBoundingBox(testPoint);

    // Check if the list is empty.  Note that the list might have one
    // zero-length entry, if this child is a drawableCompound and the
    // condition is met.
    if (!sublist.empty()) {

      for (bsgNameList::iterator jt = sublist.begin();
           jt != sublist.end(); jt++) {
        jt->push_front(it->first);
      }
    }

    out.splice(out.end(), sublist);
  }

  // If we're here, the answer is no, so this should be an empty list.
  return out;
}

std::string drawableCollection::printObj (const std::string &prefix) const {

  std::string out = prefix + "<drawableCollection:" + _name + ">";

  for (CollectionMap::const_iterator it = _collection.begin();
       it != _collection.end(); it++) {

    out += "\n" + it->second->printObj(prefix + "| ");
  }

  return out;
}


void drawableCollection::prepare() {

  for (CollectionMap::iterator it =  _collection.begin();
       it != _collection.end(); it++) {
    it->second->prepare();
  }
}

void drawableCollection::load() {

  // Then draw all the objects.
  for (CollectionMap::iterator it =  _collection.begin();
       it != _collection.end(); it++) {
    it->second->load();
  }
}

void drawableCollection::draw(const glm::mat4 &viewMatrix,
                              const glm::mat4 &projMatrix) {

  // Then draw all the objects.
  for (CollectionMap::iterator it =  _collection.begin();
       it != _collection.end(); it++) {
    it->second->draw(viewMatrix, projMatrix);
  }
}


/// \brief Adjust camera position according to input Euler angles.
///
/// We use quaternions in the implementation because they provide a
/// smooth way to rotate continuously.  You'll notice an oddness in
/// the rotation in practice, because the up vector is always the same
/// direction.
void scene::addToCameraViewAngle(const float horizAngle, const float vertAngle) {

  // Calculate current direction vector.
  glm::vec3 dir = _lookAtPosition - _cameraPosition;

  // Calculate a rotation quaternion based on the input euler angles.
  glm::quat rot = glm::quat(glm::vec3(vertAngle, horizAngle, 0.0f));

  // Apply the quaternion to the direction vector.
  glm::vec4 newDir = glm::rotate(rot, glm::vec4(dir.x, dir.y, dir.z, 1.0f));

  // Apply the direction vector to the lookat position to get the new
  // camera location.
  _cameraPosition = _lookAtPosition - glm::vec3(newDir.x, newDir.y, newDir.z);
}

bsgPtr<drawableMulti> scene::getObject(const std::string &name) {

  if (name.compare("sceneRoot") == 0) {

    return NULL;

  } else {

    return _sceneRoot.getObject(name);
  }
}

bsgPtr<drawableMulti> scene::getObject(bsgName &name) {

  if (name.empty()) return NULL;

  bsgName localName = name;
  if (localName.front().compare("sceneRoot") == 0) localName.pop_front();

  return _sceneRoot.getObject(localName);

}

bsgNameList scene::insideBoundingBox(const glm::vec4 &testPoint) {

  return _sceneRoot.insideBoundingBox(testPoint);
}


void scene::prepare() {

  _sceneRoot.prepare();
}

glm::mat4 scene::getProjMatrix() {
  // Update the projection matrix.  In case of a stereo display, both
  // eyes will use the same projection matrix.
  return glm::perspective(_fov, _aspect, _nearClip, _farClip);
}

glm::mat4 scene::getViewMatrix() {
  // Update the view matrix.
  glm::vec3 dir = glm::normalize(_lookAtPosition - _cameraPosition);
  glm::vec3 right = glm::cross(dir, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::vec3 up = glm::normalize(glm::cross(right, dir));

  return glm::lookAt(_cameraPosition, _lookAtPosition, up);
}

void scene::load() {

  _sceneRoot.load();
}

void scene::draw(const glm::mat4 &viewMatrix,
                 const glm::mat4 &projMatrix) {

  _sceneRoot.draw(viewMatrix, projMatrix);
}

}
