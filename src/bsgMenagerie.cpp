#include "bsgMenagerie.h"
#include "../external/freetype-gl/freetype-gl.h"

namespace bsg {

  drawableRectangle::drawableRectangle(bsgPtr<shaderMgr> pShader,
                                       const float &width, const float &height,
                                       const int &nDivs) :
    drawableCompound(pShader), _height(height), _width(width) {

    _name = randomName("rect");

    float w = _width/2.0f;
    float h = _height/2.0f;
    float wdiv = _width / nDivs;
    float hdiv = _height / nDivs;
    int nEntries = 2 * (nDivs + 1);

    std::vector<glm::vec4> frontFaceVertices = std::vector<glm::vec4>(nEntries);
    std::vector<glm::vec4> frontFaceColors = std::vector<glm::vec4>(nEntries);
    std::vector<glm::vec4> frontFaceNormals = std::vector<glm::vec4>(nEntries);
    std::vector<glm::vec2> frontFaceUVs = std::vector<glm::vec2>(nEntries);
    std::vector<glm::vec4> backFaceVertices = std::vector<glm::vec4>(nEntries);
    std::vector<glm::vec4> backFaceColors = std::vector<glm::vec4>(nEntries);
    std::vector<glm::vec4> backFaceNormals = std::vector<glm::vec4>(nEntries);
    std::vector<glm::vec2> backFaceUVs = std::vector<glm::vec2>(nEntries);

    for (int j = 0; j < nDivs; j++) {

      _frontFace = new drawableObj();
      _backFace = new drawableObj();

      for (int i = 0; i <= nDivs; i++) {

        int k = 2 * i;
        frontFaceVertices[k] = glm::vec4(-w + (j * wdiv),
                                         -h + (i * hdiv), 0.0f, 1.0f);
        frontFaceVertices[k + 1] = glm::vec4(-w + ((j + 1) * wdiv),
                                             -h + (i * hdiv), 0.0f, 1.0f);
        frontFaceColors[k] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        frontFaceColors[k + 1] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        frontFaceNormals[k] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
        frontFaceNormals[k + 1] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
        frontFaceUVs[k] = glm::vec2(0.0 + (j * 1.0/nDivs),
                                    0.0 + (i * 1.0/nDivs));
        frontFaceUVs[k + 1] = glm::vec2(((j + 1) * 1.0/nDivs),
                                        0.0 + (i * 1.0/nDivs));

        backFaceVertices[k] = glm::vec4(-w + ((j + 1) * wdiv),
                                        -h + (i * hdiv), 0.0f, 1.0f);
        backFaceVertices[k + 1] = glm::vec4(-w + (j * wdiv),
                                            -h + (i * hdiv), 0.0f, 1.0f);
        backFaceColors[k] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        backFaceColors[k + 1] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        backFaceNormals[k] = glm::vec4(0.0f, 0.0f,-1.0f, 0.0f);
        backFaceNormals[k + 1] = glm::vec4(0.0f, 0.0f,-1.0f, 0.0f);
        backFaceUVs[k] = glm::vec2(((j + 1) * 1.0/nDivs),
                                   0.0 + (i * 1.0/nDivs));
        backFaceUVs[k + 1] = glm::vec2(0.0 + (j * 1.0/nDivs),
                                       0.0 + (i * 1.0/nDivs));

      }

      _frontFace->addData(bsg::GLDATA_VERTICES, "position", frontFaceVertices);
      _frontFace->addData(bsg::GLDATA_COLORS, "color", frontFaceColors);
      _frontFace->addData(bsg::GLDATA_NORMALS, "normal", frontFaceNormals);
      _frontFace->addData(bsg::GLDATA_TEXCOORDS, "texture", frontFaceUVs);
      _frontFace->setDrawType(GL_TRIANGLE_STRIP, frontFaceVertices.size());

      _backFace->addData(bsg::GLDATA_VERTICES, "position", backFaceVertices);
      _backFace->addData(bsg::GLDATA_COLORS, "color", backFaceColors);
      _backFace->addData(bsg::GLDATA_NORMALS, "normal", backFaceNormals);
      _backFace->addData(bsg::GLDATA_TEXCOORDS, "texture", backFaceUVs);
      _backFace->setDrawType(GL_TRIANGLE_STRIP, backFaceVertices.size());

      addObject(_frontFace);
      addObject(_backFace);
    }
  }

  drawableRectangle::drawableRectangle(bsgPtr<shaderMgr> pShader,
                                       const float &width, const float &height,
                                       const glm::vec4 color) :
    drawableCompound(pShader), _height(height), _width(width) {

    _name = randomName("rect");
    _frontFace = new drawableObj();
    _backFace = new drawableObj();

    std::vector<glm::vec4> frontFaceVertices;

    float w = _width/2.0f;
    float h = _height/2.0f;

    frontFaceVertices.push_back(glm::vec4( -w, -h, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(  w, -h, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4( -w,  h, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(  w,  h, 0.0f, 1.0f));

    _frontFace->addData(bsg::GLDATA_VERTICES, "position", frontFaceVertices);

    // Here are the corresponding colors for the above vertices.
    std::vector<glm::vec4> frontFaceColors;

    // If no color has been provided, the default color arg is (0,0,0,0).
    // If this is the case, then the default behavior is to make each of the
    // 4 vertices a different color.
    if (color == glm::vec4(0, 0, 0, 0)) {
      frontFaceColors.push_back(glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f));
      frontFaceColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));
      frontFaceColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
      frontFaceColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));

