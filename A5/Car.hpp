#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "GeometryNode.hpp"

#include <glm/glm.hpp>
#include <memory>

class Car {
public:
	Car();
	virtual ~Car();

	// One time initialization methods:
	void init(const MeshConsolidator &meshConsolidator, const BatchInfoMap &batchInfoMap, const ShaderProgram &shader, const std::string, const glm::vec3 pos, const glm::vec3 dir);
	void processLuaSceneFile(const std::string & filename);
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos (const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void draw(const glm::mat4 &view);
	void drawInverted(const glm::mat4 &view);

	// Helper methods
	void updateShaderUniforms(const ShaderProgram &shader, const GeometryNode &node, const glm::mat4 &view);
	void recursiveDraw(const SceneNode &n, glm::mat4 matrix, const glm::mat4 &view);

	// Control methods
	void moveForward();
	void moveBackward();
	void rotateLeft();
	void rotateRight();
	void move();
	void rotate();
	void setPos(glm::vec3 pos);
	void stop();
	void push(glm::vec3 dir, double amount);
	void setSpeed(double speed);

	// Getter methods
	glm::vec3 getPos();
	glm::vec3 getDir();
	glm::vec3 getDirBack();
	double getLength();
	double getWidth();
	double getSpeed();
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
	double m_acceleration;
	double m_speed;
	double m_friction;
	glm::vec3 m_origin_pos;
	glm::vec3 m_origin_dir;

	glm::vec3 m_push_back_dir;
	double m_push_back_speed;

	double m_length;
	double m_width;
};
