#include "bsg.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace bsg {

class drawableObjModel : public drawableCompound {

private:
  const std::string &_fileName;
  drawableObj _frontFace, _backFace;

  // Do we *want* to see the interior?  Set this to false to show only
  // the object exterior, a simple optimization for big models.
  bool _includeBackFace;

  std::vector<std::string> split(const std::string line, const char separator);

  // So we can have two different constructors.
  void _processObjFile();
  
public:
  drawableObjModel(bsgPtr<shaderMgr> pShader, const std::string &fileName);
  drawableObjModel(bsgPtr<shaderMgr> pShader,
                   const std::string &fileName,
                   const bool &back);

};

class material {
 private:
  std::string _name;

 public:
  glm::vec3 colorAmbient, colorDiffuse, colorSpecular;

  float opacity, exponentSpecular;

  GLuint textureIDAmbient, textureIDDiffuse, textureIDSpecular;

 material(const std::string name) :
  _name(name),
    colorAmbient(glm::vec3(1.0f, 1.0f, 1.0f)),
    colorDiffuse(glm::vec3(1.0f, 1.0f, 1.0f)),
    colorSpecular(glm::vec3(1.0f, 1.0f, 1.0f)),
    opacity(1.0f),
    exponentSpecular(0.0f),
    textureIDAmbient(0),
    textureIDDiffuse(0),
    textureIDSpecular(0) {};    
  
};


}
