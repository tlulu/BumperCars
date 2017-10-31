#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoord;
layout (location = 3) in vec3 tangent;

out VS_OUT {
  vec2 TexCoords;
  vec3 TanLightPos;
  vec3 TanCameraPos;
  vec3 TanFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightPos;
uniform vec3 cameraPos;

void main() {
  vec3 fragPos = vec3(model * vec4(pos, 1.0));   
  vs_out.TexCoords = textureCoord;

  mat3 normalMatrix = transpose(inverse(mat3(model)));
  vec3 T = normalize(normalMatrix * tangent);
  vec3 N = normalize(normalMatrix * normal);
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);

  mat3 TBN = transpose(mat3(T, B, N));  
  // Convert to tangent space  
  vs_out.TanLightPos = TBN * lightPos;
  vs_out.TanCameraPos = TBN * cameraPos;
  vs_out.TanFragPos = TBN * fragPos;
    
  gl_Position = projection * view * model * vec4(pos, 1.0);
}