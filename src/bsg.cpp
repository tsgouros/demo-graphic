#include "bsg.h"

namespace bsg {

void shaderUtils::printMat(const std::string& name, const glm::mat4& mat) {

  std::cout << name << std::endl;  
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      printf("%8.4f ", mat[j][i]);
    }
    std::cout << std::endl;
  }
}
  
void lightList::load(GLuint programID) {

  // Get a handle for our lighting uniforms.  We are not binding the
  // attribute to a known location, just asking politely for it.  Note
  // that what is going on here is that OpenGL is actually matching
  // the _lightPositionName string to a variable in the shader.
  glUseProgram(programID);
  _lightPositions.ID = glGetUniformLocation(programID, _lightPositions.name.c_str());
  _lightColors.ID = glGetUniformLocation(programID, _lightColors.name.c_str());

}

// Update any changes to the light's position and color.
void lightList::draw(GLuint programID) {

  glUseProgram(programID);
  glUniform3fv(_lightPositions.ID, 2, &_lightPositions.data[0].x);
  glUniform3fv(_lightColors.ID, 2, &_lightColors.data[0].x);
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

  // Edit the shader source to reflect the input number of lights.  If
  // there is no 'XX' in the shader code, this will cause an ugly
  // error.
  char numLightsAsString[5];
  sprintf(numLightsAsString, "%d", _lightList->getNumLights());
  try {
    _shaderText[type].replace(_shaderText[type].find("XX"), 2, numLightsAsString);
  } catch (...) {
    std::cerr << "Caution: Shader does not care about number of lights." << std::endl;
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
    std::cerr << "** Vertex compile error: " << errorLog << std::endl;
    //std::cerr << _shaderText[GLSHADER_VERTEX] << std::endl;
  }
  
  
  glCompileShader(_shaderIDs[GLSHADER_FRAGMENT]);
  errorLog = _getShaderInfoLog(_shaderIDs[GLSHADER_FRAGMENT]);
  if (errorLog.size() > 1)
    std::cerr << "** Fragment compile error: " << errorLog << std::endl;

  if (geom) {
    glCompileShader(_shaderIDs[GLSHADER_GEOMETRY]);
    errorLog = _getShaderInfoLog(_shaderIDs[GLSHADER_GEOMETRY]);
    if (errorLog.size() > 1)
      std::cerr << "** Geometry compile error: " << errorLog << std::endl;
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
      std::cerr << "** Shader link error: " << errorLog << std::endl;
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

void shaderMgr::addLights(const shaderPtr<lightList> lightList) {
    if (_compiled) {
      throw std::runtime_error("Must load lights before compiling shader.");
    } else {
      _lightList = lightList;
    }
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
}


  
void drawableObj::prepare() {

  // Specify the OpenGL program to use for the following questions.
  _pShader->useProgram();
  
  // Figure out which buffers we need and get IDs for them.
  std::cout << "preparing a vertex buffer" << std::endl;
  glGenBuffers(1, &_vertices.bufferID);  
  _vertices.ID = _pShader->getAttribID(_vertices.name);
  
  if (!_colors.data.empty()) {
    std::cout << "preparing a color buffer" << std::endl;
    glGenBuffers(1, &_colors.bufferID);
    _colors.ID = _pShader->getAttribID(_colors.name);
  }
  if (!_normals.data.empty()) {
    glGenBuffers(1, &_normals.bufferID);
    _normals.ID = _pShader->getAttribID(_normals.name);
  }
  if (!_uvs.data.empty()) {
    glGenBuffers(1, &_uvs.bufferID);
    _uvs.ID = _pShader->getAttribID(_uvs.name);
  }

  // Put the data in its buffers, for practice.
  load();

}

void drawableObj::load() {

  glBindBuffer(GL_ARRAY_BUFFER, _vertices.bufferID);
  glBufferData(GL_ARRAY_BUFFER, _vertices.size(), &_vertices.data[0], GL_STATIC_DRAW);

  if (!_colors.data.empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, _colors.bufferID);
    glBufferData(GL_ARRAY_BUFFER, _colors.size(), &_colors.data[0], GL_STATIC_DRAW);
  }
  if (!_normals.data.empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, _normals.bufferID);
    glBufferData(GL_ARRAY_BUFFER, _normals.size(), &_normals.data[0], GL_STATIC_DRAW);
  }
  if (!_uvs.data.empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, _uvs.bufferID);
    glBufferData(GL_ARRAY_BUFFER, _uvs.size(), &_uvs.data[0], GL_STATIC_DRAW);
  }
}

