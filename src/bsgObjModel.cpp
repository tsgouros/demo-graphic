#include "bsgObjModel.h"

namespace bsg {

  drawableObjModel::drawableObjModel(bsgPtr<shaderMgr> pShader, const std::string& fileName) :
    drawableCompound(pShader), _fileName(fileName){

    std::vector<float> vert_list;
    std::vector<float> normal_list;
    std::vector<float> uv_list;
    std::vector<int> front_face_list;
    std::vector<int> back_face_list;

    std::ifstream fileObject(_fileName.c_str(), std::ios::in);
    std::string fileObjectLine;

    if (fileObject.is_open()) {
        while (! fileObject.eof() ){
            getline(fileObject, fileObjectLine);
            //std::cout << fileObjectLine << std::endl;
            if (fileObjectLine.c_str()[0] == 'v'){
                if (fileObjectLine.c_str()[1] == ' '){

                    float x, y, z;
                    fileObjectLine[0] = ' ';						
                    sscanf(fileObjectLine.c_str(),"%f %f %f ", &x, &y, &z);		
                    vert_list.push_back(x);						
                    vert_list.push_back(y);						
                    vert_list.push_back(z);
                }

                if (fileObjectLine.c_str()[1] == 'n'){
                    float nx, ny, nz;
                    fileObjectLine[0] = ' ';
                    fileObjectLine[1] = ' ';
                    sscanf(fileObjectLine.c_str(),"%f %f %f ", &nx, &ny, &nz);		
                    normal_list.push_back(nx);						
                    normal_list.push_back(ny);
                    normal_list.push_back(nz);
                }

                if (fileObjectLine.c_str()[1] == 't'){
                    float u, v;
                    fileObjectLine[0] = ' ';
                    fileObjectLine[1] = ' ';
                    sscanf(fileObjectLine.c_str(),"%f %f", &u, &v);
                    uv_list.push_back(u);
                    uv_list.push_back(v);
                }

            }
            if (fileObjectLine.c_str()[0] == 'f'){				
                int v1, vn1=0, vt1=0, v2, vn2=0, vt2=0, v3, vn3=0, vt3, v4=0, vn4=0, vt4=0;
                fileObjectLine[0] = ' ';
                std::size_t numSlash = std::count(fileObjectLine.begin(), fileObjectLine.end(), '/');
		std::size_t doubleSlash = fileObjectLine.find("//");

		if (numSlash == 3) {
			sscanf(fileObjectLine.c_str(),"%d/%d %d/%d %d/%d", &v1, &vt1, &v2, &vt2, &v3, &vt3);
		} else if (numSlash == 4) {
			sscanf(fileObjectLine.c_str(),"%d/%d %d/%d %d/%d %d/%d", &v1, &vt1, &v2, &vt2, &v3, &vt3, &v4, &vt4);
		} else if (numSlash == 6) {
			if (doubleSlash == -1){
				sscanf(fileObjectLine.c_str(),"%d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
			} else {
				sscanf(fileObjectLine.c_str(),"%d//%d %d//%d %d//%d", &v1, &vn1, &v2, &vn2, &v3, &vn3);
			}
		} else if (numSlash == 8) {
			if (doubleSlash == -1){
				sscanf(fileObjectLine.c_str(),"%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3, &v4, &vt4, &vn4);
			} else {
				sscanf(fileObjectLine.c_str(),"%d//%d %d//%d %d//%d %d//%d", &v1, &vn1, &v2, &vn2, &v3, &vn3, &v4, &vn4);
			}
		}		
                		
                front_face_list.push_back(v1-1);				
                front_face_list.push_back(vn1-1);					
                front_face_list.push_back(vt1-1);
                front_face_list.push_back(v2-1);					
                front_face_list.push_back(vn2-1);					
                front_face_list.push_back(vt2-1);
                front_face_list.push_back(v3-1);					
                front_face_list.push_back(vn3-1);				
                front_face_list.push_back(vt3-1);

		back_face_list.push_back(v1-1);					
                back_face_list.push_back(vn1-1);					
                back_face_list.push_back(vt1-1);
                back_face_list.push_back(v3-1);						
                back_face_list.push_back(vn3-1);			
                back_face_list.push_back(vt3-1);
                back_face_list.push_back(v2-1);				
                back_face_list.push_back(vn2-1);				
                back_face_list.push_back(vt2-1);

		if (numSlash == 4 || numSlash == 8) {
			front_face_list.push_back(v1-1);				
		        front_face_list.push_back(vn1-1);			
		        front_face_list.push_back(vt1-1);
		        front_face_list.push_back(v3-1);			
		        front_face_list.push_back(vn3-1);		
		        front_face_list.push_back(vt3-1);
		        front_face_list.push_back(v4-1);			
		        front_face_list.push_back(vn4-1);				
		        front_face_list.push_back(vt4-1);

			back_face_list.push_back(v1-1);				
		        back_face_list.push_back(vn1-1);				
		        back_face_list.push_back(vt1-1);
		        back_face_list.push_back(v4-1);					
		        back_face_list.push_back(vn4-1);		
		        back_face_list.push_back(vt4-1);
		        back_face_list.push_back(v3-1);				
		        back_face_list.push_back(vn3-1);		
		        back_face_list.push_back(vt3-1);
		}
            }
        }
    }

    int nEntries = front_face_list.size()/3;

    std::vector<glm::vec4> frontFaceVertices = std::vector<glm::vec4>(nEntries);
    std::vector<glm::vec4> frontFaceColors = std::vector<glm::vec4>(nEntries);
    std::vector<glm::vec4> frontFaceNormals = std::vector<glm::vec4>(nEntries);
    std::vector<glm::vec2> frontFaceUVs = std::vector<glm::vec2>(nEntries);
    std::vector<glm::vec4> backFaceVertices = std::vector<glm::vec4>(nEntries);
    std::vector<glm::vec4> backFaceColors = std::vector<glm::vec4>(nEntries);
    std::vector<glm::vec4> backFaceNormals = std::vector<glm::vec4>(nEntries);
    std::vector<glm::vec2> backFaceUVs = std::vector<glm::vec2>(nEntries);

    for(int i=0; i < front_face_list.size(); i+=3) {
	
        int iv = front_face_list[i]*3;
	frontFaceVertices[i/3] = glm::vec4(vert_list[iv], vert_list[iv+1], vert_list[iv+2], 1.0f);
	iv = back_face_list[i]*3;

	backFaceVertices[i/3] = (glm::vec4(vert_list[iv], vert_list[iv+1], vert_list[iv+2], 1.0f));

        int ivn = front_face_list[i+1]*3;
	if (ivn > -1) {        
		frontFaceNormals[i/3] = (glm::vec4(normal_list[ivn], normal_list[ivn+1], normal_list[ivn+2], 1.0f));
		ivn = back_face_list[i+1]*3;
		backFaceNormals[i/3] = (glm::vec4(-normal_list[ivn], -normal_list[ivn+1], -normal_list[ivn+2], 1.0f));
	} else {
		if (i%9 == 6) {
			glm::vec3 a = glm::vec3(frontFaceVertices[(i/3)-2]) - glm::vec3(frontFaceVertices[(i/3)-1]);
			glm::vec3 b = glm::vec3(frontFaceVertices[(i/3)-2]) - glm::vec3(frontFaceVertices[(i/3)]);
			glm::vec3 normal = glm::normalize(glm::cross(a,b));
			glm::vec4 n4 = glm::vec4(normal, 1.0f);
			frontFaceNormals[i/3-2] = (n4);
			frontFaceNormals[i/3-1] = (n4);
			frontFaceNormals[i/3] = (n4);
			backFaceNormals[i/3-2] = (-n4);
			backFaceNormals[i/3-1] = (-n4);
			backFaceNormals[i/3] = (-n4);
		}
	}

	frontFaceColors[i/3] = (glm::vec4(1.0f, 0.2f, 0.2f, 1.0f));
	backFaceColors[i/3] = (glm::vec4(0.2f, 1.0f, 0.2f, 1.0f));

        int ivt = front_face_list[i+2]*2;
        frontFaceUVs[i/3] = (glm::vec2(uv_list[ivt], uv_list[ivt+1]));
	ivt = back_face_list[i+2]*2;
	backFaceUVs[i/3] = (glm::vec2(uv_list[ivt], uv_list[ivt+1]));
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

      addObject(_frontFace);
      addObject(_backFace);

  }
}
