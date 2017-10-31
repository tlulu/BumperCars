#include "Skycube.hpp"

#include "cs488-framework/GlErrorCheck.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

float skycube_vertices[] = {
  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
  1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
  1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
  1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
  1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

  -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
   1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
   1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
   1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
  -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
  -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

  -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
   1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
   1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
   1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
   1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
   1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
   1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
  -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
  -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,

  -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
  -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
  -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
  -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
  -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
  -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

  -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
   1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
   1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
   1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
  -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
  -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f
};

Skycube::Skycube() {

}

Skycube::~Skycube() {

}

//----------------------------------------------------------------------------------------
void Skycube::init(const ShaderProgram &shader, unsigned int &cubemap_texture) {
	m_shader = shader;
	m_cubemap_texture = cubemap_texture;

	uploadVertexDataToVbos();
}


//----------------------------------------------------------------------------------------
void Skycube::uploadVertexDataToVbos() {
	glGenVertexArrays(1, &m_skycube_vao);
  glGenBuffers(1, &m_skycube_vbo);
  glBindVertexArray(m_skycube_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_skycube_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skycube_vertices), &skycube_vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void Skycube::draw(const glm::mat4 &proj, const glm::mat4 &view, const glm::vec3 &cameraPos) {
  m_shader.enable();
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::scale(model, glm::vec3(1.4, 0.5, 0.7));
  model = glm::translate(model, glm::vec3(0.0, -1.0, 0.0));
  GLint location = m_shader.getUniformLocation("view");
  glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(view));
  location = m_shader.getUniformLocation("projection");
  glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(proj));
  location = m_shader.getUniformLocation("model");
  glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(model));
  location = m_shader.getUniformLocation("lookFrom");
  glUniform3fv(location, 1, value_ptr(cameraPos));
  CHECK_GL_ERRORS;
  
  glBindVertexArray(m_skycube_vao);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap_texture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  m_shader.disable();
}