    // If color data has been provided, on the other hand, then make all
    // the vertices that color.
    } else {
      frontFaceColors.push_back(color);
      frontFaceColors.push_back(color);
      frontFaceColors.push_back(color);
      frontFaceColors.push_back(color);
    }

    _frontFace->addData(bsg::GLDATA_COLORS, "color", frontFaceColors);

    std::vector<glm::vec4> frontFaceNormals;

    frontFaceNormals.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f));
    frontFaceNormals.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f));
    frontFaceNormals.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f));
    frontFaceNormals.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f));

    _frontFace->addData(bsg::GLDATA_NORMALS, "normal", frontFaceNormals);

    std::vector<glm::vec2> frontFaceUVs;

    frontFaceUVs.push_back(glm::vec2( 0.0f, 0.0f));
    frontFaceUVs.push_back(glm::vec2( 1.0f, 0.0f));
    frontFaceUVs.push_back(glm::vec2( 0.0f, 1.0f));
    frontFaceUVs.push_back(glm::vec2( 1.0f, 1.0f));

    _frontFace->addData(bsg::GLDATA_TEXCOORDS, "texture", frontFaceUVs);

    // The vertices above are arranged into a set of triangles.
    _frontFace->setDrawType(GL_TRIANGLE_STRIP);

    // Same thing for the other rectangle.
    std::vector<glm::vec4> backFaceVertices;

    backFaceVertices.push_back(glm::vec4( -w, -h, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4( -w,  h, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(  w, -h, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(  w,  h, 0.0f, 1.0f));

    _backFace->addData(bsg::GLDATA_VERTICES, "position", backFaceVertices);

    // And the corresponding colors for the above vertices.
    std::vector<glm::vec4> backFaceColors;

    if (color == glm::vec4(0, 0, 0, 0)) {
      backFaceColors.push_back(glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f));
      backFaceColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
      backFaceColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));
      backFaceColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));
    } else {
      backFaceColors.push_back(color);
      backFaceColors.push_back(color);
      backFaceColors.push_back(color);
      backFaceColors.push_back(color);
    }

    _backFace->addData(bsg::GLDATA_COLORS, "color", backFaceColors);

    std::vector<glm::vec4> backFaceNormals;

    backFaceNormals.push_back(glm::vec4( 0.0f, 0.0f,-1.0f, 0.0f));
    backFaceNormals.push_back(glm::vec4( 0.0f, 0.0f,-1.0f, 0.0f));
    backFaceNormals.push_back(glm::vec4( 0.0f, 0.0f,-1.0f, 0.0f));
    backFaceNormals.push_back(glm::vec4( 0.0f, 0.0f,-1.0f, 0.0f));

    _backFace->addData(bsg::GLDATA_NORMALS, "normal", backFaceNormals);

    std::vector<glm::vec2> backFaceUVs;

    backFaceUVs.push_back(glm::vec2( 0.0f, 0.0f));
    backFaceUVs.push_back(glm::vec2( 0.0f, 1.0f));
    backFaceUVs.push_back(glm::vec2( 1.0f, 0.0f));
    backFaceUVs.push_back(glm::vec2( 1.0f, 1.0f));

    _backFace->addData(bsg::GLDATA_TEXCOORDS, "texture", backFaceUVs);

    // The vertices above are arranged into a set of triangles.
    _backFace->setDrawType(GL_TRIANGLE_STRIP);

    addObject(_frontFace);
    addObject(_backFace);
  }

  drawableRectangleOutline::drawableRectangleOutline(bsgPtr<shaderMgr> pShader,
                                       const float &width, const float &height,
                                       const float &strokeWidth,
                                       const glm::vec4 color) :
    drawableCompound(pShader), _height(height), _width(width),
    _strokeWidth(strokeWidth) {

    _name = randomName("rect");
    _frontFace = new drawableObj();
    _backFace = new drawableObj();

    float w = _width/2.0f;
    float h = _height/2.0f;
    float s = _strokeWidth;

    std::vector<glm::vec4> frontFaceVertices;

    frontFaceVertices.push_back(glm::vec4(-w-s,-h-s, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(-w,    -h, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(-w-s, h+s, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(-w,     h, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(w+s,  h+s, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(w,      h, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(w+s, -h-s, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(w,     -h, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(-w-s,-h-s, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(-w,    -h, 0.0f, 1.0f));

    _frontFace->addData(bsg::GLDATA_VERTICES, "position", frontFaceVertices);

    // Here are the corresponding colors for the above vertices.
    std::vector<glm::vec4> frontFaceColors;

    for (int i=0; i<10; i++) {
      frontFaceColors.push_back(color);
    }
    
    _frontFace->addData(bsg::GLDATA_COLORS, "color", frontFaceColors);

    std::vector<glm::vec4> frontFaceNormals;

    for (int i=0; i<10; i++) {
      frontFaceNormals.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f));
    }

    _frontFace->addData(bsg::GLDATA_NORMALS, "normal", frontFaceNormals);

    // The vertices above are arranged into a set of triangles.
    _frontFace->setDrawType(GL_TRIANGLE_STRIP);

    std::vector<glm::vec4> backFaceVertices;

    backFaceVertices.push_back(glm::vec4(w+s,-h-s, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(w,    -h, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(w+s, h+s, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(w,     h, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(-w-s,  h+s, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(-w,      h, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(-w-s, -h-s, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(-w,     -h, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(w+s,-h-s, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(w,    -h, 0.0f, 1.0f));

    _backFace->addData(bsg::GLDATA_VERTICES, "position", backFaceVertices);

    // Here are the corresponding colors for the above vertices.
    _backFace->addData(bsg::GLDATA_COLORS, "color", frontFaceColors);

    std::vector<glm::vec4> backFaceNormals;

    for (int i=0; i<10; i++) {
      backFaceNormals.push_back(glm::vec4( 0.0f, 0.0f, -1.0f, 0.0f));
    }

    _backFace->addData(bsg::GLDATA_NORMALS, "normal", backFaceNormals);

    // The vertices above are arranged into a set of triangles.
    _backFace->setDrawType(GL_TRIANGLE_STRIP);

    addObject(_frontFace);
    addObject(_backFace);
  }


  drawableSphere::drawableSphere(bsgPtr<shaderMgr> pShader,
                                       const int &phiTesselation, const int &thetaTesselation, const glm::vec4 &color) :
    drawableCompound(pShader), _phi(phiTesselation), _theta(thetaTesselation) {

    _name = randomName("sphere");

    float pi = 3.14159265358979323;
    float r = 0.5;
    float thetaStep = 2 * pi/_theta;
    float phiStep = pi/_phi;

    std::vector<glm::vec4> verts(0);
    std::vector<glm::vec2> uvs(0);
    std::vector<glm::vec4> normals(0);
    std::vector<glm::vec4> colors(0);

    // Uses a triangle strip to draw the sphere, so two vertices are defined at a time and are automatically turned into
    // a strip of triangles. (the / in |/|/|.../| are automatically filled in.)
    for (int j = 0; j < _phi; j++) {
        for (int i = 0; i < (_theta + 1); i++) {
            // Top vertex position
            verts.push_back(glm::vec4(r * std::sin(phiStep * j) * std::cos(-thetaStep * i), r * std::cos(phiStep * j),
              r * std::sin(phiStep * j) * std::sin(-thetaStep * i), 1.0f));

            // Top vertex normal
            glm::vec4 normal = glm::vec4(r * std::sin(phiStep * j) * std::cos(-thetaStep * i),
                                         r * std::cos(phiStep * j),
                                         r * std::sin(phiStep * j) * std::sin(-thetaStep * i), 0.0f);

            normal = glm::normalize(normal);
            normals.push_back(normal);

            // Top UV
            uvs.push_back(glm::vec2(static_cast<float>(i)/thetaTesselation, 1.0f - static_cast<float>(j)/phiTesselation));

            // Bottom vertex position
            verts.push_back(glm::vec4(r * std::sin(phiStep * (j + 1)) * std::cos(-thetaStep * i), r * std::cos(phiStep * (j + 1)),
              r * std::sin(phiStep * (j + 1)) * std::sin(-thetaStep * i), 1.0f));

            // Bottom vertex normal
            normal = glm::vec4(r * std::sin(phiStep * (j + 1)) * std::cos(-thetaStep * i),
                                r * std::cos(phiStep * (j + 1)),
                                r * std::sin(phiStep * (j + 1)) * std::sin(-thetaStep * i), 0.0f);
            normal = normalize(normal);
            normals.push_back(normal);


            // Bottom UV
            uvs.push_back(glm::vec2(static_cast<float>(i)/thetaTesselation, 1.0f - static_cast<float>(j + 1)/phiTesselation));

            // float c1 = ((double) rand() / (RAND_MAX));
            // float c2 = ((double) rand() / (RAND_MAX));
            // float c3 = ((double) rand() / (RAND_MAX));
            // float c4 = ((double) rand() / (RAND_MAX));
            // float c5 = ((double) rand() / (RAND_MAX));
            // float c6 = ((double) rand() / (RAND_MAX));
            // glm::vec4 randColor = glm::vec4(c1, c2, c3, 1.0f);
            // glm::vec4 randColor2 = glm::vec4(c4, c5, c6, 1.0f);

            // Color
            colors.push_back(color);
            colors.push_back(color);
        }
    }



    _sphere = new drawableObj();

    _sphere->addData(bsg::GLDATA_VERTICES, "position", verts);

    _sphere->addData(bsg::GLDATA_COLORS, "color", colors);

    _sphere->addData(bsg::GLDATA_NORMALS, "normal", normals);

    _sphere->addData(bsg::GLDATA_TEXCOORDS, "texture", uvs);

    // The vertices above are arranged into a set of triangles.
    _sphere->setDrawType(GL_TRIANGLE_STRIP, verts.size());

    addObject(_sphere);
  }

  drawableCircle::drawableCircle(bsgPtr<shaderMgr> pShader, const int &thetaTesselation, const float &normalDirection, const float &yPos) :
    drawableCompound(pShader), _theta(thetaTesselation) {
      _name = randomName("circle");
      glm::vec4 color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
      _circle = new drawableObj();
      getCircle(_circle, _theta, normalDirection, yPos, color);
      addObject(_circle);
    }

  // Useful function for creating the caps for the cylinder and the bottom of the cone.
  void drawableCircle::getCircle(bsgPtr<drawableObj> circle, const int &thetaTesselation, const float &normalDirection, const float &yPos, const glm::vec4 &color) {


      std::vector<glm::vec4> verts(0);
      std::vector<glm::vec2> uvs(0);
      std::vector<glm::vec4> normals(0);
      std::vector<glm::vec4> colors(0);

      float pi = 3.14159265358979323;
      float r = 0.5f;
      float thetaStep = 2 * pi/thetaTesselation;

      // Uses a triangle fan to draw the circle, so the central point is defined, followed by points
      // around the perimiter of the circle. This could have been done with a strip, but that would mean
      // redundantly including the center point many times.

      // Top vertex position
      verts.push_back(glm::vec4(0.0f, yPos, 0.0f, 1.0f));

      // Top vertex normal
      glm::vec4 normal = glm::vec4(0.0f, normalDirection, 0.0f, 0.0f);
      normalize(normal);
      normals.push_back(normal);

      // Top UV
      uvs.push_back(glm::vec2(0.5f, 0.5f));

      colors.push_back(color);

      for (int j = 0; j < (thetaTesselation + 1); j++) {

          verts.push_back(glm::vec4(r * glm::cos(-normalDirection * thetaStep*j), yPos,
            r * glm::sin(-normalDirection * thetaStep*j), 1.0f));

          normal = glm::vec4(0.0f, normalDirection, 0.0f, 0.0f);
          normalize(normal);
          normals.push_back(normal);

          uvs.push_back(glm::vec2(r * glm::cos(thetaStep*j) + 0.5f, r * glm::sin(thetaStep*j) + 0.5f));

          // Color
          colors.push_back(color);
      }
      circle->addData(bsg::GLDATA_VERTICES, "position", verts);

      circle->addData(bsg::GLDATA_COLORS, "color", colors);

      circle->addData(bsg::GLDATA_NORMALS, "normal", normals);

      circle->addData(bsg::GLDATA_TEXCOORDS, "texture", uvs);

      // The vertices above are arranged into a set of triangles.
      circle->setDrawType(GL_TRIANGLE_FAN, verts.size());
    }


  drawableSquare::drawableSquare(bsgPtr<shaderMgr> pShader, const int &tesselation, const glm::vec3 &topLeft, const glm::vec3 &topRight, const glm::vec3 &bottomLeft, const glm::vec4 &color) :
    drawableCompound(pShader), _tess(tesselation) {
      _name = randomName("square");
      _rect = new drawableObj();
      getRect(_rect, _tess, topLeft, topRight, bottomLeft, color);
      addObject(_rect);
    }
  // Generalized rectangle shape, useful for defining the cube. A somewhat more flexible, but less directly definable version of drawableRectangle.
  // the first argument is a shape to be filled in by this static function.
  void drawableSquare::getRect(bsgPtr<drawableObj> rect, const int &tesselation, const glm::vec3 &topLeft, const glm::vec3 &topRight, const glm::vec3 &bottomLeft, const glm::vec4 &color) {


      std::vector<glm::vec4> verts(0);
      std::vector<glm::vec2> uvs(0);
      std::vector<glm::vec4> normals(0);
      std::vector<glm::vec4> colors(0);

      glm::vec3 horizontal = (topRight - topLeft) * (1.0f / tesselation);
      glm::vec3 vertical = (bottomLeft - topLeft) * (1.0f / tesselation);

      glm::vec3 n = glm::cross(horizontal, vertical);
      glm::vec4 normal = glm::vec4(n, 0.0f);
      glm::normalize(normal);

      for (int i = 0; i < tesselation; ++i) {
        for (int j = 0; j <= tesselation; ++j) {
          // current vertex
          glm::vec3 currPos = topLeft + ((float) i * vertical) + ((float) j * horizontal);
          // next vertex, directly below current. GL_TRIANGLE_STRIP will fill in the / in the |/|/.../| pattern.
          glm::vec3 v = currPos + vertical;
          verts.push_back(glm::vec4(currPos, 1.0f));
          verts.push_back(glm::vec4(v, 1.0f));

          normals.push_back(normal);
          normals.push_back(normal);

          uvs.push_back(glm::vec2(static_cast<float>(j)/tesselation, 1.0f - static_cast<float>(i)/tesselation));
          uvs.push_back(glm::vec2(static_cast<float>(j)/tesselation, 1.0f - static_cast<float>(i + 1)/tesselation));

          colors.push_back(color);
          colors.push_back(color);
        }
      }


      rect->addData(bsg::GLDATA_VERTICES, "position", verts);

      rect->addData(bsg::GLDATA_COLORS, "color", colors);

      rect->addData(bsg::GLDATA_NORMALS, "normal", normals);

      rect->addData(bsg::GLDATA_TEXCOORDS, "texture", uvs);

      // The vertices above are arranged into a set of triangles.
      rect->setDrawType(GL_TRIANGLE_STRIP, verts.size());
    }


  drawableCube::drawableCube(bsgPtr<shaderMgr> pShader,
                                       const int &tesselation, const glm::vec4 &color) :
    drawableCompound(pShader), _tess(tesselation) {
      _name = randomName("cube");

      _front = new drawableObj;
      _back = new drawableObj;
      _left = new drawableObj;
      _right = new drawableObj;
      _top = new drawableObj;
      _bottom = new drawableObj;

      drawableSquare::getRect(_front, _tess, glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0.5, 0.5, 0.5), glm::vec3(-0.5, -0.5, 0.5), color);
      drawableSquare::getRect(_back, _tess, glm::vec3(0.5, 0.5, -0.5), glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0.5, -0.5, -0.5), color);
      drawableSquare::getRect(_left, _tess, glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-0.5, -0.5, -0.5), color);
      drawableSquare::getRect(_right, _tess, glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.5, 0.5, -0.5), glm::vec3(0.5, -0.5, 0.5), color);
      drawableSquare::getRect(_top, _tess, glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0.5, 0.5, -0.5), glm::vec3(-0.5, 0.5, 0.5), color);
      drawableSquare::getRect(_bottom, _tess, glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0.5, -0.5, 0.5), glm::vec3(-0.5, -0.5, -0.5), color);

      addObject(_front);
      addObject(_back);
      addObject(_left);
      addObject(_right);
      addObject(_top);
      addObject(_bottom);
    }

  drawableCone::drawableCone(bsgPtr<shaderMgr> pShader,
                                       const int &heightTesselation, const int &thetaTesselation, const glm::vec4 &color) :
    drawableCompound(pShader), _height(heightTesselation), _theta(thetaTesselation) {

    _name = randomName("cone");

    _cap = new drawableObj();
    _base = new drawableObj();

    float radius = 0.5;
    float height = 0.5;


    float pi = 3.14159265358979323;
    float thetaStep = 2 * pi/thetaTesselation;
    float heightStep = 2 * height/heightTesselation;

    std::vector<glm::vec4> verts(0);
    std::vector<glm::vec2> uvs(0);
    std::vector<glm::vec4> normals(0);
    std::vector<glm::vec4> colors(0);



    for (int i = 0; i < heightTesselation; i++) {
        for (int j = 0; j < (thetaTesselation + 1); j++) {
            int linearScale = heightTesselation - i;
            // Top vertex position
            verts.push_back(glm::vec4(radius*heightStep*(linearScale - 1) * std::cos(-thetaStep*j), heightStep * (i + 1) - height/2,
              radius*heightStep*(linearScale - 1) * std::sin(-thetaStep*j), 1.0f));

            // Top vertex normal
            glm::vec4 normal = glm::vec4(2/(std::sqrt(5)) * std::cos(-thetaStep*j),
                                         1/(std::sqrt(5)),
                                         2/(std::sqrt(5)) * std::sin(-thetaStep*j), 0.0f);
            normal = normalize(normal);
            normals.push_back(normal);

            // Top UV
            uvs.push_back(glm::vec2(static_cast<float>(j)/thetaTesselation, static_cast<float>(i + 1)/heightTesselation));

            // Bottom vertex position
            verts.push_back(glm::vec4(radius*heightStep*linearScale * std::cos(-thetaStep*j), heightStep * i - height/2,
              radius*heightStep*linearScale * std::sin(-thetaStep*j), 1.0f));

            // Bottom vertex normal
            normals.push_back(normal);

            // Top UV
            uvs.push_back(glm::vec2(static_cast<float>(j)/thetaTesselation, static_cast<float>(i)/heightTesselation));

            // Color
            colors.push_back(color);
            colors.push_back(color);
        }

    }

    _cap->addData(bsg::GLDATA_VERTICES, "position", verts);

    _cap->addData(bsg::GLDATA_COLORS, "color", colors);

    _cap->addData(bsg::GLDATA_NORMALS, "normal", normals);

    _cap->addData(bsg::GLDATA_TEXCOORDS, "texture", uvs);

    // The vertices above are arranged into a set of triangles.
    _cap->setDrawType(GL_TRIANGLE_STRIP, verts.size());

    drawableCircle::getCircle(_base, _theta, -1, -radius/2.0f, color);

    addObject(_cap);
    addObject(_base);
  }


  void drawableCone::makeCone(bsgPtr<drawableObj> cone, int heightTesselation, int thetaTesselation, float radius, float height, glm::vec4 color) {

}

  drawableCylinder::drawableCylinder(bsgPtr<shaderMgr> pShader, const int &heightTesselation, const int &thetaTesselation, const glm::vec4 &color) :
    drawableCompound(pShader), _height(heightTesselation), _theta(thetaTesselation) {

    float pi = 3.14159265358979323;
    float r = 0.5;
    float thetaStep = 2 * pi/thetaTesselation;
    float heightStep = 1.f/heightTesselation;

    std::vector<glm::vec4> verts(0);
    std::vector<glm::vec2> uvs(0);
    std::vector<glm::vec4> normals(0);
    std::vector<glm::vec4> colors(0);

    for (int i = 0; i < heightTesselation; i++) {
        for (int j = 0; j < (thetaTesselation + 1); j++) {

            // Top vertex position
            verts.push_back(glm::vec4(r * std::cos(-thetaStep * j), heightStep * (i + 1) - r, r * std::sin(-thetaStep * j), 1.0f));

            // Top vertex normal
            glm::vec4 normal = glm::vec4(r * std::cos(-thetaStep * j), 0, r * std::sin(-thetaStep * j), 0.0f);
            normal = normalize(normal);
            normals.push_back(normal);

            // Top UV
            uvs.push_back(glm::vec2(static_cast<float>(j)/thetaTesselation, static_cast<float>(i + 1)/heightTesselation));

            // Bottom vertex position
            verts.push_back(glm::vec4(r * std::cos(-thetaStep * j), heightStep * i - r, r * std::sin(-thetaStep * j), 1.0f));

            // Bottom vertex normal
            normals.push_back(normal);

            // Bottom UV
            uvs.push_back(glm::vec2(static_cast<float>(j)/thetaTesselation, static_cast<float>(i)/heightTesselation));

            // Colors
            colors.push_back(color);
            colors.push_back(color);
        }
    }

    _base = new drawableObj();
    _body = new drawableObj();
    _top = new drawableObj();

    _body->addData(bsg::GLDATA_VERTICES, "position", verts);

    _body->addData(bsg::GLDATA_COLORS, "color", colors);

    _body->addData(bsg::GLDATA_NORMALS, "normal", normals);

    _body->addData(bsg::GLDATA_TEXCOORDS, "texture", uvs);

    _body->setDrawType(GL_TRIANGLE_STRIP, verts.size());

    drawableCircle::getCircle(_top, _theta, 1, r, color);
    drawableCircle::getCircle(_base, _theta, -1, -r, color);

    addObject(_base);
    addObject(_body);
    addObject(_top);
  }

  drawableAxes::drawableAxes(bsgPtr<shaderMgr> pShader, const float &length) :
    drawableCompound(pShader), _length(length) {

    _name = randomName("axes");
    _axes = new drawableObj();

    std::vector<glm::vec4> axesVertices;
    axesVertices.push_back(glm::vec4( -_length, 0.0f, 0.0f, 1.0f));
    axesVertices.push_back(glm::vec4(  _length, 0.0f, 0.0f, 1.0f));

    axesVertices.push_back(glm::vec4( 0.0f, -_length, 0.0f, 1.0f));
    axesVertices.push_back(glm::vec4( 0.0f,  _length, 0.0f, 1.0f));

    axesVertices.push_back(glm::vec4( 0.0f, 0.0f, -_length, 1.0f));
    axesVertices.push_back(glm::vec4( 0.0f, 0.0f,  _length, 1.0f));

    // With colors. (X = red, Y = green, Z = blue)
    std::vector<glm::vec4> axesColors;
    axesColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));
    axesColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));

    axesColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
    axesColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));

    axesColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));
    axesColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));

    _axes->addData(bsg::GLDATA_VERTICES, "position", axesVertices);
    _axes->addData(bsg::GLDATA_COLORS, "color", axesColors);

    // The axes are not triangles, but lines.
    _axes->setDrawType(GL_LINES);

    // It's rarely what the user wants to select the axes.
    _axes->setSelectable(false);

    addObject(_axes);
  }


