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
  vec3 lightDir = normalize(vec3(0.0, 1.0, 0.5));
  float lambertian = max(dot(lightDir, gsNormal.xyz), 0.0);
  float ambient = 0.3;
  float diffuse = 1.0 - ambient;
  vec4 baseColor = gsColor;

  color = baseColor * (ambient + diffuse * lambertian);
}
