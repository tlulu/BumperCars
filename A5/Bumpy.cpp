#include "Bumpy.hpp"

#include "cs488-framework/GlErrorCheck.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float PI = 3.14159265f;

Bumpy::Bumpy() {
}

Bumpy::~Bumpy() {

}

void Bumpy::init(unsigned int &normalMap, unsigned int &textureMap, const ShaderProgram &shader) {
	m_normal_map = normalMap;
	m_texture_map = textureMap;
	m_shader = shader;

	uploadVertexDataToVbos();
}

void Bumpy::uploadVertexDataToVbos() {
	// positions
  glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
  glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
  glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
  glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
  
  // texture coordinates
  glm::vec2 uv1(0.0f, 1.0f);
  glm::vec2 uv2(0.0f, 0.0f);
  glm::vec2 uv3(1.0f, 0.0f);  
  glm::vec2 uv4(1.0f, 1.0f);
  // normal vector
  glm::vec3 normal(0.0f, 0.0f, 1.0f);

  // calculate tangent vectors of both triangles
  glm::vec3 tangent1;
  glm::vec3 tangent2;
  // triangle 1
  // ----------
  glm::vec3 edge1 = pos2 - pos1;
  glm::vec3 edge2 = pos3 - pos1;
  glm::vec2 deltaUV1 = uv2 - uv1;
  glm::vec2 deltaUV2 = uv3 - uv1;

  GLfloat denom = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

  // Calculate texture tangents
  tangent1.x = denom * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
  tangent1.y = denom * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
  tangent1.z = denom * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
  tangent1 = glm::normalize(tangent1);

  // triangle 2
  // ----------
  edge1 = pos3 - pos1;
  edge2 = pos4 - pos1;
  deltaUV1 = uv3 - uv1;
  deltaUV2 = uv4 - uv1;

  denom = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

  // Calculate texture tangents
  tangent2.x = denom * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
  tangent2.y = denom * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
  tangent2.z = denom * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
  tangent2 = glm::normalize(tangent2);

  float vertices[] = {
    // positions            // normal                     // texcoords  // tangent                        
    pos1.x, pos1.y, pos1.z, normal.x, normal.y, normal.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z,
    pos2.x, pos2.y, pos2.z, normal.x, normal.y, normal.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z,
    pos3.x, pos3.y, pos3.z, normal.x, normal.y, normal.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z,

    pos1.x, pos1.y, pos1.z, normal.x, normal.y, normal.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z,
    pos3.x, pos3.y, pos3.z, normal.x, normal.y, normal.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z,
    pos4.x, pos4.y, pos4.z, normal.x, normal.y, normal.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z
  };

  // configure plane VAO
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
}

void Bumpy::draw(glm::mat4 perpsective, glm::mat4 view, glm::vec3 camera_position, glm::vec3 light_position) {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("projection");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(perpsective));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("view");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(view));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("cameraPos");
		glUniform3fv(location, 1, value_ptr(camera_position));
		location = m_shader.getUniformLocation("lightPos");
		glUniform3fv(location, 1, value_ptr(light_position));
		CHECK_GL_ERRORS;

		glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_map);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_normal_map);
	  
	  glBindVertexArray(quadVAO);
	  glm::mat4 model = glm::mat4();
	  model = glm::translate(model, glm::vec3(0.0, 0.01, 0.0));
	  model = glm::scale(model, glm::vec3(1.4f, 1.0f, 0.7f));
	  model = glm::rotate(model, -PI/2, glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		location = m_shader.getUniformLocation("model");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(model));
	  glDrawArrays(GL_TRIANGLES, 0, 6);

	  model = glm::mat4();
	  model = glm::translate(model, glm::vec3(0.0, -0.5, 0.701));
	  model = glm::scale(model, glm::vec3(1.4f, 0.5f, 0.7f));
	  //model = glm::rotate(model, -PI/2, glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		location = m_shader.getUniformLocation("model");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(model));
	  glDrawArrays(GL_TRIANGLES, 0, 6);

    model = glm::mat4();
    model = glm::translate(model, glm::vec3(0.0, -0.5, -0.701));
    model = glm::scale(model, glm::vec3(1.4f, 0.5f, 0.7f));
    //model = glm::rotate(model, -PI/2, glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
    location = m_shader.getUniformLocation("model");
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 6);

    model = glm::mat4();
    model = glm::translate(model, glm::vec3(1.401, -0.5, 0.0));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.7f));
    model = glm::rotate(model, -PI/2, glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
    location = m_shader.getUniformLocation("model");
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 6);

    model = glm::mat4();
    model = glm::translate(model, glm::vec3(-1.401, -0.5, 0.0));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.7f));
    model = glm::rotate(model, -PI/2, glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
    location = m_shader.getUniformLocation("model");
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	m_shader.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}
