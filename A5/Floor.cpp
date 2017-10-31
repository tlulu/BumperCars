#include "Floor.hpp"

#include "cs488-framework/GlErrorCheck.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

float floor_vertices[] = {
  0.0f, 0.0f, 0.0f, // triangle 1 : begin
  1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, // triangle 1 : end
  1.0f, 0.0f, 1.0f, // triangle 2 : begin
  0.0f, 0.0f, 1.0f,
  1.0f, 0.0f, 0.0f // triangle 2 : end
};

Floor::Floor() {

}

Floor::~Floor() {

}

//----------------------------------------------------------------------------------------
void Floor::init(const ShaderProgram &shader) {
	m_shader = shader;

	uploadVertexDataToVbos();
}


//----------------------------------------------------------------------------------------
void Floor::uploadVertexDataToVbos() {
	glGenVertexArrays(1, &m_floor_vao);
	glBindVertexArray(m_floor_vao);

	// Generate a vertex buffer object to hold the triangle's vertex data.
	glGenBuffers(1, &m_floor_vbo);

	//-- Upload triangle vertex data to the vertex buffer:
	glBindBuffer(GL_ARRAY_BUFFER, m_floor_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0 );

	// GLint colAttrib = m_shader.getAttribLocation( "color" );
 //  glEnableVertexAttribArray(colAttrib);
 //  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
	glBindVertexArray( 0 );
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void Floor::draw(const glm::mat4 &proj, const glm::mat4 &view) {
	glBindVertexArray( m_floor_vao );

	GLint location = m_shader.getUniformLocation("proj");
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(proj));

	location = m_shader.getUniformLocation("view");
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(view));

	glm::mat4 model = glm::mat4();
	model = glm::scale(model, glm::vec3(2.8, 1.0, 1.4));
	model = glm::translate(model, glm::vec3(-0.5, 0.0, -0.5));
	location = m_shader.getUniformLocation("model");
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(model));

	location = m_shader.getUniformLocation("color");
	glUniform3f(location, 0.827451, 0.827451, 0.827451);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray( 0 );
}