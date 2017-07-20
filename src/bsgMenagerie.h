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

class drawableSquare : public drawableCompound {
 private:

  int _tess;

  bsgPtr<drawableObj> _rect;

 public:
  drawableSquare(bsgPtr<shaderMgr> pShader, const int &tesselation, const glm::vec3 &topLeft, const glm::vec3 &topRight, const glm::vec3 &bottomLeft, const glm::vec4 &color);
  static void getRect(bsgPtr<drawableObj> rect, const int &tesselation, const glm::vec3 &topLeft, const glm::vec3 &topRight, const glm::vec3 &bottomLeft, const glm::vec4 &color);

};

class drawableCube : public drawableCompound {
 private:

  int _tess;

  bsgPtr<drawableObj> _front;
  bsgPtr<drawableObj> _back;
  bsgPtr<drawableObj> _top;
  bsgPtr<drawableObj> _bottom;
  bsgPtr<drawableObj> _left;
  bsgPtr<drawableObj> _right;

 public:
  drawableCube(bsgPtr<shaderMgr> pShader, const int &tesselation, const glm::vec4 &color);

};


class drawableSphere : public drawableCompound {
 private:

  float _phi, _theta;

  bsgPtr<drawableObj> _sphere;

 public:
  drawableSphere(bsgPtr<shaderMgr> pShader,
                    const int &phi, const int &theta, const glm::vec4 &color);

};

class drawableCone : public drawableCompound {
 private:

  float _height, _theta;

  bsgPtr<drawableObj> _cap;
  bsgPtr<drawableObj> _base;

  void makeCone(bsgPtr<drawableObj> cone, int heightTesselation, int thetaTesselation, float radius, float height, glm::vec4 color);

 public:
   drawableCone(bsgPtr<shaderMgr> pShader, const int &heightTesselation, const int &thetaTesselation, const glm::vec4 &color);
};

class drawableCircle : public drawableCompound {
 private:

  float _theta;

  bsgPtr<drawableObj> _circle;

 public:
  drawableCircle(bsgPtr<shaderMgr> pShader, const int &thetaTesselation, const float &normalDirection, const float &yPos);
  static void getCircle(bsgPtr<drawableObj> circle, const int &thetaTesselation, const float &normalDirection, const float &yPos, const glm::vec4 &color);

};

class drawableCylinder : public drawableCompound {
 private:

  float _height, _theta;

  bsgPtr<drawableObj> _base;
  bsgPtr<drawableObj> _body;
  bsgPtr<drawableObj> _top;

 public:
  drawableCylinder(bsgPtr<shaderMgr> pShader, const int &heightTesselation, const int &thetaTesselation, const glm::vec4 &color);
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

/// \brief A collection of points.
///
/// A collection of arbitrarily-positioned points in 3D space.
class drawablePoints : public drawableCompound {

 private:
  bsgPtr<drawableObj> _points;

 public:
  drawablePoints(bsgPtr<shaderMgr> pShader,
                 const std::vector<glm::vec4> &points,
                 const std::vector<glm::vec4> &colors);
};


}
