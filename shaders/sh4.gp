#version 150

layout(points) in;
layout(triangle_strip, max_vertices=85) out;

in vec4 vertexColor[];

out vec4 gsColor;
out vec4 gsNormal;

//UNIFORM
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

float radius = 0.06;
float pi = 3.1415926;

vec4 get_vertex(float theta, float phi){
  return  vec4(sin(theta) * sin(phi), cos(phi), cos(theta) * sin(phi), 1.0);
}

vec4 transform(vec4 i){
  return projMatrix * viewMatrix * modelMatrix * i;
}

void main ()
{
  vec4 pointPosition = gl_in[0].gl_Position;

  gsColor = vertexColor[0];

  int nLat = 6;
  int nLon = 6;

  float phiStep = pi / nLat;
  float thetaStep = 2 * pi / nLon;

  for (float phi = 0; phi <= pi; phi += phiStep){

    for (float theta = 0; theta <= 2 * pi; theta += thetaStep){

      gsNormal = get_vertex(theta, phi);
      gl_Position = transform(pointPosition + radius * gsNormal);
      EmitVertex();

      gsNormal = get_vertex(theta, phi + phiStep);
      gl_Position = transform(pointPosition + radius * gsNormal);
      EmitVertex();
    }

    EndPrimitive();
  }
}
