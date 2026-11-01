#version 330 core

in vec2 vTexCoord;
in vec3 vWorldPosition;
in vec3 vWorldNormal;

uniform sampler2D imageTexture;
uniform vec3 lightPos;

out vec4 color;

void main() {
  vec3 N = normalize(vWorldNormal);
  vec3 L = normalize(lightPos - vWorldPosition);

  float diffuse = max(dot(N, L), 0.0);

  vec4 baseColor = texture(imageTexture, vTexCoord);
  color = vec4(baseColor.rgb * diffuse, baseColor.a);
}