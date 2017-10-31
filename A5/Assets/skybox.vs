#version 330 core
in vec3 pos;
out vec3 dir;

uniform mat4 projection;
uniform mat4 view;

void main() {
  dir = pos;
  gl_Position = projection * view * vec4(pos, 1.0);
} 