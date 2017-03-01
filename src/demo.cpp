 
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
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
float verticesAxis[] = {  -20.0f, 0.0f, 0.0f, 1.0f, // The X axis
                          20.0f, 0.0f, 0.0f, 1.0f,

                          0.0f, -20.0f, 0.0f, 1.0f, // The Y axis
                          0.0f,  20.0f, 0.0f, 1.0f,

                          0.0f, 0.0f, -20.0f, 1.0f, // The Z axis
                          0.0f, 0.0f,  20.0f, 1.0f};
 
float colorAxis[] = {  1.0f, 1.0f, 0.0f, 0.0f,
                       1.0f, 0.0f, 0.0f, 0.0f,
                       
                       0.0f, 1.0f, 1.0f, 0.0f,
                       0.0f, 1.0f, 0.0f, 0.0f,
                       
                       1.0f, 0.0f, 1.0f, 0.0f,
                       0.0f, 0.0f, 1.0f, 0.0f};
 
// Data for triangle 1
float vertices1[] = { -3.0f, 0.0f, -5.0f, 1.0f,
                      -1.0f, 0.0f, -5.0f, 1.0f,
                      -2.0f, 2.0f, -5.0f, 1.0f};
 
float colors1[] = { 0.0f, 1.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 1.0f, 0.0f};
 
// Data for triangle 2
float vertices2[] = { 1.0f, 0.0f, -5.0f, 1.0f,
                      3.0f, 0.0f, -5.0f, 1.0f,
                      2.0f, 2.0f, -5.0f, 1.0f};
 
float colors2[] = { 1.0f, 1.0f, 0.0f, 0.0f,
                    1.0f, 1.0f, 0.0f, 0.0f,
                    1.0f, 1.0f, 0.0f, 0.0f};

// Where we are ...
float posX, posY, posZ;

// And where we're looking.
float lookAtX, lookAtY, lookAtZ;

// The step value for moving the pos or lookAt values.
float stepX, stepY, stepZ;

// Shader file names
std::string vertexFileName = "../src/shader.vp";
std::string fragmentFileName = "../src/shader.fp";
 
// This is where the index numbers for each of the data buffers are
// stored.  They are initialized in the setupBuffers() function below,
// and used in teh renderScene() function.
GLuint buffers[6];

// Program Identifier -- This represents the compiled collection
// of shaders.
GLuint shaderProgramID;
 
// Vertex attribute locations -- These tell OpenGL what variable
// (called "attribute" in glsl-speak) in your shader goes with a
// particular buffer of data.
GLuint vertexID, colorID;
 
// These are IDs of the glsl "uniform" values, used to tell OpenGL
// which one belongs with which matrix in this program.
GLuint projMatrixID, viewMatrixID;
 
// These are the actual matrices.
float projMatrix[16];
float viewMatrix[16];
 
// ---------------------------------------------------------------
// VECTOR STUFF -- You will usually use a matrix math package that
// takes care of all this stuff.  It's here for your edification in
// case you're interested.
//
 
// res = a cross b;
void crossProduct( float *a, float *b, float *res) {
 
  res[0] = a[1] * b[2]  -  b[1] * a[2];
  res[1] = a[2] * b[0]  -  b[2] * a[0];
  res[2] = a[0] * b[1]  -  b[0] * a[1];
}

// Normalize a vec3 -- Produce a unit length vector in the same
// direction as the input vector.
void normalize(float *a) {
 
  float mag = sqrt(a[0] * a[0]  +  a[1] * a[1]  +  a[2] * a[2]);
 
  a[0] /= mag;
  a[1] /= mag;
  a[2] /= mag;
}
 
// ----------------------------------------------------------------
// MATRIX STUFF -- Same as above.  You'll usually use a matrix math
// package for this.
//
 
// Creates a square identity matrix, i.e. a square matrix with 1's on
// the diagonal.  It's an identity matrix because if you multiply it
// by some vector or another matrix, you get the original vector or
// matrix.
void setIdentityMatrix( float *mat, int size) {
  // size refers to the number of rows (or columns)
  
  // fill matrix with 0s
  for (int i = 0; i < size * size; ++i)
    mat[i] = 0.0f;
 
  // fill diagonal with 1s
  for (int i = 0; i < size; ++i)
    mat[i + i * size] = 1.0f;
}
 
// Multiplication of two matrices.
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