drawableLine::drawableLine(bsgPtr<shaderMgr> pShader,
                           const glm::vec3 &start, const glm::vec3 &end,
                           const glm::vec4 &color) :
  drawableCompound(pShader) {

  _name = randomName("line");
  _line = new drawableObj();

  std::vector<glm::vec4> lineVertices;
  lineVertices.push_back(glm::vec4(start.x, start.y, start.z, 1.0f));
  lineVertices.push_back(glm::vec4(end.x, end.y, end.z, 1.0f));

  std::vector<glm::vec4> lineColors;
  lineColors.push_back(glm::vec4(color.r, color.g, color.b, 1.0f));
  lineColors.push_back(glm::vec4(color.r, color.g, color.b, 1.0f));

  _line->addData(bsg::GLDATA_VERTICES, "position", lineVertices);
  _line->addData(bsg::GLDATA_COLORS, "color", lineColors);

  _line->setDrawType(GL_LINES);

  _line->setSelectable(false);
  _line->setInterleaved(false);

  addObject(_line);
}

void drawableLine::setLineEnds(const glm::vec3 &start, const glm::vec3 &end) {

  std::vector<glm::vec4> lineVertices;
  lineVertices.push_back(glm::vec4(start.x, start.y, start.z, 1.0f));
  lineVertices.push_back(glm::vec4(end.x, end.y, end.z, 1.0f));

  _line->setData(bsg::GLDATA_VERTICES, lineVertices);

}


