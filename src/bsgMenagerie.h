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


class drawableSphere : public drawableCompound {
 private:

  float _phi, _theta;

  bsgPtr<drawableObj> _sphere;

 public:
  drawableSphere(bsgPtr<shaderMgr> pShader,
                    const int &phi, const int &theta);

};

class drawableCone : public drawableCompound {
 private:
  
  float _height, _theta;

  bsgPtr<drawableObj> _cap;
  bsgPtr<drawableObj> _base;

  void makeCone(bsgPtr<drawableObj> cone, int heightTesselation, int thetaTesselation, float radius, float height, glm::vec4 color);

 public:
   drawableCone(bsgPtr<shaderMgr> pShader, const int &heightTesselation, const int &thetaTesselation);
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

/// \brief A saggy line connecting two points.
///
/// A multiple-segment line with two colors that is longer than
/// necessary.  Calculates a sag as if there is gravity in the
/// negative Y direction.
class drawableSaggyLine : public drawableCompound {

 private:
  bsgPtr<drawableObj> _line;
  glm::vec3 _start, _end;
  float _sagFactor;
  int _nSegments;

  std::vector<glm::vec4> _calculateCatenary(const glm::vec3 &start,
                                            const glm::vec3 &end,
                                            const int &nSegments,
                                            const float &sagFactor);

 public:
  drawableSaggyLine(bsgPtr<shaderMgr> pShader,
                    const glm::vec3 &start, const glm::vec3 &end,
                    const glm::vec4 &startColor, const glm::vec4 &endColor,
                    const int &nSegments, const float &sagFactor);

  void setLineEnds(const glm::vec3 &start, const glm::vec3 &end);
};



 
}