// A 'translation' matrix is one that moves a vector in a particular
// direction.  If you define the direction with an (x, y, z), then the
// matrix that moves another vector in that direction is an identity
// matrix,
void setTranslationMatrix(float *mat, float x, float y, float z) {
 
  setIdentityMatrix(mat, 4);

  mat[12] = x;
  mat[13] = y;
  mat[14] = z;
}
 
// --------------------------------------------------------------------
// Projection Matrix -- This is how you build a projection matrix.
// 'fov' stands for 'field of view' and is measured in degrees.
// 'ratio' refers to the aspect ratio of the window the scene is
// displayed in.  The nearClip and farClip distances define planes
// perpendicular to the direction of view.  Objects closer than
// nearClip or farther than farClip will not be displayed.  This is
// applied to the view in the viewer space, also called the camera
// space.
//
void buildProjectionMatrix(float fov, float ratio, float nearClip, float farClip) {
 
  float f = 1.0f / tan (fov * (M_PI / 360.0));
 
  setIdentityMatrix(projMatrix,4);
    
  projMatrix[0] = f / ratio;
  projMatrix[1 * 4 + 1] = f;
  projMatrix[2 * 4 + 2] = (farClip + nearClip) / (nearClip - farClip);
  projMatrix[3 * 4 + 2] = (2.0f * farClip * nearClip) / (nearClip - farClip);
  projMatrix[2 * 4 + 3] = -1.0f;
  projMatrix[3 * 4 + 3] = 0.0f;
}
 
// ---------------------------------------------------------------
// View Matrix -- Defines how we move from object space to viewer
// space, also called 'camera space'.  One way to think of this is
// that it's kind of the same as moving the viewer to the origin and
// rotating the whole world to accommodate the viewer's orientation.
//
// Note: We assume here that the camera is not tilted, and that 'up'
// is in the positive Y direction.
// 
void setCamera(float posX, float posY, float posZ,
               float lookAtX, float lookAtY, float lookAtZ) {
 
    float dir[3], right[3], up[3];

    // Define the 'up' vector.
    up[0] = 0.0f;   up[1] = 1.0f;   up[2] = 0.0f;

    // Figure out the vector that represents the viewer's gaze direction.
    dir[0] =  (lookAtX - posX);
    dir[1] =  (lookAtY - posY);
    dir[2] =  (lookAtZ - posZ);
    normalize(dir);

    // The gaze direction cross the up vector should point to the
    // viewer's right.
    crossProduct(dir, up, right);
    normalize(right);

    // The right vector cross the gaze direction should point up
    // again, but it might be slightly different.
    crossProduct(right, dir, up);
    normalize(up);
 
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

    float transMatrix[16];
    setTranslationMatrix(transMatrix, -posX, -posY, -posZ);
 
    multMatrix(viewMatrix, transMatrix);
}
 
// -----------------------------------------------------------------
// This function is called when the window changes size.  It adjusts
// the projection matrix, and resets an OpenGL value for the size of
// the viewport.
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


// This function sets up each buffer in the buffers array, and loads
// them with data.  This program moves the view of the objects around,
// but doesn't do much in the way of modification of those objects, so
// these buffers can be loaded and then left alone.
void setupBuffers() {

  // Ask OpenGL for some ID numbers for data buffers.
  glGenBuffers(6, buffers);

  // These are the buffers for first triangle.  We use two buffers for
  // it, one for the vertices and the other for the colors.
  
  // First we 'bind' the buffer with the ID, which means that the
  // commands after this call refer to that ID...
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);

  // ... And then we copy data into the bound buffer.
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
 
  // Repeat -- bind buffer and then fill it -- for the colors.
  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors1), colors1, GL_STATIC_DRAW);
 
  //
  // Second triangle, bind and fill, once for the vertices...
  glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
 
  // ... and once for the colors.
  glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors2), colors2, GL_STATIC_DRAW);
 
  //
  //  Same thing with the axis: bind the buffer and then fill it, once
  //  for the vertices, and once for the colors.
  glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAxis), verticesAxis, GL_STATIC_DRAW);
 
  glBindBuffer(GL_ARRAY_BUFFER, buffers[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colorAxis), colorAxis, GL_STATIC_DRAW);
 
}
 
void setUniforms() {
 
  // This function must be called after glUseProgram, which does the
  // same sort of thing as glBindBuffer(): it affects the OpenGL calls
  // after it.
    glUniformMatrix4fv(projMatrixID,  1, false, projMatrix);
    glUniformMatrix4fv(viewMatrixID,  1, false, viewMatrix);
}

