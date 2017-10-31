#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "GeometryNode.hpp"

#include <glm/glm.hpp>
#include <memory>

class Ball {
public:
	Ball();
	virtual ~Ball();

	// One time initialization methods:
	void init(const MeshConsolidator &meshConsolidator, const BatchInfoMap &batchInfoMap, const ShaderProgram &shader);
	void processLuaSceneFile(const std::string & filename);
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos (const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void draw(const glm::mat4 &view);
	void drawInverted(const glm::mat4 &view);

	// Helper methods
	void updateShaderUniforms(const ShaderProgram &shader, const GeometryNode &node, const glm::mat4 &view);
	void recursiveDraw(const SceneNode &n, glm::mat4 matrix, const glm::mat4 &view);

	void move(glm::vec3 dir, double ispeed);
	void setPos(glm::vec3 pos);
	void stop();
	int hitGoal();

	double getRadius();
	double getSpeed();
	glm::vec3 getPos();
	glm::vec3 getDir();
private:
	BatchInfoMap m_batchInfoMap;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	std::shared_ptr<SceneNode> m_rootNode;

	glm::vec3 m_pos;
	glm::vec3 m_dir;
	double m_radius;
	double m_speed;
	double m_friction;

	int m_hit_goal;
};
