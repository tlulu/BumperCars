#include "Skybox.hpp"

#include "cs488-framework/GlErrorCheck.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

float skybox_vertices[] = {
	10.0f, -10.0f, -10.0f, // triangle 1 : begin
  10.0f, -10.0f,  10.0f,
  10.0f,  10.0f,  10.0f, // triangle 1 : end
  10.0f,  10.0f,  10.0f, // triangle 2 : begin
  10.0f,  10.0f, -10.0f,
  10.0f, -10.0f, -10.0f, // triangle 2 : end

  -10.0f,  10.0f, -10.0f, // triangle 3 : begin
  -10.0f, -10.0f, -10.0f,
   10.0f, -10.0f, -10.0f, // triangle 3 : end
   10.0f, -10.0f, -10.0f, // triangle 4 : begin
   10.0f,  10.0f, -10.0f,
  -10.0f,  10.0f, -10.0f, // triangle 4 : end

  -10.0f, -10.0f,  10.0f, // triangle 5 : begin
  -10.0f, -10.0f, -10.0f,
  -10.0f,  10.0f, -10.0f, // triangle 5 : end
  -10.0f,  10.0f, -10.0f, // triangle 6 : begin
  -10.0f,  10.0f,  10.0f,
  -10.0f, -10.0f,  10.0f, // triangle 6 : end

  -10.0f, -10.0f,  10.0f, // triangle 7 : begin
  -10.0f,  10.0f,  10.0f,
   10.0f,  10.0f,  10.0f, // triangle 7 : end
   10.0f,  10.0f,  10.0f, // triangle 8 : begin
   10.0f, -10.0f,  10.0f,
  -10.0f, -10.0f,  10.0f, // triangle 8 : end

  -10.0f,  10.0f, -10.0f, // triangle 9 : begin
   10.0f,  10.0f, -10.0f,
   10.0f,  10.0f,  10.0f, // triangle 9 : end
   10.0f,  10.0f,  10.0f, // triangle 10 : begin
  -10.0f,  10.0f,  10.0f,
  -10.0f,  10.0f, -10.0f, // triangle 10 : end

  -10.0f, -10.0f, -10.0f, // triangle 11 : begin
  -10.0f, -10.0f,  10.0f,
   10.0f, -10.0f, -10.0f, // triangle 11 : end
   10.0f, -10.0f, -10.0f, // triangle 12 : begin
  -10.0f, -10.0f,  10.0f,
   10.0f, -10.0f,  10.0f	 // triangle 12 : end
};

Skybox::Skybox() {

}

Skybox::~Skybox() {

}

//----------------------------------------------------------------------------------------
void Skybox::init(const ShaderProgram &shader, unsigned int &cubemap_texture) {
	m_shader = shader;
	m_cubemap_texture = cubemap_texture;

	uploadVertexDataToVbos();
}


//----------------------------------------------------------------------------------------
void Skybox::uploadVertexDataToVbos() {
	// Skybox VAO
	glGenVertexArrays(1, &m_skybox_vao);
  glGenBuffers(1, &m_skybox_vbo);
  glBindVertexArray(m_skybox_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_skybox_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void Skybox::draw(const glm::mat4 &proj, const glm::mat4 &view) {
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
  m_shader.enable();
  // Move with the camera
  glm::mat4 view_no_translate = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
  GLint location = m_shader.getUniformLocation("view");
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(view_no_translate));
	location = m_shader.getUniformLocation("projection");
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(proj));
	CHECK_GL_ERRORS;
  // skybox cube
  glBindVertexArray(m_skybox_vao);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap_texture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS); // set depth function back to default
  m_shader.disable();
}