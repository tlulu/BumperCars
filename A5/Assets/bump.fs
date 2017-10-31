#version 330 core
out vec4 FragColor;

in VS_OUT {
  vec2 TexCoords;
  vec3 TanLightPos;
  vec3 TanCameraPos;
  vec3 TanFragPos;
} fs_in;

uniform sampler2D textureMap;
uniform sampler2D normalMap;

vec3 ambientLighting(in vec3 color) {
   return 0.1 * color;
}

vec3 diffuseLighting(in vec3 N, in vec3 L, in vec3 color) {
   // calculation as for Lambertian reflection
   float diffuseTerm = clamp(dot(N, L), 0, 1) ;
   return diffuseTerm * color;
}

vec3 specularLighting(in vec3 N, in vec3 L, in vec3 V) {
   float specularTerm = 0;

   // calculate specular reflection only if
   // the surface is oriented to the light source
   if(dot(N, L) > 0) {
      // half vector
      vec3 H = normalize(L + V);
      specularTerm = pow(dot(N, H), 32.0);
   }
   return vec3(0.2) * specularTerm;
}

// Blinn-Phong
void main() {
  // normal from normal map in rgb range[0,1]
  vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
  // transform normal vector to range [-1,1]
  normal = normalize(normal * 2.0 - 1.0);

  // get texture color
  vec3 color = texture(textureMap, fs_in.TexCoords).rgb;

  // ambient
  vec3 ambient = ambientLighting(color);

  // diffuse
  vec3 lightDir = normalize(fs_in.TanLightPos - fs_in.TanFragPos);
  vec3 diffuse = diffuseLighting(normal, lightDir, color);

  // specular
  vec3 viewDir = normalize(fs_in.TanCameraPos - fs_in.TanFragPos);
  vec3 specular = specularLighting(normal, lightDir, viewDir);

  FragColor = vec4(ambient + diffuse + specular, 1.0);
}