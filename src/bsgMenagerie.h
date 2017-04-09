#include "bsg.h"

namespace bsg {
  
class drawableRectangle : public drawableCompound {
 private:

  float _width, _height;

  bsgPtr<drawableObj> _frontFace, _backFace;

 public:
  drawableRectangle(bsgPtr<shaderMgr> pShader,
                    const float &width, const float &height);
  drawableRectangle(bsgPtr<shaderMgr> pShader,
                    const float &width, const float &height,
                    const int &nDivs);

};


/// \brief Some axes.
///
/// There are no normals and textures for this one, so don't use a
/// shader that requires them.
class drawableAxes : public drawableCompound {

 private:
   float _length;
   bsgPtr<drawableObj> _axes;

 public:
   drawableAxes(bsgPtr<shaderMgr> pShader, const float &length);
};
 


/// \brief A line connecting two points.
///
/// A straight line with a single color.
class drawableLine : public drawableCompound {

 private:
  bsgPtr<drawableObj> _line;

 public:
  drawableLine(bsgPtr<shaderMgr> pShader,
               const glm::vec3 &start, const glm::vec3 &end,
               const glm::vec4 &color);

  void setLineEnds(const glm::vec3 &start, const glm::vec3 &end);
};

}
