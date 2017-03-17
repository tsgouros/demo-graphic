#include "bsg.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm> 

namespace bsg {
  
class drawableObjModel : public drawableCompound {
 private:

  
  const std::string& _fileName;

  drawableObj _frontFace, _backFace;

 public:
  drawableObjModel(bsgPtr<shaderMgr> pShader, const std::string& fileName);

};

}
