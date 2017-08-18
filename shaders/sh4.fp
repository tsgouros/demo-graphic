#version 330

in vec4 gsColor;
in vec4 gsNormal;
//in vec4 gsPosition;

out vec4 color;

uniform sampler2D colorMap;

float gamma = 2.2;

vec4 togamma(vec4 a){
  return pow(a, vec4(gamma));
}

vec4 fromgamma(vec4 a){
  return pow(a, vec4(1./gamma));
}

void main()
{
  vec3 lightDir = normalize(vec3(0, 0.0, 1));
  float lambertian = max(dot(lightDir, gsNormal.xyz), 0.0);
  float ambient = 0.3;
  float diffuse = 1. - ambient;
  vec4 baseColor = gsColor;

  vec4 minColor = togamma(vec4( 0 , .9, 0 , 1));
  vec4 medColor = togamma(vec4( 1 , .2, .2, 1));
  vec4 maxColor = togamma(vec4( .4, .4,  1, 1));
  float midpoint = 0.3;

  float ratio = 0.5;

  color = baseColor * (ambient + diffuse * lambertian);
  //color = baseColor;
}