drawableSaggyLine::drawableSaggyLine(bsgPtr<shaderMgr> pShader,
                                     const glm::vec3 &start, const glm::vec3 &end,
                                     const glm::vec4 &startColor,
                                     const glm::vec4 &endColor,
                                     const int &nSegments,
                                     const float &sagFactor) :
  drawableCompound(pShader),
  _start(start), _end(end), _sagFactor(sagFactor), _nSegments(nSegments) {

  _name = randomName("saggyLine");
  _line = new drawableObj();

  std::vector<glm::vec4> lineVertices = _calculateCatenary(start, end,
                                                           nSegments,
                                                           sagFactor);

  std::vector<glm::vec4> lineColors;
  glm::vec4 spanColor = (endColor - startColor)/(float)nSegments;

  for (int i = 0; i <= _nSegments; i++) {
    lineColors.push_back(glm::vec4(startColor.r + i * spanColor.r,
                                   startColor.g + i * spanColor.g,
                                   startColor.b + i * spanColor.b,
                                   1.0f));
  }

  _line->addData(bsg::GLDATA_VERTICES, "position", lineVertices);
  _line->addData(bsg::GLDATA_COLORS, "color", lineColors);

  _line->setDrawType(GL_LINE_STRIP);

  _line->setSelectable(false);
  _line->setInterleaved(false);

  addObject(_line);
}

