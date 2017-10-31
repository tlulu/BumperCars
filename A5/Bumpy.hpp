#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

class Bumpy {
public:
	Bumpy();
	virtual ~Bumpy();

	// One time initialization methods:
	void init(unsigned int &normalMap, unsigned int &textureMap, const ShaderProgram &shader);
	void draw(glm::mat4 perpsective, glm::mat4 view, glm::vec3 camera_position, glm::vec3 light_position);
private:
	void uploadVertexDataToVbos();

	ShaderProgram m_shader;
	unsigned int m_normal_map;
	unsigned int m_texture_map;

	unsigned int quadVAO;
	unsigned int quadVBO;

	glm::vec3 m_pos;
};
