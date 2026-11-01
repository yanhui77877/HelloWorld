#version 330 core

in vec2 vTexCoord;
in vec3 vWorldPosition;
in vec3 vWorldNormal;

uniform sampler2D imageTexture;
uniform vec3 lightPos;
uniform vec3 cameraPos;

out vec4 color;

void main() {
  vec3 N = normalize(vWorldNormal);
  vec3 L = normalize(lightPos - vWorldPosition);
  vec3 V = normalize(cameraPos - vWorldPosition);

  float diffuse = max(dot(N, L), 0.0);
  vec4 baseColor = texture(imageTexture, vTexCoord);

  float ambientStrength = 0.25;
  vec3 ambientColor = vec3(1.0);

  float specular = 0.0f;
  if (diffuse > 0.0) {
    vec3 R = reflect(-L, N);
    specular = pow(max(dot(V, R), 0.0), 128.0);
  }

  vec3 ambient = ambientStrength * ambientColor;
  vec3 directDiffuse = vec3(diffuse);

  float specularStrength = 0.5;
  vec3 specularColor = vec3(1.0);

  vec3 result = baseColor.rgb * (ambient + directDiffuse) +
                specularStrength * specular * specularColor;

  color = vec4(result, baseColor.a);
}