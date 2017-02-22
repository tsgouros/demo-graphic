 
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


std::string textFileRead(std::string fileName) {

  std::string shaderCode;

  // Read the shader code from the file
  std::ifstream shaderStream(fileName.c_str(), std::ios::in);
  if (shaderStream.is_open()) {
    std::string line = "";
    
    while(getline(shaderStream, line)) shaderCode += "\n" + line;
    
    shaderStream.close();
    
  } else {
    throw std::runtime_error("Cannot open: " + fileName);
  }

  return shaderCode;
}

// Data for drawing Axis
float verticesAxis[] = {-20.0f, 0.0f, 0.0f, 1.0f,
                         20.0f, 0.0f, 0.0f, 1.0f,
 
                        0.0f, -20.0f, 0.0f, 1.0f,
                        0.0f,  20.0f, 0.0f, 1.0f,
 
                        0.0f, 0.0f, -20.0f, 1.0f,
                        0.0f, 0.0f,  20.0f, 1.0f};
 
float colorAxis[] = {   0.0f, 1.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 1.0f, 0.0f,
                        1.0f, 1.0f, 0.0f, 0.0f,
                        1.0f, 1.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, 1.0f, 0.0f,
                        1.0f, 0.0f, 1.0f, 0.0f};
 
// Data for triangle 1
float vertices1[] = {   -3.0f, 0.0f, -5.0f, 1.0f,
                        -1.0f, 0.0f, -5.0f, 1.0f,
                        -2.0f, 2.0f, -5.0f, 1.0f};
 
float colors1[] = { 0.0f, 1.0f, 1.0f, 1.0f,
                    0.0f, 1.0f, 1.0f, 1.0f,
                    0.0f, 1.0f, 1.0f, 1.0f};
 
// Data for triangle 2
float vertices2[] = {   1.0f, 0.0f, -5.0f, 1.0f,
                        3.0f, 0.0f, -5.0f, 1.0f,
                        2.0f, 2.0f, -5.0f, 1.0f};
 
float colors2[] = { 1.0f, 1.0f, 0.0f, 1.0f,
                    1.0f, 1.0f, 0.0f, 1.0f,
                    1.0f, 1.0f, 0.0f, 1.0f};

// Where are we and where are we looking?
float posX, posY, posZ, lookAtX, lookAtY, lookAtZ;
float stepX, stepY, stepZ;

// Shader Names
std::string vertexFileName = "../src/shader.vp";
std::string fragmentFileName = "../src/shader.fp";
 
// Program and Shader Identifiers
GLuint shaderProgramID;
 
// Vertex Attribute Locations
GLuint vertexLoc, colorLoc;
 
// Uniform variable Locations
GLuint projMatrixID, viewMatrixID;
 
// Vertex Array Objects Identifiers
GLuint vao[3];
 
// storage for Matrices
float projMatrix[16];
float viewMatrix[16];
 
// ----------------------------------------------------
// VECTOR STUFF
//
 
// res = a cross b;
void crossProduct( float *a, float *b, float *res) {
 
    res[0] = a[1] * b[2]  -  b[1] * a[2];
    res[1] = a[2] * b[0]  -  b[2] * a[0];
    res[2] = a[0] * b[1]  -  b[0] * a[1];
}
 
// Normalize a vec3
void normalize(float *a) {
 
    float mag = sqrt(a[0] * a[0]  +  a[1] * a[1]  +  a[2] * a[2]);
 
    a[0] /= mag;
    a[1] /= mag;
    a[2] /= mag;
}
 
// ----------------------------------------------------
// MATRIX STUFF
//
 
// sets the square matrix mat to the identity matrix,
// size refers to the number of rows (or columns)
void setIdentityMatrix( float *mat, int size) {
 
    // fill matrix with 0s
    for (int i = 0; i < size * size; ++i)
            mat[i] = 0.0f;
 
    // fill diagonal with 1s
    for (int i = 0; i < size; ++i)
        mat[i + i * size] = 1.0f;
}
 
//
// a = a * b;
//
void multMatrix(float *a, float *b) {
 
    float res[16];
 
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res[j*4 + i] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                res[j*4 + i] += a[k*4 + i] * b[j*4 + k];
            }
        }
    }
    memcpy(a, res, 16 * sizeof(float));
 
}
 
