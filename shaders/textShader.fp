#version 120

// The number of lights is filled in before the shader is compiled.
const int NUM_LIGHTS = XX;
const float MAX_DIST = 50.0;
const float MAX_DIST_SQUARED = MAX_DIST * MAX_DIST;

// Interpolated values from the vertex shaders
varying vec4 colorFrag;
varying vec2 uvFrag;
varying vec4 positionWS;
varying vec4 eyeDirectionCS;
varying vec4 lightDirectionCS[NUM_LIGHTS];
varying vec4 normalCS;
varying vec4 lightPositionCS;

// Values that stay constant for the whole mesh.
uniform sampler2D textureImage;
uniform vec4 lightPositionWS[NUM_LIGHTS];
uniform vec4 lightColor[NUM_LIGHTS];

void main() {

  float a = texture2D(textureImage, uvFrag).r;

  vec4 materialColor;

  if (a > 0.1) {
    materialColor = vec4(colorFrag.xyz * a, 1);
  } else {
    discard;
    // materialColor = vec4(0.f);
  }

  // gl_FragColor = materialColor;
  // gl_FragColor = vec4(uvFrag, 0, 1);

  //vec4 materialColor = colorFrag;
  //0.6 * vec4(1.0, 1.0, 1.0, 1.0);
  float ambientCoefficient = 0.3;
  vec4 materialSpecularColor = 0.5 * vec4(1.0, 1.0, 1.0, 0.0);

  vec4 color = 0.5 * materialColor;
  //vec4 color = vec4(0,0,0,0);
  
  // The lighting effects are additive, so we run through the lights,
  // and add their effects.
  for (int i = 0; i < NUM_LIGHTS; i++) {

   // Ambient : simulates indirect lighting
   vec4 ambient = ambientCoefficient * lightColor[i] * materialColor;
   
   // Distance to the light
   float distanceToLight = length(lightPositionWS[i] - positionWS);

   // Cosine of the angle between the normal and the light direction, 
   // clamped to remain above 0.
   //  - light is at the vertical of the triangle -> 1
   //  - light is perpendicular to the triangle -> 0
   //  - light is behind the triangle -> 0
   // TODO @martha what is this
   // ws and cs are worldspace and cameraspace!
   // float cosAngleFromNormal = max(0.0, dot(normalCS, lightDirectionCS[i]));

   // Diffuse : "color" of the object
   vec4 diffuse = materialColor * lightColor[i];// * cosAngleFromNormal;
   
   // Direction in which the triangle reflects the light
   // vec4 reflectDir = reflect(- [i], normalCS);

   // // Cosine of the angle between the Eye vector and the Reflect vector,
   // // clamped to remain above 0.
   // float cosAlpha = clamp(dot(eyeDirectionCS, reflectDir), 0.0, 1.0);

   // // Specular : reflective highlight, like a mirror. Adjust the
   // // exponent to adjust the size of the highlight.
   // vec4 specular = materialSpecularColor * lightColor[i] * pow(cosAlpha, 5);
   // //specular = materialSpecularColor * pow(cosAlpha, 9);
   
   float attenuation = 1.0 / (1.0 + 0.01 * pow(distanceToLight, 2));
   //attenuation = 1.0;
   
   // @martha
   // commenting attenuation back in will still work, but it'll erase the color
   // and also create this weird pixellated effect -- why does that happen?
   color += ambient;// + attenuation * (diffuse + 0.0 * specular);
  }
  
  gl_FragColor = color; // normalize(normalCS) ;//+ materialColor;

}
