#include "bsgObjModel.h"

namespace bsg {

drawableObjModel::drawableObjModel(bsgPtr<shaderMgr> pShader,
                                   const std::string &fileName)
  : drawableCompound(pShader), _fileName(fileName), _includeBackFace(true) {
  _processObjFile();
}
   
drawableObjModel::drawableObjModel(bsgPtr<shaderMgr> pShader,
                                   const std::string &fileName,
                                   const bool &back)
  : drawableCompound(pShader), _fileName(fileName), _includeBackFace(back) {
  _processObjFile();
}
   
void drawableObjModel::_processObjFile() {

  std::vector<glm::vec4> vert_list;
  std::vector<glm::vec4> normal_list;
  std::vector<glm::vec2> uv_list;
  std::vector<material> materials;
  std::vector<std::vector<int> > face_list;

  std::ifstream fileObject(_fileName.c_str(), std::ios::in);
  std::string fileObjectLine;
  std::vector<std::string> lineTokens;
  std::string lineType;
  const char vertexGroupSeparator = ' ';
  const char vertexFieldSeparator = '/';

  int matIndex =
      0; // placeholder material Index until material parsing is implemented
  face_list.push_back(std::vector<int>());
  
  std::cout << "Processing: " << _fileName;
  if (!_includeBackFace) std::cout << " (front face only)";
  std::cout << " ..." << std::endl;

  if (fileObject.is_open()) {
    while (!fileObject.eof()) {
      getline(fileObject, fileObjectLine);

      // Split the line into segments, using whitespace as separator
      lineTokens = split(fileObjectLine.c_str(), vertexGroupSeparator);

      // skip empty lines
      if (lineTokens.size() == 0) {
        continue;
      }

      // The first token defines the line type (e.g. "v", "vn", etc.)
      lineType = lineTokens[0];

      if (lineType.compare("v") == 0) {

        // Parse an obj vertex location line. Format: "v x y z"
        float x, y, z;

        sscanf(lineTokens[1].c_str(), "%f", &x);
        sscanf(lineTokens[2].c_str(), "%f", &y);
        sscanf(lineTokens[3].c_str(), "%f", &z);

        vert_list.push_back(glm::vec4(x, y, z, 1.0f));

      } else if (lineType.compare("vn") == 0) {
        // Parse an obj vertex normal line. Format: "vn nx ny nz"
        float nx, ny, nz;

        sscanf(lineTokens[1].c_str(), "%f", &nx);
        sscanf(lineTokens[2].c_str(), "%f", &ny);
        sscanf(lineTokens[3].c_str(), "%f", &nz);

        normal_list.push_back(glm::vec4(nx, ny, nz, 1.0f));

      } else if (lineType.compare("vt") == 0) {
        // Parse an obj texture coordinate line. Format: "vt u v"
        float u, v;

        sscanf(lineTokens[1].c_str(), "%f", &u);
        sscanf(lineTokens[2].c_str(), "%f", &v);

        uv_list.push_back(glm::vec2(u, v));

      } else if (lineType.compare("f") == 0) {
        // Parse the indices on an obj face line.
        //"f v1 v2 v3" ("v4" optional)
        //"f v1/vt1 v2/vt2 v3/vt3" ("v4/vt4 ..." optional)
        //"f v1//vn1 v2//vn2 v3//vn3" ("v4//vn4 ..." optional)
        //"f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3" ("v4/vt4/vn4 ..." optional)
        // every primitive will be broken down into triangles

        // Covered primitives are triangles and quads (ignoring other
        // primitives)
        if (lineTokens.size() == 4 || lineTokens.size() == 5) {
          int vIndex, vnIndex, vtIndex;
          std::vector<std::string> faceElementTokens;
          std::size_t numSlash;

          for (int i = 1; i < lineTokens.size(); i++) {
            // Initialize fields with invalid indices for filtering later
            vIndex = 0;
            vnIndex = 0;
            vtIndex = 0;

            // Split the token around the "/" character and count the occurrence
            faceElementTokens =
                split(lineTokens[i].c_str(), vertexFieldSeparator);
            numSlash = std::count(lineTokens[i].begin(), lineTokens[i].end(),
                                  vertexFieldSeparator);

            switch (faceElementTokens.size()) {
            case 1:
              // v1
              sscanf(faceElementTokens[0].c_str(), "%d", &vIndex);
              break;
            case 2:
              if (numSlash == 1) {
                // v1/vt1
                sscanf(faceElementTokens[0].c_str(), "%d", &vIndex);
                sscanf(faceElementTokens[1].c_str(), "%d", &vtIndex);
              } else {
                // v1//vn1
                sscanf(faceElementTokens[0].c_str(), "%d", &vIndex);
                sscanf(faceElementTokens[1].c_str(), "%d", &vnIndex);
              }
              break;
            case 3:
              // v1/vt1/vn1
              sscanf(faceElementTokens[0].c_str(), "%d", &vIndex);
              sscanf(faceElementTokens[1].c_str(), "%d", &vtIndex);
              sscanf(faceElementTokens[2].c_str(), "%d", &vnIndex);
              break;
            }

            // Store all vertex indices in temporary list, until parsing of all
            // lines of the file is complete
            // C++ vectors are zero-indexed, so all obj indizes have to be
            // reduced by one
            if (i < 4) {
              // The current vertex is part of the first triangle of the
              // primitive and can be stored in incoming order
              face_list[matIndex].push_back(vIndex - 1);
              face_list[matIndex].push_back(vtIndex - 1);
              face_list[matIndex].push_back(vnIndex - 1);
            } else {
              // The primitive has more than three vertices, triangulize the
              // primitive as triangle fan
              // using the first, previous and current vertex
              int firstVertexPos = face_list[matIndex].size() - ((i - 1) * 3);
              int previousVertexPos = face_list[matIndex].size() - 3;

              face_list[matIndex].push_back(
                  face_list[matIndex][firstVertexPos]);
              face_list[matIndex].push_back(
                  face_list[matIndex][firstVertexPos + 1]);
              face_list[matIndex].push_back(
                  face_list[matIndex][firstVertexPos + 2]);

              face_list[matIndex].push_back(
                  face_list[matIndex][previousVertexPos]);
              face_list[matIndex].push_back(
                  face_list[matIndex][previousVertexPos + 1]);
              face_list[matIndex].push_back(
                  face_list[matIndex][previousVertexPos + 2]);

              face_list[matIndex].push_back(vIndex - 1);
              face_list[matIndex].push_back(vtIndex - 1);
              face_list[matIndex].push_back(vnIndex - 1);
            }
          }
        }
      }
    }
  }

  // File parsing complete

  // "Unpack" indices into Vertex Buffers to load them into graphics memory

  int nEntries = face_list[matIndex].size() / 3;

  std::vector<glm::vec4> frontFaceVertices = std::vector<glm::vec4>(nEntries);
  std::vector<glm::vec4> frontFaceColors = std::vector<glm::vec4>(nEntries);
  std::vector<glm::vec4> frontFaceNormals = std::vector<glm::vec4>(nEntries);
  std::vector<glm::vec2> frontFaceUVs = std::vector<glm::vec2>(nEntries);
  std::vector<glm::vec4> backFaceVertices = std::vector<glm::vec4>(nEntries);
  std::vector<glm::vec4> backFaceColors = std::vector<glm::vec4>(nEntries);
  std::vector<glm::vec4> backFaceNormals = std::vector<glm::vec4>(nEntries);
  std::vector<glm::vec2> backFaceUVs = std::vector<glm::vec2>(nEntries);

  glm::vec2 genericUV = glm::vec2(0.0f, 0.0f);

  for (int i = 0; i < nEntries / 3; i++) {
    // process every triangle in the face_list

    int j = i * 9; // Start index of the triangle, since every triangle has 9
                   // indices (v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3)

    int writePos =
        i * 3; // Writing position in vertex, color, normal and uv vectors

    if (!(face_list[matIndex][j] < 0 || face_list[matIndex][j + 3] < 0 ||
          face_list[matIndex][j + 6] < 0)) {
      // Only process triangle if all of the vertex coordinate indices are valid
      // (>= 0)

      // Add front-facing triangle
      frontFaceVertices[writePos] = vert_list[face_list[matIndex][j]];
      frontFaceVertices[writePos + 1] = vert_list[face_list[matIndex][j + 3]];
      frontFaceVertices[writePos + 2] = vert_list[face_list[matIndex][j + 6]];

      // Add back-facing triangle by flipping the order of the last two vertices
      backFaceVertices[writePos] = vert_list[face_list[matIndex][j]];
      backFaceVertices[writePos + 1] = vert_list[face_list[matIndex][j + 6]];
      backFaceVertices[writePos + 2] = vert_list[face_list[matIndex][j + 3]];

      if (!(face_list[matIndex][j + 1] < 0 || face_list[matIndex][j + 4] < 0 ||
            face_list[matIndex][j + 7] < 0)) {
        // All texture coordinate indices are valid (>= 0)
        // Add front-facing triangle UVs
        frontFaceUVs[writePos] = uv_list[face_list[matIndex][j + 1]];
        frontFaceUVs[writePos + 1] = uv_list[face_list[matIndex][j + 4]];
        frontFaceUVs[writePos + 2] = uv_list[face_list[matIndex][j + 7]];

        // Add back-facing triangle UVs by flipping the order of the last two
        // vertices
        backFaceUVs[writePos] = uv_list[face_list[matIndex][j + 1]];
        backFaceUVs[writePos + 1] = uv_list[face_list[matIndex][j + 7]];
        backFaceUVs[writePos + 2] = uv_list[face_list[matIndex][j + 4]];
      } else {
        // At least one texture coordinate index was invalid, store generic
        // texture coordinates
        frontFaceUVs[writePos] = genericUV;
        frontFaceUVs[writePos + 1] = genericUV;
        frontFaceUVs[writePos + 2] = genericUV;
        backFaceUVs[writePos] = genericUV;
        backFaceUVs[writePos + 1] = genericUV;
        backFaceUVs[writePos + 2] = genericUV;
      }

      if (!(face_list[matIndex][j + 2] < 0 || face_list[matIndex][j + 5] < 0 ||
            face_list[matIndex][j + 8] < 0)) {
        // All normal indices are valid (>= 0)
        // Add front-facing triangle normals
        frontFaceNormals[writePos] = normal_list[face_list[matIndex][j + 2]];
        frontFaceNormals[writePos + 1] =
            normal_list[face_list[matIndex][j + 5]];
        frontFaceNormals[writePos + 2] =
            normal_list[face_list[matIndex][j + 8]];

        // Add back-facing triangle normals by negating front facing ones
        backFaceNormals[writePos] = -normal_list[face_list[matIndex][j + 2]];
        backFaceNormals[writePos + 1] =
            -normal_list[face_list[matIndex][j + 5]];
        backFaceNormals[writePos + 2] =
            -normal_list[face_list[matIndex][j + 8]];
      } else {
        // At least one normal index was invalid, calculate face normal from
        // vertex data
        glm::vec3 a = glm::vec3(frontFaceVertices[writePos]) -
                      glm::vec3(frontFaceVertices[writePos + 1]);
        glm::vec3 b = glm::vec3(frontFaceVertices[writePos]) -
                      glm::vec3(frontFaceVertices[writePos + 2]);
        glm::vec3 n = glm::normalize(glm::cross(a, b));
        glm::vec4 faceNormal = glm::vec4(n, 1.0f);

        frontFaceNormals[writePos] = faceNormal;
        frontFaceNormals[writePos + 1] = faceNormal;
        frontFaceNormals[writePos + 2] = faceNormal;

        // Add back-facing triangle normals by negating front facing ones
        backFaceNormals[writePos] = -faceNormal;
        backFaceNormals[writePos + 1] = -faceNormal;
        backFaceNormals[writePos + 2] = -faceNormal;
      }
    }
  }

  _frontFace.addData(bsg::GLDATA_VERTICES, "position", frontFaceVertices);
  _frontFace.addData(bsg::GLDATA_COLORS, "color", frontFaceColors);
  _frontFace.addData(bsg::GLDATA_NORMALS, "normal", frontFaceNormals);
  _frontFace.addData(bsg::GLDATA_TEXCOORDS, "texture", frontFaceUVs);
  _frontFace.setDrawType(GL_TRIANGLES, frontFaceVertices.size());

  _backFace.addData(bsg::GLDATA_VERTICES, "position", backFaceVertices);
  _backFace.addData(bsg::GLDATA_COLORS, "color", backFaceColors);
  _backFace.addData(bsg::GLDATA_NORMALS, "normal", backFaceNormals);
  _backFace.addData(bsg::GLDATA_TEXCOORDS, "texture", backFaceUVs);
  _backFace.setDrawType(GL_TRIANGLES, backFaceVertices.size());

  std::cout << "numVertices:" << frontFaceVertices.size() << " numColors:" << frontFaceColors.size() << std::endl;


  _frontFace.setInterleaved(true);
  addObject(_frontFace);

  if (_includeBackFace) {
    _backFace.setInterleaved(true);
    addObject(_backFace);
  }

  std::cout << "... " << _fileName << " done." << std::endl;
}

std::vector<std::string> drawableObjModel::split(const std::string line,
                                                 const char separator) {
  std::vector<std::string> out;
  std::string element;
  std::stringstream linestream(line);
  while (std::getline(linestream, element, separator)) {
    out.push_back(element);
  }
  return out;
}
}
