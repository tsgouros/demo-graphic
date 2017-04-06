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
uniform vec4 lightPositionWS[NUM_LIGHTS];
uniform vec4 lightColor[NUM_LIGHTS];
uniform vec4 lightCoefficients[NUM_LIGHTS];

// Material properties
uniform vec3 colorAmbient;
uniform vec3 colorDiffuse;
uniform vec3 colorSpecular;
uniform float specularExp;
uniform float opacity;

// Indicators if textures exists and textureIDs
uniform bool hasVertexColors;
uniform bool hasTexAmbient;
uniform bool hasTexDiffuse;
uniform bool hasTexSpecular;
uniform bool hasTexSpecularExp;
uniform bool hasTexOpacity;

uniform sampler2D texAmbient;
uniform sampler2D texDiffuse;
uniform sampler2D texSpecular;
uniform sampler2D texSpecularExp;
uniform sampler2D texOpacity;

void main() {
  vec4 matColorAmbient = vec4(colorAmbient, 1.0);
  vec4 matColorDiffuse = vec4(colorDiffuse, 1.0);
  vec4 matColorSpecular = vec4(colorDiffuse, 1.0);
  float matSpecularExp = specularExp;
  float matOpacity = opacity;

  // Overwrite material colors with texture colors if any are loaded
    if (hasTexAmbient) {
          matColorAmbient = texture2D(texAmbient, uvFrag);
    } else {
        // If a diffuse texture exists we want to use it as ambient texture as well
        if (hasTexDiffuse) {
              matColorDiffuse = texture2D(texDiffuse, uvFrag);
        } else {
              if (hasVertexColors) {
                  matColorDiffuse = colorFrag;
              }
        }
    }

    if (hasTexDiffuse) {
          matColorDiffuse = texture2D(texDiffuse, uvFrag);
    } else {
          if (hasVertexColors) {
              matColorDiffuse = colorFrag;
          }
    }

    if (hasTexSpecular) {
          matColorSpecular = texture2D(texSpecular, uvFrag);
    }


    if (hasTexSpecularExp) {
          matSpecularExp = texture2D(texSpecularExp, uvFrag).x;
    }

    if (hasTexOpacity) {
          matOpacity = texture2D(texOpacity, uvFrag).x;
    }

  vec4 outputColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  
  // The lighting effects are additive, so we run through the lights,
  // and add their effects.
  for (int i = 0; i < NUM_LIGHTS; i++) {

    // Ambient : simulates indirect lighting
    vec4 ambient = lightCoefficients[i].r * lightColor[i] * matColorAmbient;
    
    // Distance to the light
    float distanceToLight = length(lightPositionWS[i] - positionWS);

    // Cosine of the angle between the normal and the light direction, 
    // clamped to remain above 0.
    //  - light is at the vertical of the triangle -> 1
    //  - light is perpendicular to the triangle -> 0
    //  - light is behind the triangle -> 0
    float cosAngleFromNormal = max(0.0, dot(normalCS, lightDirectionCS[i]));

    // Diffuse : "color" of the object
    vec4 diffuse = lightCoefficients[i].g * lightColor[i] * cosAngleFromNormal * matColorDiffuse;
    
    // Direction in which the triangle reflects the light
    vec4 reflectDir = reflect(-lightDirectionCS[i], normalCS);

    // Cosine of the angle between the Eye vector and the Reflect vector,
    // clamped to remain above 0.
    float cosAlpha = clamp(dot(eyeDirectionCS, reflectDir), 0.0, 1.0);

    // Specular : reflective highlight, like a mirror. Adjust the
    // exponent to adjust the size of the highlight.
    vec4 specular = lightColor[i] * pow(cosAlpha, matSpecularExp) * matColorSpecular;
    float attenuation = 1.0 / (1.0 + 0.01 * pow(distanceToLight, 2));

    outputColor += ambient + attenuation * (diffuse + 0.0 * lightCoefficients[i].b * specular);

  }
  
  gl_FragColor = outputColor;

}
