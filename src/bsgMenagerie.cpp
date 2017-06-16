#include "bsgMenagerie.h"

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
                                       const float &width, const float &height) :
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
    frontFaceColors.push_back(glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f));
    frontFaceColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));
    frontFaceColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
    frontFaceColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));

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
    backFaceColors.push_back(glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f));
    backFaceColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
    backFaceColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));
    backFaceColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));

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

  drawableSphere::drawableSphere(bsgPtr<shaderMgr> pShader,
                                       const int &phiTesselation, const int &thetaTesselation) :
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

    glm::vec4 color = glm::vec4(1.0, 0.0, 0.0, 1.0);

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
            uvs.push_back(glm::vec2(static_cast<float>(i)/thetaTesselation, static_cast<float>(j)/phiTesselation));

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
            uvs.push_back(glm::vec2(static_cast<float>(i)/thetaTesselation, static_cast<float>(j + 1)/phiTesselation));

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

  drawableCircle::drawableCircle(bsgPtr<shaderMgr> pShader, const int &thetaTesselation, const int &radiusTesselation, const float &normalDirection, const float &yPos) :
    drawableCompound(pShader), _theta(thetaTesselation), _radius(radiusTesselation) {
      _name = randomName("circle");
      glm::vec4 color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
      _circle = new drawableObj();
      getCircle(_circle, _theta, _radius, normalDirection, yPos, color);
      addObject(_circle);
    }

  void drawableCircle::getCircle(bsgPtr<drawableObj> circle, const int &thetaTesselation, const int &radiusTesselation, const float &normalDirection, const float &yPos, const glm::vec4 &color) {
      

      std::vector<glm::vec4> verts(0);
      std::vector<glm::vec2> uvs(0);
      std::vector<glm::vec4> normals(0);
      std::vector<glm::vec4> colors(0);

      float pi = 3.14159265358979323;
      float r = 0.5f;
      float thetaStep = 2 * pi/thetaTesselation;
      float radiusStep = r/radiusTesselation;

      for (int i = 0; i < radiusTesselation; i++) {
          for (int j = 0; j < (thetaTesselation + 1); j++) {
              // Top vertex position
              verts.push_back(glm::vec4(radiusStep * i * glm::cos(-normalDirection * thetaStep*j), yPos, 
                radiusStep * i * glm::sin(-normalDirection * thetaStep*j), 1.0f));

              // Top vertex normal
              glm::vec4 normal = glm::vec4(0.0f, normalDirection, 0.0f, 0.0f);
              normalize(normal);
              normals.push_back(normal);

              // Top UV
              uvs.push_back(glm::vec2(radiusStep * i * glm::cos(-thetaStep*j) + 0.5f, radiusStep * i * glm::sin(-thetaStep*j) + 0.5f));

              // Bottom vertex position
              verts.push_back(glm::vec4(radiusStep * (i + 1) * glm::cos(-normalDirection * thetaStep*j), yPos, 
                radiusStep * (i + 1) * glm::sin(-normalDirection * thetaStep*j), 1.0f));

              // Bottom vertex normal
              normal = glm::vec4(0.0f, normalDirection, 0.0f, 0.0f);
              normalize(normal);
              normals.push_back(normal);

              // Bottom UV
              uvs.push_back(glm::vec2(radiusStep * (i + 1) * glm::cos(-thetaStep*j) + 0.5f, radiusStep * (i + 1) * glm::sin(-thetaStep*j) + 0.5f));

              // Color
              colors.push_back(color);
              colors.push_back(color);
          }
      }
      circle->addData(bsg::GLDATA_VERTICES, "position", verts);

      circle->addData(bsg::GLDATA_COLORS, "color", colors);

      circle->addData(bsg::GLDATA_NORMALS, "normal", normals);

      circle->addData(bsg::GLDATA_TEXCOORDS, "texture", uvs);
    
      // The vertices above are arranged into a set of triangles.
      circle->setDrawType(GL_TRIANGLE_STRIP, verts.size());
    }

  drawableCone::drawableCone(bsgPtr<shaderMgr> pShader,
                                       const int &heightTesselation, const int &thetaTesselation) :
    drawableCompound(pShader), _height(heightTesselation), _theta(thetaTesselation) {

    _name = randomName("cone");





    _cap = new drawableObj();
    _base = new drawableObj();
    
    // Store the vertex data and other values to be used later when constructing the VAO
    float r = 0.5;

    makeCone(_cap, _height, _theta, r, r, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    drawableCircle::getCircle(_base, _theta, _height, -1, -r/2.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    //makeCaps(_base, _height, _theta, r, -r/2, glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));  

    addObject(_cap);
    addObject(_base);
  }


  void drawableCone::makeCone(bsgPtr<drawableObj> cone, int heightTesselation, int thetaTesselation, float radius, float height, glm::vec4 color) {
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

    cone->addData(bsg::GLDATA_VERTICES, "position", verts);

    cone->addData(bsg::GLDATA_COLORS, "color", colors);

    cone->addData(bsg::GLDATA_NORMALS, "normal", normals);

    cone->addData(bsg::GLDATA_TEXCOORDS, "texture", uvs);
    
    // The vertices above are arranged into a set of triangles.
    cone->setDrawType(GL_TRIANGLE_STRIP, verts.size());  
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
            uvs.push_back(glm::vec2(static_cast<float>(j)/thetaTesselation, -static_cast<float>(i + 1)/heightTesselation));

            // Bottom vertex position
            verts.push_back(glm::vec4(r * std::cos(-thetaStep * j), heightStep * i - r, r * std::sin(-thetaStep * j), 1.0f));

            // Bottom vertex normal
            normals.push_back(normal);

            // Bottom UV
            uvs.push_back(glm::vec2(static_cast<float>(j)/thetaTesselation, -static_cast<float>(i)/heightTesselation));

            //Colors
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
    
    // The vertices above are arranged into a set of triangles.
    _body->setDrawType(GL_TRIANGLE_STRIP, verts.size());

    drawableCircle::getCircle(_top, _theta, _height, 1, r, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    drawableCircle::getCircle(_base, _theta, _height, -1, -r, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

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

  
}