void drawableObj::draw() {

  glBindBuffer(GL_ARRAY_BUFFER, _vertices.bufferID);
  glEnableVertexAttribArray(_vertices.ID);
  glVertexAttribPointer(_vertices.ID, _vertices.intSize(), GL_FLOAT, 0, 0, 0);

  if (!_colors.data.empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, _colors.bufferID);
    glEnableVertexAttribArray(_colors.ID);
    glVertexAttribPointer(_colors.ID, _colors.intSize(), GL_FLOAT, 0, 0, 0);
  }
  if (!_normals.data.empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, _normals.bufferID);
    glEnableVertexAttribArray(_normals.ID);
    glVertexAttribPointer(_normals.ID, _normals.intSize(), GL_FLOAT, 0, 0, 0);
  }
  if (!_uvs.data.empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, _uvs.bufferID);
    glEnableVertexAttribArray(_uvs.ID);
    glVertexAttribPointer(_uvs.ID, _uvs.intSize(), GL_FLOAT, 0, 0, 0);
  }

  glDrawArrays(_drawType, 0, _count);
}

glm::mat4 drawableCompound::getModelMatrix() {

  if (_modelMatrixNeedsReset) {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), _position);
    glm::mat4 rotationMatrix = glm::mat4_cast(_orientation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), _scale);

    _modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    _modelMatrixNeedsReset = false;

    // shaderUtils::printMat("trans:", translationMatrix);
    // shaderUtils::printMat("rotat:", rotationMatrix);
    // shaderUtils::printMat("scale:", scaleMatrix);
    // shaderUtils::printMat("model:", _modelMatrix);
  }

  return _modelMatrix;
}

void drawableCompound::prepare() {

  _pShader->useProgram();

  _modelMatrixID = _pShader->getUniformID(_modelMatrixName);
  _viewMatrixID = _pShader->getUniformID(_viewMatrixName);
  _projMatrixID = _pShader->getUniformID(_projMatrixName);

  // Prepare each component object.
  for (std::list<drawableObj>::iterator it = _objects.begin();
       it != _objects.end(); it++) {
    it->prepare();
  }
}
  
void drawableCompound::load() {

  _pShader->useProgram();

  // Load the model matrix, which is the same for however many times
  // this scene is rendered.
  glUniformMatrix4fv(_modelMatrixID, 1, false, &(getModelMatrix())[0][0]);

  // Load each component object.
  for (std::list<drawableObj>::iterator it = _objects.begin();
       it != _objects.end(); it++) {
    it->load();
  }
}

void drawableCompound::draw(const glm::mat4& viewMatrix,
                            const glm::mat4& projMatrix) {

  _pShader->useProgram();
  
  glUniformMatrix4fv(_viewMatrixID, 1, false, &viewMatrix[0][0]);
  glUniformMatrix4fv(_projMatrixID, 1, false, &projMatrix[0][0]);

  for (std::list<drawableObj>::iterator it = _objects.begin();
       it != _objects.end(); it++) {
    it->draw();
  }  
}


void scene::prepare() {

  for (compoundList::iterator it =  _compoundObjects.begin();
       it != _compoundObjects.end(); it++) {
    (*it)->prepare();
  }
}

void scene::load() {

  // TBD: Something's not right about this arrangement, since the load
  // step will load a different model matrix for each compound object
  // into the same place.

  
  // Update the projection matrix.  In case of a stereo display, both
  // eyes will use the same projection matrix.
  _projMatrix = glm::perspective(_fov, _aspect, _nearClip, _farClip);

  for (compoundList::iterator it =  _compoundObjects.begin();
       it != _compoundObjects.end(); it++) {
    (*it)->load();
  }
}

void scene::draw() {

  // Update the view matrix.
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  _viewMatrix = glm::lookAt(_cameraPosition, _lookAtPosition, up);

  // Then draw all the objects.
  for (compoundList::iterator it =  _compoundObjects.begin();
       it != _compoundObjects.end(); it++) {
    (*it)->draw(_viewMatrix, _projMatrix);
  }
}



  
}
