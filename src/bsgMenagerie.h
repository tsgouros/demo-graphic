#include "bsg.h"

namespace bsg {
  
class drawableRectangle : public drawableCompound {
 private:

  float _width, _height;

  drawableObj _frontFace, _backFace;

 public:
  drawableRectangle(bsgPtr<shaderMgr> pShader,
                    const float width, const float height);
  drawableRectangle(bsgPtr<shaderMgr> pShader,
                    const float width, const float height,
                    const int nDivs);

};


/// \brief Some axes.
///
/// There are no normals and textures for this one, so don't use a
/// shader that requires them.
class drawableAxes : public drawableCompound {

 private:
   float _length;
   drawableObj _axes;

 public:
   drawableAxes(bsgPtr<shaderMgr> pShader, const float length);
};
 
}
