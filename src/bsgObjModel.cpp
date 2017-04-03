#include "bsgObjModel.h"

namespace bsg {

drawableObjModel::drawableObjModel(bsgPtr<shaderMgr> pShader,
                                   const std::string &fileName)
    : drawableCompound(pShader), _fileName(fileName) {
  std::vector<glm::vec4> vert_list;
  std::vector<glm::vec4> normal_list;
  std::vector<glm::vec2> uv_list;
  std::map<std::string, material> materialLib;
  std::vector<std::string> face_materials;
  std::vector<std::vector<int> > face_list;

  std::ifstream fileObject(_fileName.c_str(), std::ios::in);
  std::string fileObjectLine;
  std::vector<std::string> lineTokens;
  std::string lineType;
  const char vertexGroupSeparator = ' ';
  const char vertexFieldSeparator = '/';

  int matIndex = 0;
  face_list.push_back(std::vector<int>());
  face_materials.push_back("internal_default");

  std::string fileParentDir = fileName;
  const size_t last_slash_idx = fileParentDir.find_last_of("\\/");
  if (std::string::npos != last_slash_idx)
  {
      fileParentDir.erase(last_slash_idx + 1, fileParentDir.size());
  }


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
      } else if (lineType.compare("mtllib") == 0) {
	  // Reading the specified material library file
          // Multiple .mtl files are allowed, all materials are merged into the materialLib map
          std::map<std::string, material> tempMtlLib = readMtlFile(fileParentDir, lineTokens[1]);
          materialLib.insert(tempMtlLib.begin(), tempMtlLib.end());
      } else if (lineType.compare("usemtl") == 0) {
	// The material for the upcoming triangles has changed
        // Starting a new face_list entry 
        face_materials.push_back(lineTokens[1]);
        face_list.push_back(std::vector<int>());
        matIndex++;
      } else if (lineType.compare("o") == 0) {
          //Named objects, currently not implemented
      } else if (lineType.compare("s") == 0) {
          //Shading groups, currently not implemented
      }
    }
  }

  // File parsing complete

  // "Unpack" indices into Vertex Buffers to load them into graphics memory

  for (matIndex = 0; matIndex < face_materials.size(); matIndex++) {
  int nEntries = face_list[matIndex].size() / 3;

  if (nEntries > 0) {
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
  if(materialLib.count(face_materials[matIndex]) == 0 && !face_materials[matIndex].compare("internal_default") == 0) {
	// The material was not present in the materialLibrary, using the internal default 
	std::cout << "Material \"" << face_materials[matIndex] << "\" was not found, using default material" << std::endl;
        std::cout << "Check the mtllib entry of the \"" << fileName << "\" file and its associated .mtl files for errors" << std::endl;
  }
  _frontFace.addMaterial(materialLib[face_materials[matIndex]]);

  /*_backFace.addData(bsg::GLDATA_VERTICES, "position", backFaceVertices);
  _backFace.addData(bsg::GLDATA_COLORS, "color", backFaceColors);
  _backFace.addData(bsg::GLDATA_NORMALS, "normal", backFaceNormals);
  _backFace.addData(bsg::GLDATA_TEXCOORDS, "texture", backFaceUVs);
  _backFace.setDrawType(GL_TRIANGLES, backFaceVertices.size());
  _backFace.addMaterial(materialLib[face_materials[matIndex]]);*/

  addObject(_frontFace);
  //addObject(_backFace);
  }
  }
}


