#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 CubeNormal;
out vec3 CubePosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	// Normal for each point on the cube in WWC
  CubeNormal = mat3(transpose(inverse(model))) * normal;
  // Position for each point on the cube in WWC
  CubePosition = vec3(model * vec4(pos, 1.0));
  gl_Position = projection * view * model * vec4(pos, 1.0);
}