// Defines a transformation matrix mat with a translation
void setTranslationMatrix(float *mat, float x, float y, float z) {
 
    setIdentityMatrix(mat,4);

    mat[12] = x;
    mat[13] = y;
    mat[14] = z;
}
 
// ----------------------------------------------------
// Projection Matrix
//
 
void buildProjectionMatrix(float fov, float ratio, float nearP, float farP) {
 
    float f = 1.0f / tan (fov * (M_PI / 360.0));
 
    setIdentityMatrix(projMatrix,4);

    
    projMatrix[0] = f / ratio;
    projMatrix[1 * 4 + 1] = f;
    projMatrix[2 * 4 + 2] = (farP + nearP) / (nearP - farP);
    projMatrix[3 * 4 + 2] = (2.0f * farP * nearP) / (nearP - farP);
    projMatrix[2 * 4 + 3] = -1.0f;
    projMatrix[3 * 4 + 3] = 0.0f;
}
 
// ----------------------------------------------------
// View Matrix
//
// note: it assumes the camera is not tilted,
// i.e. a vertical up vector (remmeber gluLookAt?)
//
 
void setCamera(float posX, float posY, float posZ,
               float lookAtX, float lookAtY, float lookAtZ) {
 
    float dir[3], right[3], up[3];
 
    up[0] = 0.0f;   up[1] = 1.0f;   up[2] = 0.0f;
 
    dir[0] =  (lookAtX - posX);
    dir[1] =  (lookAtY - posY);
    dir[2] =  (lookAtZ - posZ);
    normalize(dir);
 
    crossProduct(dir, up, right);
    normalize(right);
 
    crossProduct(right, dir, up);
    normalize(up);
 
    float aux[16];
 
    viewMatrix[0]  = right[0];
    viewMatrix[4]  = right[1];
    viewMatrix[8]  = right[2];
    viewMatrix[12] = 0.0f;
 
    viewMatrix[1]  = up[0];
    viewMatrix[5]  = up[1];
    viewMatrix[9]  = up[2];
    viewMatrix[13] = 0.0f;
 
    viewMatrix[2]  = -dir[0];
    viewMatrix[6]  = -dir[1];
    viewMatrix[10] = -dir[2];
    viewMatrix[14] =  0.0f;
 
    viewMatrix[3]  = 0.0f;
    viewMatrix[7]  = 0.0f;
    viewMatrix[11] = 0.0f;
    viewMatrix[15] = 1.0f;
 
    setTranslationMatrix(aux, -posX, -posY, -posZ);
 
    multMatrix(viewMatrix, aux);
}
 
// ----------------------------------------------------
 
void changeSize(int w, int h) {
 
    float ratio;
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0) h = 1;
 
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);
 
    ratio = (1.0f * w) / h;
    buildProjectionMatrix(53.13f, ratio, 0.1f, 100.0f);
}
 
void setupBuffers() {
 
    GLuint buffers[2];

    glGenVertexArrays(3, vao);
    //
    // VAO for first triangle
    //
    glBindVertexArray(vao[0]);
    // Generate two slots for the vertex and color buffers
    glGenBuffers(2, buffers);
    // bind buffer for vertices and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vertexLoc);
    glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);
 
    // bind buffer for colors and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors1), colors1, GL_STATIC_DRAW);
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);
 
    //
    // VAO for second triangle
    //
    glBindVertexArray(vao[1]);
    // Generate two slots for the vertex and color buffers
    glGenBuffers(2, buffers);
 
    // bind buffer for vertices and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vertexLoc);
    glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);
 
    // bind buffer for colors and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors2), colors2, GL_STATIC_DRAW);
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);
 
    //
    // This VAO is for the Axis
    //
    glBindVertexArray(vao[2]);
    // Generate two slots for the vertex and color buffers
    glGenBuffers(2, buffers);
    // bind buffer for vertices and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAxis), verticesAxis, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vertexLoc);
    glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);
 
    // bind buffer for colors and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorAxis), colorAxis, GL_STATIC_DRAW);
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);
 
}
 
void setUniforms() {
 
    // must be called after glUseProgram
    glUniformMatrix4fv(projMatrixID,  1, false, projMatrix);
    glUniformMatrix4fv(viewMatrixID,  1, false, viewMatrix);
}
 