std::map<std::string, material> drawableObjModel::readMtlFile(const std::string &mtlFileDir, const std::string &mtlFileName) {
    std::ifstream fileObject((mtlFileDir + mtlFileName).c_str(), std::ios::in);
    std::string fileObjectLine;
    std::vector<std::string> lineTokens;
    std::string lineType;

    std::map<std::string, material> mtlLib;
    material currentMaterial;

    textureMgr texLoader;

    if (fileObject.is_open()) {
      while (!fileObject.eof()) {
        getline(fileObject, fileObjectLine);

        // Split the line into segments, using whitespace as separator
        lineTokens = split(fileObjectLine.c_str(), ' ');

        // skip empty lines
        if (lineTokens.size() == 0) {
          continue;
        }

        // The first token defines the line type (e.g. "v", "vn", etc.)
        lineType = lineTokens[0];

        if (lineType.compare("newmtl") == 0) {
            // start of a new material block
            // write previous material into map
            if (!currentMaterial.name.empty()) {
                mtlLib[currentMaterial.name] = currentMaterial;
            }
            currentMaterial = material();
            currentMaterial.name = lineTokens[1];

        } else if (lineType.compare("Ka") == 0) {
	    // parsing ambient color entry
            if (lineTokens[1].compare("spectral") != 0 && lineTokens[1].compare("xyz") != 0 ) {
                float r, g, b;
                sscanf(lineTokens[1].c_str(), "%f", &r);
                sscanf(lineTokens[2].c_str(), "%f", &g);
                sscanf(lineTokens[3].c_str(), "%f", &b);
                currentMaterial.colorAmbient = glm::vec3(r, g, b);
            }

        } else if (lineType.compare("Kd") == 0) {
	    // parsing diffuse color entry
            if (lineTokens[1].compare("spectral") != 0 && lineTokens[1].compare("xyz") != 0 ) {
                float r, g, b;
                sscanf(lineTokens[1].c_str(), "%f", &r);
                sscanf(lineTokens[2].c_str(), "%f", &g);
                sscanf(lineTokens[3].c_str(), "%f", &b);
                currentMaterial.colorDiffuse = glm::vec3(r, g, b);
            }

        } else if (lineType.compare("Ks") == 0) {
            // parsing specular color entry
            if (lineTokens[1].compare("spectral") != 0 && lineTokens[1].compare("xyz") != 0 ) {
                float r, g, b;
                sscanf(lineTokens[1].c_str(), "%f", &r);
                sscanf(lineTokens[2].c_str(), "%f", &g);
                sscanf(lineTokens[3].c_str(), "%f", &b);
                currentMaterial.colorSpecular = glm::vec3(r, g, b);
            }
        } else if (lineType.compare("Ns") == 0) {
            // parsing specular exponent entry
            float factor;
            sscanf(lineTokens[1].c_str(), "%f", &factor);
            currentMaterial.specularExp = factor;
        } else if (lineType.compare("d") == 0) {
            // parsing opacity entry
            float opacity;
            sscanf(lineTokens[1].c_str(), "%f", &opacity);
            currentMaterial.opacity = opacity;
        } else if (lineType.compare("map_Ka") == 0) {
            // loading ambient color texture and storing textureID
            // texture options ("-<option> <value>") are currently not supported
            for (std::vector<std::string>::iterator it = lineTokens.begin() + 1 ; it != lineTokens.end(); ++it) {
                if (it->c_str()[0] == '-') {
                    ++it;
                } else {
                    texLoader.readFile((mtlFileDir + *(it)).c_str());
                    currentMaterial.textureIDAmbient = texLoader.getTextureID();
                    break;
                }
            }
        } else if (lineType.compare("map_Kd") == 0) {
            // loading diffuse color texture and storing textureID
            // texture options ("-<option> <value>") are currently not supported
            for (std::vector<std::string>::iterator it = lineTokens.begin() + 1 ; it != lineTokens.end(); ++it) {
                if (it->c_str()[0] == '-') {
                    ++it;
                } else {
                    texLoader.readFile((mtlFileDir + *(it)).c_str());
                    currentMaterial.textureIDDiffuse = texLoader.getTextureID();
                    break;
                }
            }
        } else if (lineType.compare("map_Ks") == 0) {
	    // loading specular color texture and storing textureID
            // texture options ("-<option> <value>") are currently not supported
            for (std::vector<std::string>::iterator it = lineTokens.begin() + 1 ; it != lineTokens.end(); ++it) {
                if (it->c_str()[0] == '-') {
                    ++it;
                } else {
                    texLoader.readFile((mtlFileDir + *(it)).c_str());
                    currentMaterial.textureIDSpecular = texLoader.getTextureID();
                    break;
                }
            }
        } else if (lineType.compare("map_Ns") == 0) {
            // loading specular exponent texture and storing textureID
            // texture options ("-<option> <value>") are currently not supported
            for (std::vector<std::string>::iterator it = lineTokens.begin() + 1 ; it != lineTokens.end(); ++it) {
                if (it->c_str()[0] == '-') {
                    ++it;
                } else {
                    texLoader.readFile((mtlFileDir + *(it)).c_str());
                    currentMaterial.textureIDSpecularExp = texLoader.getTextureID();
                    break;
                }
            }
        } else if (lineType.compare("map_d") == 0) {
            // loading opacity texture and storing textureID
            // texture options ("-<option> <value>") are currently not supported
            for (std::vector<std::string>::iterator it = lineTokens.begin() + 1 ; it != lineTokens.end(); ++it) {
                if (it->c_str()[0] == '-') {
                    ++it;
                } else {
                    texLoader.readFile((mtlFileDir + *(it)).c_str());
                    currentMaterial.textureIDOpacity = texLoader.getTextureID();
                    break;
                }
            }
        }
      }
    }

    if (!currentMaterial.name.empty()) {
        // Reached end of file
        // Write the current material to map
        mtlLib[currentMaterial.name] = currentMaterial;
    }
    return mtlLib;
}

std::vector<std::string> drawableObjModel::split(const std::string line,
                                                 const char separator) {
  // Split a string at separator character
  // Empty tokens are removed, and line feed characters '\r' filtered out of the resulting tokens
  std::vector<std::string> out;
  std::string element;
  std::stringstream linestream(line);
  while (std::getline(linestream, element, separator)) {
	if (!element.empty()) {
element.erase(std::remove(element.begin(), element.end(), '\r'), element.end());
    out.push_back(element);
}
  }
  return out;
}
}
