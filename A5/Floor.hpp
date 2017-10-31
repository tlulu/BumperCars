#pragma once

#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

class Floor {
public:
	Floor();
	virtual ~Floor();

	// One time initialization methods:
	void init(const ShaderProgram &shader);
	void uploadVertexDataToVbos();
	void draw(const glm::mat4 &proj, const glm::mat4 &view);

private:
	GLuint m_floor_vao;
	GLuint m_floor_vbo;
	ShaderProgram m_shader;
};
