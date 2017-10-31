#pragma once

#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

class Skybox {
public:
	Skybox();
	virtual ~Skybox();

	// One time initialization methods:
	void init(const ShaderProgram &shader, unsigned int &cubemap_texture);
	void uploadVertexDataToVbos();
	void draw(const glm::mat4 &proj, const glm::mat4 &view);

private:
	GLuint m_skybox_vao;
	GLuint m_skybox_vbo;
	ShaderProgram m_shader;
	unsigned int m_cubemap_texture;
};