void renderScene(void) {
 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    //    setCamera(20, 2, 20, 0, 2, -5);
    setCamera(posX, posY, posZ, lookAtX, lookAtY, lookAtZ);
 
    glUseProgram(shaderProgramID);
    setUniforms();
 
    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
 
    glBindVertexArray(vao[1]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
 
    glBindVertexArray(vao[2]);
    glDrawArrays(GL_LINES, 0, 6);
 
    glutSwapBuffers();
}
 
void processNormalKeys(unsigned char key, int x, int y) {

  //std::cout << "key:" << key << " x:" << x << " y:" << y << std::endl;
  switch (key) {
  case 27:
    glDeleteVertexArrays(3, &vao[0]);
    glDeleteProgram(shaderProgramID);
    exit(0);

    // These next few are for steering the position of the viewer.
  case 'a':
    posX -= stepX;
    break;
  case 'q':
    posX += stepX;
    break;
  case 's':
    posY -= stepY;
    break;
  case 'w':
    posY += stepY;
    break;
  case 'd':
    posZ -= stepZ;
    break;
  case 'e':
    posZ += stepZ;
    break;

    // These next are for steering the position of where you're looking.
  case 'j':
    lookAtX -= stepX;
    break;
  case 'u':
    lookAtX += stepX;
    break;
  case 'k':
    lookAtY -= stepY;
    break;
  case 'i':
    lookAtY += stepY;
    break;
  case 'l':
    lookAtZ -= stepZ;
    break;
  case 'o':
    lookAtZ += stepZ;
    break;
  }
}
 
void printShaderInfoLog(GLuint obj) {
  
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
    GLint result = GL_FALSE;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &result);
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
 
    if (infologLength > 0)
    {
      infoLog = (char *)malloc(infologLength);
      glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
      std::cout << std::string(infoLog) << std::endl;
      free(infoLog);
    }
}
 
void printProgramInfoLog(GLuint obj) {
  
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        std::cout << std::string(infoLog) << std::endl;
        free(infoLog);
    }
}
 
GLuint setupShaders() {
 
    GLuint shaderProgramID, vertShaderID, fragShaderID;
 
    vertShaderID = glCreateShader(GL_VERTEX_SHADER);
    fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
 
    std::string vs = textFileRead(vertexFileName.c_str());
    std::string fs = textFileRead(fragmentFileName.c_str());
 
    const char * vv = vs.c_str();
    const char * ff = fs.c_str();
 
    glShaderSource(vertShaderID, 1, &vv, NULL);
    glShaderSource(fragShaderID, 1, &ff, NULL);
 
    glCompileShader(vertShaderID);
    glCompileShader(fragShaderID);
 
    printShaderInfoLog(vertShaderID);
    printShaderInfoLog(fragShaderID);
 
    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID,vertShaderID);
    glAttachShader(shaderProgramID,fragShaderID);
 
    //    glBindFragDataLocation(shaderProgramID, 0, "outputF");
    glLinkProgram(shaderProgramID);
    printProgramInfoLog(shaderProgramID);
 
    vertexLoc = glGetAttribLocation(shaderProgramID,"position");
    colorLoc = glGetAttribLocation(shaderProgramID, "color"); 
 
    projMatrixID = glGetUniformLocation(shaderProgramID, "projMatrix");
    viewMatrixID = glGetUniformLocation(shaderProgramID, "viewMatrix");
 
    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);

    return(shaderProgramID);
}
 
int main(int argc, char **argv) {

  // Initialize position:
  posX = 20.0f;   posY = 2.0f;     posZ = 20.0f;
  lookAtX = 0.0f; lookAtY = 2.0f;  lookAtZ = -5.0f;
  stepX = 0.1f;   stepY = 0.1f;    stepZ = 0.1f;  

  glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(320,320);
    glutCreateWindow("OpenGL Demo");
 
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processNormalKeys);

    std::cout << glGetString(GL_RENDERER)  // e.g. Intel 3000 OpenGL Engine
              << glGetString(GL_VERSION)    // e.g. 3.2 INTEL-8.0.61
              << std::endl;

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
      throw std::runtime_error("Failed to initialize GLEW");
    }

    if (glewIsSupported("GL_VERSION_2_1"))
        printf("Ready for OpenGL 2.1\n");
    else {
        printf("OpenGL 2.1 not supported\n");
        exit(1);
    }
 
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1 , 0.0, 0.7, 1.0);
 
    shaderProgramID = setupShaders(); 
    setupBuffers(); 
 
    glutMainLoop();
 
    return(0); 
}
