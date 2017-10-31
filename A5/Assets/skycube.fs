#version 330 core
out vec4 FragColor;

in vec3 CubeNormal;
in vec3 CubePosition;

uniform vec3 lookFrom;
uniform samplerCube cubemap;

void main() {    
  vec3 Eye = normalize(CubePosition - lookFrom);
  vec3 norm = normalize(CubeNormal);
  vec3 Reflection = Eye - 2 * dot(Eye, norm) * norm;
  FragColor = vec4(texture(cubemap, Reflection).rgb, 1.0);
}