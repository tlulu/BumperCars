#pragma once

#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

class Skycube {
public:
	Skycube();
	virtual ~Skycube();

	// One time initialization methods:
	void init(const ShaderProgram &shader, unsigned int &cubemap_texture);
	void uploadVertexDataToVbos();
	void draw(const glm::mat4 &proj, const glm::mat4 &view, const glm::vec3 &cameraPos);

private:
	GLuint m_skycube_vao;
	GLuint m_skycube_vbo;
	ShaderProgram m_shader;
	unsigned int m_cubemap_texture;
};