// This is the heart of any graphics program, the render function.  It
// is called each time through the main graphics loop, and re-draws
// the scene according to whatever has changed since the last time it
// was drawn.
void renderScene(void) {

  // First clear the display.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Generate a view matrix using the viewer (camera) location and
  // where the viewer is looking.
  setCamera(posX, posY, posZ, lookAtX, lookAtY, lookAtZ);

  // We want to use this program, that was compiled in setupShaders()
  // below.  When we call glUseProgram(), we are affecting a bunch of
  // calls that follow.
  glUseProgram(shaderProgramID);

  // Set the uniforms (in this case, the view and projection matrices)
  // for the shaders that are attached to shaderProgramID.
  setUniforms();

  // Bind a buffer.
  glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
  // This is a buffer with vertex data, which we say with this function.
  glEnableVertexAttribArray(vertexID);
  // This function describes how the data is laid out in the buffer.
  glVertexAttribPointer(vertexID, 4, GL_FLOAT, 0, 0, 0);

  // Now bind, label, and describe the color buffer.
  glBindBuffer(GL_ARRAY_BUFFER, buffers[5]);
  glEnableVertexAttribArray(colorID);
  glVertexAttribPointer(colorID, 4, GL_FLOAT, 0, 0, 0);

  // Now draw the data in the active buffers.  Draw it as a set of lines.
  glDrawArrays(GL_LINES, 0, 6);

  // Repeat the process with another pair of vertex/color buffers.
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glEnableVertexAttribArray(vertexID);
  glVertexAttribPointer(vertexID, 4, GL_FLOAT, 0, 0, 0);
 
  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  glEnableVertexAttribArray(colorID);
  glVertexAttribPointer(colorID, 4, GL_FLOAT, 0, 0, 0);

  // The last two buffers drew one of the triangles, so when we draw
  // it, make sure you're drawing it as a triangle.
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // One more pair of vertex/color buffers.
  glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
  glEnableVertexAttribArray(vertexID);
  glVertexAttribPointer(vertexID, 4, GL_FLOAT, 0, 0, 0);
 
  glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
  glEnableVertexAttribArray(colorID);
  glVertexAttribPointer(colorID, 4, GL_FLOAT, 0, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  // Now swap the image buffers, so the unseen buffer is swapped onto
  // the screen and the image buffer that was on the screen is hidden
  // so it can be drawn into again.
    glutSwapBuffers();
}
 
void processNormalKeys(unsigned char key, int x, int y) {

  // This function processes only the 'normal' keys.  The arrow keys
  // don't appear here, nor mouse events.
  switch (key) {
  case 27:
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

  // Uncomment these to see where you are (where the camera is) and where
  // you're looking.
  // std::cout << "location:(" << posX << ", "
  //           << posY << ", " << posZ << ")" << std::endl; 
  // std::cout << "lookAt:(" << lookAtX << ", "
  //           << lookAtY << ", " << lookAtZ << ")" << std::endl; 

}

// If there are errors when compiling your shader, they are printed here.
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

// If there are errors in your program, this prints them.
void printProgramInfoLog(GLuint obj) {
  
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        std::cout << std::string(infoLog) << std::endl;
        free(infoLog);
    }
}

// This is the meat of the shader management.  Here is where the
// shaders are read in and compiled, and reviewed for the names that
// will link them to the data you'll feed to them.
GLuint setupShaders(std::string vertexFile, std::string fragmentFile) {

  // This is the ID for the whole shader program, which includes both
  // the vertex and fragment shaders.  This is the return value of
  // this function.
  GLuint shaderProgramID;

  // These are IDs for the two shaders we're using.
  GLuint vertShaderID, fragShaderID;

  // Prepare the proper IDs for each kind of shader.
  vertShaderID = glCreateShader(GL_VERTEX_SHADER);
  fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  // Read the contents of the two shader files.
  std::string vs = textFileRead(vertexFile.c_str());
  std::string fs = textFileRead(fragmentFile.c_str());

  // The OpenGL calls don't really like the modern C++ types, so we
  // convert back to old-fashioned char*.
  const char * vv = vs.c_str();
  const char * ff = fs.c_str();

  // Feed the shader source to OpenGL.
  glShaderSource(vertShaderID, 1, &vv, NULL);
  glShaderSource(fragShaderID, 1, &ff, NULL);

  // Compule the shader source.
  glCompileShader(vertShaderID);
  glCompileShader(fragShaderID);

  // If there are any errors, print them.
  printShaderInfoLog(vertShaderID);
  printShaderInfoLog(fragShaderID);

  // Now create a program to contain the two shaders, and attach the
  // shaders to it.
  shaderProgramID = glCreateProgram();
  glAttachShader(shaderProgramID,vertShaderID);
  glAttachShader(shaderProgramID,fragShaderID);

  // Assemble the shaders into a single program with 'link', which
  // will make sure that the inputs to the fragment shader correspond
  // with outputs from the vertex shader, and so on.
  glLinkProgram(shaderProgramID);
  printProgramInfoLog(shaderProgramID);

  // Now that the program has been linked, create links to the
  // locations where these names point.  Data from this program will
  // be 'labeled' with these ID numbers.
  vertexID = glGetAttribLocation(shaderProgramID,"position");
  colorID = glGetAttribLocation(shaderProgramID, "color"); 
 
  projMatrixID = glGetUniformLocation(shaderProgramID, "projMatrix");
  viewMatrixID = glGetUniformLocation(shaderProgramID, "viewMatrix");

  // The shaders are linked into the program, so we can delete the raw
  // shaders.
  glDeleteShader(vertShaderID);
  glDeleteShader(fragShaderID);

  // The return value is the product of all that.
  return(shaderProgramID);
}
 
int main(int argc, char **argv) {

  // Initialize position of the camera.
  posX = -3.5f;   posY = 4.0f;     posZ = -12.5f;

  // Initialize where the camera is pointing (aka where you are looking).
  lookAtX = 0.0f; lookAtY = 2.0f;  lookAtZ = -5.0f;

  // When you press an active key, the coordinate changes by this much.
  stepX = 0.5f;   stepY = 0.5f;    stepZ = 0.5f;  

  // Initialize the graphics display, in all the ways required.
  // You'll often see this as "creating a graphics *context*".  The
  // funny thing about OpenGL is that its functions don't even exist
  // if there is no graphics context.  This means that glBufferData()
  // doesn't merely fail, it pretends not to exist.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

  // Create the window, at this position and with this size, and heading.
  glutInitWindowPosition(100,100);
  glutInitWindowSize(320,320);
  glutCreateWindow("OpenGL Demo");

  // These next few functions tell glut what to do under certain
  // conditions.  This is where the render function (it's called
  // renderScene) is nominated, and where the keyboard handler
  // (processNormalKeys) is, too.
  glutDisplayFunc(renderScene);
  glutIdleFunc(renderScene);
  glutKeyboardFunc(processNormalKeys);
  // This function is called when the user changes the size of the window.
  glutReshapeFunc(changeSize);

  // Now that we have a graphics context, let's look at what's inside.
  std::cout << "Hardware check: "
            << glGetString(GL_RENDERER)  // e.g. Intel 3000 OpenGL Engine
            << " / "
            << glGetString(GL_VERSION)    // e.g. 3.2 INTEL-8.0.61
            << std::endl;

  // There is one more graphics library used here, called GLEW.  This
  // library sorts through the various OpenGL updates and changes and
  // allows a user to pretend that it's all a consistent and simple
  // system.  The 'core profile' refers to some modern OpenGL
  // enhancements that are not so modern as of 2017.  Set this to true
  // to get those enhancements.
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("Failed to initialize GLEW");
  }

  if (glewIsSupported("GL_VERSION_2_1")) {
    std::cout << "Software check: Ready for OpenGL 2.1." << std::endl;
  } else {
    throw std::runtime_error("Software check: OpenGL 2.1 not supported.");
  }
 
  // Now we're ready to start issuing OpenGL calls.  Start by enabling
  // the modes we want.  The DEPTH_TEST and CULL_FACE are how you get
  // hidden faces.
  glEnable(GL_DEPTH_TEST | GL_PROGRAM_POINT_SIZE | GL_CULL_FACE);

  // This is the background color of the viewport.
  glClearColor(0.1 , 0.0, 0.4, 1.0);

  // Compile our shaders.
  shaderProgramID = setupShaders(vertexFileName, fragmentFileName);

  // Set up our buffers.
  setupBuffers(); 

  // This loop never exits.
  glutMainLoop();

  // We never get here, but the compiler is annoyed when you don't
  // exit from a function.
  return(0); 
}
