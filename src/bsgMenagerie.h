#include "bsg.h"

namespace bsg {

class drawableRectangle : public drawableCompound {
 private:

  float _width, _height;

  bsgPtr<drawableObj> _frontFace, _backFace;

 public:
  drawableRectangle(bsgPtr<shaderMgr> pShader,
                    const float &width, const float &height,
                    const glm::vec4 color = glm::vec4(0,0,0,0));
  drawableRectangle(bsgPtr<shaderMgr> pShader,
                    const float &width, const float &height,
                    const int &nDivs);

};

class drawableRectangleOutline : public drawableCompound {
 private:

  float _width, _height, _strokeWidth;
  bsgPtr<drawableObj> _frontFace, _backFace;

 public:
  drawableRectangleOutline(bsgPtr<shaderMgr> pShader,
                    const float &width, const float &height,
                    const float &strokeWidth,
                    const glm::vec4 color = glm::vec4(0,0,0,0));

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

/// \brief A line of text.
///
/// A single line of text in the size, color, and font you specify.
/// If you pass in no texture object, it sees the default (null) and creates
/// its own texture, which you can then retrieve using getFontTexture
/// and reuse for all other text in the scene.
/// If it is passed an already-initialized texture, it'll
/// either use that texture with the existing font, or add a new font to the
/// texture, but it's all stored within the same texture so as to save on
/// memory usage.

class drawableText : public drawableCompound {
  private:
    const char *_text;
    const float _height;
    const char *_fontFilePath;
    const glm::vec4 _color;
    bsgPtr<fontTextureMgr> _texture;

  public:
    drawableText(bsgPtr<shaderMgr> shader,
                 const char *text,
                 const float height,
                 const char *fontFilePath,
                 const glm::vec4 color,
                 bsgPtr<fontTextureMgr> texture=NULL);

    bsgPtr<fontTextureMgr> getFontTexture();
    void _write();
};

/// \brief A rectangle of text, like a button.
///
/// Situates text in the size, color, and font you specify on a box of the
/// dimensions, border color, and background color you specify. In order to
/// avoid z-fighting, the drawableText is placed a teeny bit in front of
/// the rectangle. The default is .001, but this is configurable using the
/// argument offsetDist.
/// Similar to drawableText, drawableTextRect also is capable of either
/// creating its own texture OR receiving an already-initialized texture and
/// simply using that. Again, you should pass it `null` the first time you
/// add text to a scene, but other times, you should retrieve the original texture
/// and reuse it to save on memory.

class drawableTextRect : public drawableCollection {
 private:
   const char *_text;
   const float _textHeight;
   const char *_fontFilePath;
   const float _boxHeight;
   const float _boxWidth;
   const float _borderWidth;
   const float _offsetDist;
   const glm::vec4 _textColor;
   const glm::vec4 _backgroundColor;
   const glm::vec4 _borderColor;
   bsgPtr<fontTextureMgr> _texture;

 public:
   drawableTextRect(bsgPtr<shaderMgr> textShader,
                   bsgPtr<shaderMgr> backgroundShader,
                   const char *text,
                   const char *fontFilePath,
                   bsgPtr<fontTextureMgr> texture=NULL,
                   const float textHeight=0.75,
                   const glm::vec4 textColor=glm::vec4(1.0, 1.0, 1.0, 1.0),
                      // default is white
                   const glm::vec4 backgroundColor=glm::vec4(0.0, 0.0, 0.0, 1.0),
                      // default is black
                   const glm::vec4 borderColor=glm::vec4(1.0, 1.0, 1.0, 1.0),
                      // default is white
                   const float boxHeight=1,
                   const float boxWidth=2,
                   const float borderWidth=0.1,
                   const float offsetDist=0.001);

   bsgPtr<fontTextureMgr> getFontTexture();
};

/// \brief A 3d box of text.
///
/// Situates text in the size, color, and font you specify within a 3s box of
/// the dimensions, border color, background color, and extrusion you specify.

class drawableTextBox : public drawableCollection {
 private:
   const char *_text;
   const float _textHeight;
   const char *_fontFilePath;
   const float _boxHeight;
   const float _boxWidth;
   const float _borderWidth;
   const float _extrusion;
   const float _offsetDist;
   const glm::vec4 _extrusionColor;
   const glm::vec4 _textColor;
   const glm::vec4 _backgroundColor;
   const glm::vec4 _borderColor;
   bsgPtr<fontTextureMgr> _texture;

 public:
   drawableTextBox(bsgPtr<shaderMgr> textShader,
                   bsgPtr<shaderMgr> backgroundShader,
                   const char *text,
                   const char *fontFilePath,
                   bsgPtr<fontTextureMgr> texture=NULL,
                   const float extrusion=0.5,
                   const float textHeight=0.75,
                   const glm::vec4 textColor=glm::vec4(1.0, 1.0, 1.0, 1.0),
                      // default is white
                   const glm::vec4 backgroundColor=glm::vec4(0.0, 0.0, 0.0, 1.0),
                      // default is black
                   const glm::vec4 borderColor=glm::vec4(1.0, 1.0, 1.0, 1.0),
                      // default is white
                   const float boxHeight=1,
                   const float boxWidth=2,
                   const float borderWidth=0.1,
                   const float offsetDist=0.001,
                   const glm::vec4 extrusionColor=glm::vec4(1.0, 1.0, 1.0, 1.0));

   bsgPtr<fontTextureMgr> getFontTexture();
};


}
