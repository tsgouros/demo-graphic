#include "bsg.h"

namespace bsg {
  
class drawableRectangle : public drawableCompound {
 private:

  float _width, _height;

  drawableObj _frontFace, _backFace;

 public:
  drawableRectangle(bsgPtr<shaderMgr> pShader,
                    const float width, const float height);

};

}
