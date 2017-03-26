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

  std::vector<std::string> split(const std::string line, const char separator);

public:
  drawableObjModel(bsgPtr<shaderMgr> pShader, const std::string &fileName);
};
}
