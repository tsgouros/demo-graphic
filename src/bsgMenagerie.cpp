#include "bsgMenagerie.h"

namespace bsg {

  drawableRectangle::drawableRectangle(bsgPtr<shaderMgr> pShader,
                                       const float width, const float height,
                                       const int nDivs) :
    drawableCompound(pShader), _height(height), _width(width) {

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
      
      _frontFace.addData(bsg::GLDATA_VERTICES, "position", frontFaceVertices);
      _frontFace.addData(bsg::GLDATA_COLORS, "color", frontFaceColors);
      _frontFace.addData(bsg::GLDATA_NORMALS, "normal", frontFaceNormals);
      _frontFace.addData(bsg::GLDATA_TEXCOORDS, "texture", frontFaceUVs);
      _frontFace.setDrawType(GL_TRIANGLE_STRIP);  

      _backFace.addData(bsg::GLDATA_VERTICES, "position", backFaceVertices);
      _backFace.addData(bsg::GLDATA_COLORS, "color", backFaceColors);
      _backFace.addData(bsg::GLDATA_NORMALS, "normal", backFaceNormals);
      _backFace.addData(bsg::GLDATA_TEXCOORDS, "texture", backFaceUVs);
      _backFace.setDrawType(GL_TRIANGLE_STRIP);  

      addObject(_frontFace);
      addObject(_backFace);
    }
  }
    
  drawableRectangle::drawableRectangle(bsgPtr<shaderMgr> pShader,
                                       const float width, const float height) :
    drawableCompound(pShader), _height(height), _width(width) {

    std::vector<glm::vec4> frontFaceVertices;

    float w = _width/2.0f;
    float h = _height/2.0f;

    frontFaceVertices.push_back(glm::vec4( -w, -h, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(  w, -h, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4( -w,  h, 0.0f, 1.0f));
    frontFaceVertices.push_back(glm::vec4(  w,  h, 0.0f, 1.0f));

    _frontFace.addData(bsg::GLDATA_VERTICES, "position", frontFaceVertices);

    // Here are the corresponding colors for the above vertices.
    std::vector<glm::vec4> frontFaceColors;
    frontFaceColors.push_back(glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f));
    frontFaceColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));
    frontFaceColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
    frontFaceColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));

    _frontFace.addData(bsg::GLDATA_COLORS, "color", frontFaceColors);

    std::vector<glm::vec4> frontFaceNormals;

    frontFaceNormals.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f));
    frontFaceNormals.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f));
    frontFaceNormals.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f));
    frontFaceNormals.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f));

    _frontFace.addData(bsg::GLDATA_NORMALS, "normal", frontFaceNormals);

    std::vector<glm::vec2> frontFaceUVs;

    frontFaceUVs.push_back(glm::vec2( 0.0f, 0.0f));
    frontFaceUVs.push_back(glm::vec2( 1.0f, 0.0f));
    frontFaceUVs.push_back(glm::vec2( 0.0f, 1.0f));
    frontFaceUVs.push_back(glm::vec2( 1.0f, 1.0f));

    _frontFace.addData(bsg::GLDATA_TEXCOORDS, "texture", frontFaceUVs);
    
    // The vertices above are arranged into a set of triangles.
    _frontFace.setDrawType(GL_TRIANGLE_STRIP);  

    // Same thing for the other rectangle.
    std::vector<glm::vec4> backFaceVertices;

    backFaceVertices.push_back(glm::vec4( -w, -h, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4( -w,  h, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(  w, -h, 0.0f, 1.0f));
    backFaceVertices.push_back(glm::vec4(  w,  h, 0.0f, 1.0f));

    _backFace.addData(bsg::GLDATA_VERTICES, "position", backFaceVertices);

    // And the corresponding colors for the above vertices.
    std::vector<glm::vec4> backFaceColors;
    backFaceColors.push_back(glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f));
    backFaceColors.push_back(glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f));
    backFaceColors.push_back(glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f));
    backFaceColors.push_back(glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f));

    _backFace.addData(bsg::GLDATA_COLORS, "color", backFaceColors);

    std::vector<glm::vec4> backFaceNormals;

    backFaceNormals.push_back(glm::vec4( 0.0f, 0.0f,-1.0f, 0.0f));
    backFaceNormals.push_back(glm::vec4( 0.0f, 0.0f,-1.0f, 0.0f));
    backFaceNormals.push_back(glm::vec4( 0.0f, 0.0f,-1.0f, 0.0f));
    backFaceNormals.push_back(glm::vec4( 0.0f, 0.0f,-1.0f, 0.0f));

    _backFace.addData(bsg::GLDATA_NORMALS, "normal", backFaceNormals);

    std::vector<glm::vec2> backFaceUVs;

    backFaceUVs.push_back(glm::vec2( 0.0f, 0.0f));
    backFaceUVs.push_back(glm::vec2( 0.0f, 1.0f));
    backFaceUVs.push_back(glm::vec2( 1.0f, 0.0f));
    backFaceUVs.push_back(glm::vec2( 1.0f, 1.0f));

    _backFace.addData(bsg::GLDATA_TEXCOORDS, "texture", backFaceUVs);
    
    // The vertices above are arranged into a set of triangles.
    _backFace.setDrawType(GL_TRIANGLE_STRIP);  

    addObject(_frontFace);
    addObject(_backFace);
  }

  drawableAxes::drawableAxes(bsgPtr<shaderMgr> pShader, const float length) :
    drawableCompound(pShader), _length(length) {
    
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

    _axes.addData(bsg::GLDATA_VERTICES, "position", axesVertices);
    _axes.addData(bsg::GLDATA_COLORS, "color", axesColors);

    // The axes are not triangles, but lines.
    _axes.setDrawType(GL_LINES);

    addObject(_axes);
  }
  
}