void drawableSaggyLine::setLineEnds(const glm::vec3 &start, const glm::vec3 &end) {

  std::vector<glm::vec4> lineVertices = _calculateCatenary(start, end,
                                                           _nSegments,
                                                           _sagFactor);
  _line->setData(bsg::GLDATA_VERTICES, lineVertices);
}

std::vector<glm::vec4>
drawableSaggyLine::_calculateCatenary(const glm::vec3 &start,
                                      const glm::vec3 &end,
                                      const int &nSegments,
                                      const float &sagFactor) {

  std::vector<glm::vec4> out;
  glm::vec3 span = (end - start)/(float)nSegments;

  float len = glm::distance(start, end);
  float k = pow(len/2.0f, 2); // / sagFactor;
  float f;

  for (int i = 0; i <= _nSegments; i++) {

    f = sagFactor/len * (pow(len * float(i)/float(_nSegments) - len/2.0f, 2) - k);

    out.push_back(glm::vec4(start.x + i * span.x,
                            start.y + i * span.y - f,
                            start.z + i * span.z,
                            1.0f));
  }

  return out;
}

drawablePoints::drawablePoints(bsgPtr<shaderMgr> pShader,
                               const std::vector<glm::vec4> &points,
                               const std::vector<glm::vec4> &colors) :
  drawableCompound(pShader) {

  _name = randomName("points");
  _points = new drawableObj();

  _points->addData(bsg::GLDATA_VERTICES, "position", points);
  _points->addData(bsg::GLDATA_COLORS, "color", colors);

  _points->setDrawType(GL_POINTS);

  _points->setSelectable(false);
  _points->setInterleaved(false);

  addObject(_points);
}

