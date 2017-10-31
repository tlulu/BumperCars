#include "Ball.hpp"

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "scene_lua.hpp"

Ball::Ball() 
	: m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_speed(1.0),
	  m_friction(0.005),
	  m_dir(glm::vec3(0.0, 0.0, 1.0)),
	  m_hit_goal(0)
	{

	}

//----------------------------------------------------------------------------------------
Ball::~Ball() {}

//----------------------------------------------------------------------------------------
void Ball::init(const MeshConsolidator &meshConsolidator, const BatchInfoMap &batchInfoMap, const ShaderProgram &shader) {
	m_batchInfoMap = batchInfoMap;
	m_shader = shader;

	processLuaSceneFile("Assets/ball.lua");

	glGenVertexArrays(1, &m_vao_meshData);

	enableVertexShaderInputSlots();

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(meshConsolidator);

	mapVboDataToVertexShaderInputLocations();
}

//----------------------------------------------------------------------------------------
void Ball::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
		return;
	}

	if (m_rootNode->children.size() > 0) {
		for (const SceneNode * node : m_rootNode->children) {
			if (node->m_name == "ball") {
				glm::mat4 trans = node->get_transform();
				glm::vec4 temp = trans * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				m_radius = temp.x;
				m_rootNode->translate(glm::vec3(0.0, m_radius, 0.0));
				break;
			}
		}
	}

	m_pos = glm::vec3(0.0, m_radius, 0.0);
}

//----------------------------------------------------------------------------------------
void Ball::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}
	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void Ball::uploadVertexDataToVbos (const MeshConsolidator & meshConsolidator) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void Ball::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void Ball::move(glm::vec3 dir, double ispeed) {
	m_dir = dir;
	m_speed = ispeed;
}

//----------------------------------------------------------------------------------------
void Ball::draw(const glm::mat4 &view) {
	// Ball is moving
	if (m_speed > 0.001) {

		glm::vec3 ballPos = getPos();
		glm::vec3 ballDir = getDir();
		double s = getSpeed();
		const double wall_boost = 0.05;

		// Within goal
		if (ballPos.x + m_radius >= 1.4 && (ballPos.z + m_radius < 0.2 && ballPos.z - m_radius > -0.2)) {
			stop();
			m_hit_goal = 2;
			return;
		} else if (ballPos.x - m_radius < -1.4 && (ballPos.z + m_radius < 0.2 && ballPos.z - m_radius > -0.2)) {
			stop();
			m_hit_goal = 1;
			return;
		} else {
			// Wall collision
			if (ballPos.x + m_radius >= 1.4) {
				move(glm::vec3(-ballDir.x, ballDir.y, ballDir.z), s + wall_boost);
			}
			if (ballPos.x - m_radius <= -1.4) {
				move(glm::vec3(-ballDir.x, ballDir.y, ballDir.z), s + wall_boost);
			}
			if (ballPos.z + m_radius >= 0.7) {
				move(glm::vec3(ballDir.x, ballDir.y, -ballDir.z), s + wall_boost);
			}
			if (ballPos.z - m_radius <= -0.7) {
				move(glm::vec3(ballDir.x, ballDir.y, -ballDir.z), s + wall_boost);
			}
		}

		float delta = m_speed * 0.05;
		float amountX = m_dir.x * delta;
		float amountZ = m_dir.z * delta;

		m_rootNode->translate(glm::vec3(amountX, 0, amountZ));
		m_pos = glm::vec3(m_pos.x + amountX, 0, m_pos.z + amountZ);
	}

	// std::cout << m_speed << std::endl;
	if (m_speed > 1.0) {
		m_speed = 1.0;
	}

	m_speed -= m_friction;

	if (m_speed < 0.0) {
		m_speed = 0.0;
	}

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	recursiveDraw(*m_rootNode, glm::mat4(1.0f), view);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void Ball::drawInverted(const glm::mat4 &view) {
	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	recursiveDraw(*m_rootNode, glm::scale(
                glm::translate(glm::mat4(), glm::vec3(0, 0, 0)),
                glm::vec3(1, -1, 1)), view);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void Ball::recursiveDraw(const SceneNode &n, glm::mat4 matrix, const glm::mat4 &view) {
	glm::mat4 transform = matrix * n.get_transform();

	if (n.m_nodeType == NodeType::GeometryNode) {
		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(&n);

		updateShaderUniforms(m_shader, *geometryNode, view * transform);

		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();
	}

	for (const SceneNode * node : n.children) {
		recursiveDraw(*node, transform, view);
	}
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
void Ball::updateShaderUniforms(
		const ShaderProgram &shader,
		const GeometryNode &node,
		const glm::mat4 &viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		glm::mat4 modelView = viewMatrix;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;

		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		glm::vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		glm::vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		glUniform1f(location, node.material.shininess);
		CHECK_GL_ERRORS;
	}
	shader.disable();
}

//----------------------------------------------------------------------------------------
double Ball::getRadius() {
	return m_radius;
}

//----------------------------------------------------------------------------------------
double Ball::getSpeed() {
	return m_speed;
}

//----------------------------------------------------------------------------------------
glm::vec3 Ball::getPos() {
	return m_pos;
}

//----------------------------------------------------------------------------------------
glm::vec3 Ball::getDir() {
	return m_dir;
}

//----------------------------------------------------------------------------------------
void Ball::stop() {
	m_speed = 0.0;
}

//----------------------------------------------------------------------------------------
void Ball::setPos(glm::vec3 pos) {
	m_rootNode->translate(glm::vec3(pos.x - m_pos.x, m_pos.y, m_pos.z));
	m_pos = pos;
}

//----------------------------------------------------------------------------------------
int Ball::hitGoal() {
	return m_hit_goal;
}
