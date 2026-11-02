#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

uniform mat4 transform;
uniform mat4 model;

out vec2 vTexCoord;
out vec3 vWorldPosition;
out vec3 vWorldNormal;

void main() {
  gl_Position = transform * vec4(position, 1.0);

  vTexCoord = texcoord;
  vWorldPosition = (model * vec4(position, 1.0)).xyz;
  mat3 normalMatrix = transpose(inverse(mat3(model)));
  vWorldNormal = normalMatrix * normal;
}