drawableText::drawableText(bsgPtr<shaderMgr> pShader, const char *text, 
                           const float height, const char *fontFilePath,
                           const glm::vec4 color, bsgPtr<fontTextureMgr> texture) :
    drawableCompound(pShader),
    _texture(texture),
    _text(text),
    _height(height),
    _fontFilePath(fontFilePath),
    _color(color) {

  _name = randomName("text");

  if (!_texture) {
    _texture = new bsg::fontTextureMgr();
    _pShader->addTexture(bsgPtr<textureMgr>((textureMgr *) (_texture.ptr())));
  }

  // If the font requested isn't already in this texture's fontMap, we need to
  // load it in. If it is, we don't do anything.
  if (!_texture->getFont(_fontFilePath)) {
    std::cout << "we have not got that font already" << std::endl;
    _texture->readFile(bsg::textureTTF, _fontFilePath);
  } else {
    std::cout << "we have that font already" << std::endl;
  }

  _write();
}

void drawableText::_write() {

  texture_font_t *font = _texture->getFont(_fontFilePath);

  // vertex_buffer_t *buffer = vertex_buffer_new("vertex:3f,tex_coord:2f,color:4f");

  // _height is the height we want the text to be, in world units. But
  // freetypegl measures font size in pixels, so if we don't scale the
  // coordinates it gives us somehow, the text will show up many times too big.
  // So, we scale the coordinates by dividing height (the height that the user
  // wanted) by font's height attribute. See:
  // https://github.com/rougier/freetype-gl/blob/master/texture-font.h
  float scaling_factor = _height / font->height;
  int i = 0;
  vec2 pen = {{0.0f, 0.0f}};

  while(_text[i]) {
    texture_glyph_t *glyph = texture_font_get_glyph(font, &_text[i]);

    float kerning = 0.0f;
    if (i > 0) {
      kerning = texture_glyph_get_kerning(glyph, _text + i - 1);
    }

    pen.x += kerning;
    pen.x += glyph->advance_x;

    if (pen.y < glyph->height) {
      pen.y = glyph->height;
    }
    i++;
  }

  float w = scaling_factor * pen.x;
  float h = scaling_factor * pen.y;

  i = 0;
  pen.x = 0.0f;
  pen.y = 0.0f;

  while (_text[i]) {
    bsgPtr<drawableObj> _frontFace = new drawableObj();
    bsgPtr<drawableObj> _backFace = new drawableObj();
    texture_glyph_t *glyph = texture_font_get_glyph(font, &_text[i]);

    float kerning = 0.0f;
    if (i > 0) {
      kerning = texture_glyph_get_kerning(glyph, _text + i - 1);
    }

    pen.x += kerning;

    float x0  = (pen.x + glyph->offset_x);
    float y0  = (int)(pen.y + glyph->offset_y);
    float x1  = (x0 + glyph->width);
    float y1  = (int)(y0 - glyph->height);

    std::vector<glm::vec4> frontFaceVertices;

    frontFaceVertices.push_back(glm::vec4(scaling_factor*x0-w/2, scaling_factor*y1-h/2, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(scaling_factor*x1-w/2, scaling_factor*y1-h/2, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(scaling_factor*x0-w/2, scaling_factor*y0-h/2, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(scaling_factor*x1-w/2, scaling_factor*y0-h/2, 0.0f, 1.0f));

    _frontFace->addData(bsg::GLDATA_VERTICES, "position", frontFaceVertices);

    std::vector<glm::vec4> frontFaceNormals;

    frontFaceNormals.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
    frontFaceNormals.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
    frontFaceNormals.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
    frontFaceNormals.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

    _frontFace->addData(bsg::GLDATA_NORMALS, "normal", frontFaceNormals);

    std::vector<glm::vec2> frontFaceUVs;

    float u0 = glyph->s0;
    float v0 = glyph->t0;
    float u1 = glyph->s1;
    float v1 = glyph->t1;

    frontFaceUVs.push_back(glm::vec2(u0, v1));
    frontFaceUVs.push_back(glm::vec2(u1, v1));
    frontFaceUVs.push_back(glm::vec2(u0, v0));
    frontFaceUVs.push_back(glm::vec2(u1, v0));

    _frontFace->addData(bsg::GLDATA_TEXCOORDS, "texture", frontFaceUVs);

    // Here are the corresponding colors for the above vertices.
    std::vector<glm::vec4> frontFaceColors;
    frontFaceColors.push_back(_color);
    frontFaceColors.push_back(_color);
    frontFaceColors.push_back(_color);
    frontFaceColors.push_back(_color);

    _frontFace->addData(bsg::GLDATA_COLORS, "color", frontFaceColors);
    
    // The vertices above are arranged into a set of triangles.
    _frontFace->setDrawType(GL_TRIANGLE_STRIP);  

    // Same thing for the back-facing rectangle.
    std::vector<glm::vec4> backFaceVertices;

    backFaceVertices.push_back(glm::vec4(scaling_factor*x0-w/2, scaling_factor*y1-h/2, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(scaling_factor*x0-w/2, scaling_factor*y0-h/2, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(scaling_factor*x1-w/2, scaling_factor*y1-h/2, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(scaling_factor*x1-w/2, scaling_factor*y0-h/2, 0.0f, 1.0f));

    _backFace->addData(bsg::GLDATA_VERTICES, "position", backFaceVertices);

    std::vector<glm::vec4> backFaceNormals;

    backFaceNormals.push_back(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    backFaceNormals.push_back(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    backFaceNormals.push_back(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    backFaceNormals.push_back(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));

    _backFace->addData(bsg::GLDATA_NORMALS, "normal", backFaceNormals);

    std::vector<glm::vec2> backFaceUVs;

    backFaceUVs.push_back(glm::vec2(u0, v1));
    backFaceUVs.push_back(glm::vec2(u0, v0));
    backFaceUVs.push_back(glm::vec2(u1, v1));
    backFaceUVs.push_back(glm::vec2(u1, v0));

    _backFace->addData(bsg::GLDATA_TEXCOORDS, "texture", backFaceUVs);

    std::vector<glm::vec4> backFaceColors;
    backFaceColors.push_back(_color);
    backFaceColors.push_back(_color);
    backFaceColors.push_back(_color);
    backFaceColors.push_back(_color);

    _backFace->addData(bsg::GLDATA_COLORS, "color", backFaceColors);
    
    _backFace->setDrawType(GL_TRIANGLE_STRIP);  

    addObject(_frontFace);
    addObject(_backFace);

    pen.x += glyph->advance_x;

    i++;
  }
}

bsgPtr<fontTextureMgr> drawableText::getFontTexture() {
  return _texture;
}


drawableTextRect::drawableTextRect(bsgPtr<shaderMgr> textShader,
                   bsgPtr<shaderMgr> backgroundShader,
                   const char *text,
                   const char *fontFilePath,
                   bsgPtr<fontTextureMgr> texture,
                   const float textHeight,
                   const glm::vec4 textColor,
                   const glm::vec4 backgroundColor,
                   const glm::vec4 borderColor,
                   const float boxHeight,
                   const float boxWidth,
                   const float borderWidth,
                   const float offsetDist) : 
    drawableCollection(),
    _text(text),
    _fontFilePath(fontFilePath),
    _texture(texture),
    _textHeight(textHeight),
    _textColor(textColor),
    _backgroundColor(backgroundColor),
    _borderColor(borderColor),
    _boxHeight(boxHeight),
    _boxWidth(boxWidth),
    _borderWidth(borderWidth),
    _offsetDist(offsetDist) {

  std::cout << "1" << std::endl;
  // The background rectangle
  drawableRectangle *rect = new drawableRectangle(backgroundShader, 
    _boxWidth, _boxHeight, _backgroundColor);

  // The outline for the rectangle
  drawableRectangleOutline *outline = new drawableRectangleOutline(
    backgroundShader, _boxWidth, _boxHeight, _borderWidth, _borderColor);

  // The text itself
  drawableText *drawText;
  drawText = new drawableText(textShader, _text, _textHeight,
    _fontFilePath, _textColor, _texture);

  // If this object wasn't passed a _texture initially, that means this is
  // probably the first piece of text to be displayed in the scene, so we
  // grab the _texture that drawText is using and make that accessible to
  // other people who might want to use it.
  if (!_texture) {
    _texture = drawText->getFontTexture();
  }

  // Bump it forward juust a bit to avoid z-fighting
  drawText->setPosition(0.0f, 0.0f, _offsetDist);

  // Add everybody together!
  addObject(rect);
  addObject(outline);
  addObject(drawText);
}

bsgPtr<fontTextureMgr> drawableTextRect::getFontTexture() {
  return _texture;
}

drawableTextBox::drawableTextBox(bsgPtr<shaderMgr> textShader,
                   bsgPtr<shaderMgr> backgroundShader,
                   const char *text,
                   const char *fontFilePath,
                   bsgPtr<fontTextureMgr> texture,
                   const float extrusion,
                   const float textHeight,
                   const glm::vec4 textColor,
                   const glm::vec4 backgroundColor,
                   const glm::vec4 borderColor,
                   const float boxHeight,
                   const float boxWidth,
                   const float borderWidth,
                   const float offsetDist,
                   const glm::vec4 extrusionColor) : 
    drawableCollection(),
    _text(text),
    _fontFilePath(fontFilePath),
    _texture(texture),
    _extrusion(extrusion),
    _textHeight(textHeight),
    _textColor(textColor),
    _backgroundColor(backgroundColor),
    _borderColor(borderColor),
    _boxHeight(boxHeight),
    _boxWidth(boxWidth),
    _borderWidth(borderWidth),
    _offsetDist(offsetDist),
    _extrusionColor(extrusionColor) {

  _name = randomName("textBox");

  // To make things 3d, we follow the (probably slow but much clearer) strategy
  // of creating the six rectangular faces as individual 2d objects, then
  // rotating them and moving them about until they form a rectangular prism.

  // The front face, that actually has the text on it.
  drawableTextRect *textRect = new drawableTextRect(textShader, backgroundShader, 
    _text, _fontFilePath, _texture, _textHeight, _textColor, _backgroundColor, _borderColor,
    _boxHeight, _boxWidth, _borderWidth, _offsetDist);
  textRect->setPosition(0.f, 0.f, _extrusion/2);

  // If this object wasn't passed a _texture initially, that means this is
  // probably the first piece of text to be displayed in the scene, so we
  // grab the _texture that drawText is using and make that accessible to
  // other people who might want to use it.
  if (!_texture) {
    _texture = textRect->getFontTexture();
  }

  // The backface's outline and fill.
  drawableRectangleOutline *backFaceOutline = new drawableRectangleOutline(
    backgroundShader, _boxWidth, _boxHeight, _borderWidth, _borderColor);
  backFaceOutline->setPosition(0.0f, 0.0f, -_extrusion/2);

  drawableRectangle *backFace = new drawableRectangle(backgroundShader, 
    _boxWidth, _boxHeight, _backgroundColor);
  backFace->setPosition(0.0f, 0.0f, -_extrusion/2);

  // The left, right, top, and bottom faces...
  drawableRectangle *leftFace = new drawableRectangle(backgroundShader, 
    _extrusion, _boxHeight + 2 * _borderWidth, _extrusionColor);
  leftFace->setRotation(0.0f, M_PI/2, 0.0f);
  leftFace->setPosition(-_boxWidth/2 - borderWidth, 0.f, 0.f);

  drawableRectangle *rightFace = new drawableRectangle(backgroundShader, 
    _extrusion, _boxHeight + 2 * _borderWidth, _extrusionColor);
  rightFace->setRotation(0.0f, M_PI/2, 0.0f);
  rightFace->setPosition(_boxWidth/2 + borderWidth, 0.f, 0.f);

  drawableRectangle *topFace = new drawableRectangle(backgroundShader, 
    _boxWidth + 2 * _borderWidth, _extrusion, _extrusionColor);
  topFace->setRotation(M_PI/2, 0.0f, 0.0f);
  topFace->setPosition(0.f, _boxHeight/2 + borderWidth, 0.f);

  drawableRectangle *bottomFace = new drawableRectangle(backgroundShader, 
    _boxWidth + 2 * _borderWidth, _extrusion, _extrusionColor);
  bottomFace->setRotation(M_PI/2, 0.0f, 0.0f);
  bottomFace->setPosition(0.f, -_boxHeight/2 - borderWidth, 0.f);

  // Add everybody to the collection.
  addObject(textRect);
  addObject(backFaceOutline);
  addObject(backFace);
  addObject(leftFace);
  addObject(rightFace);
  addObject(topFace);
  addObject(bottomFace);
}

bsgPtr<fontTextureMgr> drawableTextBox::getFontTexture() {
  return _texture;
}

}
