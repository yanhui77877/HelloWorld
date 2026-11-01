#version 330 core

in vec2 vTexCoord;
in vec3 vWorldPosition;
in vec3 vWorldNormal;

uniform sampler2D imageTexture;
uniform vec3 lightPos;
uniform vec3 lightColour;
uniform vec3 cameraPos;
uniform float lightIntensity;

out vec4 color;

void 
main() 
{
  vec3 N = normalize(vWorldNormal);
  vec3 L = normalize(lightPos - vWorldPosition);
  float distance = length(lightPos - vWorldPosition);
  vec3 V = normalize(cameraPos - vWorldPosition);

  float diffuse = max(dot(N, L), 0.0);
  vec4 baseColor = texture(imageTexture, vTexCoord);

  float ambientStrength = 0.25;
  vec3 ambientColor = vec3(1.0);

  float specular = 0.0f;

  if (diffuse > 0.0) 
  {
    vec3 R = reflect(-L, N);
    specular = pow(max(dot(V, R), 0.0), 128.0);
  }

  float attenuation = 1.0/(1.0 + 0.09*distance + 0.032*distance*distance);

  float specularStrength = 1.f;

  vec3 finalAmbient  = baseColor.rgb*ambientStrength*ambientColor;
  vec3 finalDiffuse  = baseColor.rgb*vec3(diffuse)*lightColour;
  vec3 finalSpecular = specularStrength*specular*lightColour;

  vec3 result = finalAmbient + attenuation*lightIntensity*(finalDiffuse + finalSpecular);

  color = vec4(result, baseColor.a);
}