#include "Car.hpp"

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "glm/ext.hpp"

#include "scene_lua.hpp"

#include <math.h>

Car::Car() 
	: m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_pos(glm::vec3(0, 0, 0)),
	  m_dir(glm::vec3(1, 0, 0)),
	  m_acceleration(1.0),
	  m_speed(0.0),
	  m_friction(0.008)
	{

	}

//----------------------------------------------------------------------------------------
Car::~Car() {}

//----------------------------------------------------------------------------------------
void Car::init(const MeshConsolidator &meshConsolidator, const BatchInfoMap &batchInfoMap, const ShaderProgram &shader, const std::string file, const glm::vec3 pos, const glm::vec3 dir) {
	m_batchInfoMap = batchInfoMap;
	m_shader = shader;

	processLuaSceneFile(file);

	glGenVertexArrays(1, &m_vao_meshData);

	enableVertexShaderInputSlots();

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	m_origin_pos = pos;
	m_origin_dir = dir;

	m_pos = pos;
	m_dir = dir;
	m_rootNode->translate(m_pos);
}

//----------------------------------------------------------------------------------------
void Car::processLuaSceneFile(const std::string & filename) {
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

	glm::vec4 root_trans = m_rootNode->get_transform() * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	if (m_rootNode->children.size() > 0) {
		for (const SceneNode * node : m_rootNode->children) {
			if (node->m_name == "car") {
				glm::vec4 temp = node->get_transform() * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				// std::cout << temp.x << " " << temp.y << " " << temp.z << std::endl;
				m_length = temp.x * root_trans.x * 2;
				m_width = temp.z * root_trans.z * 2;
				m_rootNode->translate(glm::vec3(0.0f, (temp.y * root_trans.y/2.0), 0.0f));
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void Car::enableVertexShaderInputSlots()
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
void Car::uploadVertexDataToVbos (const MeshConsolidator & meshConsolidator) {
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
void Car::mapVboDataToVertexShaderInputLocations()
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
void Car::draw(const glm::mat4 &view) {
	if (m_speed > 0.001) {
		const float delta = 0.02;
		float amountX = m_dir.x * delta * m_speed + (0.5 * delta * delta * m_acceleration);
		float amountZ = m_dir.z * delta * m_speed + (0.5 * delta * delta * m_acceleration);

		// glm::vec3 carPos = getPos();
		// glm::vec3 carDir = getDir();
		// double bounceSpeed = m_speed;

		// if (carPos.x + getLength()/2.0 >= 1.4 && carDir.x > 0) {
		// 	//stop();
		// 	push(glm::vec3(-carDir.x, carDir.y, carDir.z), bounceSpeed);
		// }
		// else if (carPos.x - getLength()/2.0 <= -1.4 && carDir.x < 0) {
		// 	//stop();
		// 	push(glm::vec3(-carDir.x, carDir.y, carDir.z), bounceSpeed);
		// }
		// else if (carPos.z + getLength()/2.0 >= 0.7 && carDir.z > 0) {
		// 	//stop();
		// 	push(glm::vec3(carDir.x, carDir.y, -carDir.z), bounceSpeed);
		// }
		// else if (carPos.z - getLength()/2.0 <= -0.7 && carDir.z < 0) {
		// 	//stop();
		// 	push(glm::vec3(carDir.x, carDir.y, -carDir.z), bounceSpeed);
		// }

		m_rootNode->translate(glm::vec3(amountX, 0, amountZ));
		m_pos = glm::vec3(m_pos.x + amountX, 0, m_pos.z + amountZ);
	}

	m_speed -= m_friction;

	if (m_speed < 0.0) {
		m_speed = 0.0;
	}

	if (m_push_back_speed > 0.001) {
		const float delta = 0.023;
		float amountX = m_push_back_dir.x * delta * m_push_back_speed + (0.5 * delta * delta * m_acceleration);
		float amountZ = m_push_back_dir.z * delta * m_push_back_speed + (0.5 * delta * delta * m_acceleration);
		if (m_pos.x + m_length/2.0 + amountX < 1.4 && m_pos.x + amountX - m_length/2.0 > -1.4 &&  m_pos.z + amountZ + m_width/2.0 < 0.7 && m_pos.z + amountZ - m_width/2.0 > -0.7) {
			m_rootNode->translate(glm::vec3(amountX, 0, amountZ));
			m_pos = glm::vec3(m_pos.x + amountX, 0, m_pos.z + amountZ);
		}
	}

	m_push_back_speed -= m_friction;

	if (m_push_back_speed < 0.0) {
		m_push_back_speed = 0.0;
	}

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	//std::cout << m_pos.x << " " << m_pos.y << " " << m_pos.z << std::endl;

	recursiveDraw(*m_rootNode, glm::mat4(1.0f), view);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void Car::drawInverted(const glm::mat4 &view) {
	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	//std::cout << m_pos.x << " " << m_pos.y << " " << m_pos.z << std::endl;

	recursiveDraw(*m_rootNode, glm::scale(
                glm::translate(glm::mat4(), glm::vec3(0, 0, 0)),
                glm::vec3(1, -1, 1)), view);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void Car::moveForward() {
	m_speed += 0.018;
}

//----------------------------------------------------------------------------------------
void Car::moveBackward() {
	const float delta = -0.005;
	float amountX = m_dir.x * delta;
	float amountZ = m_dir.z * delta;
	m_rootNode->translate(glm::vec3(amountX, 0, amountZ));
	m_pos = glm::vec3(m_pos.x + amountX, 0, m_pos.z + amountZ);
}

//----------------------------------------------------------------------------------------
void Car::push(glm::vec3 dir, double amount) {
	m_push_back_dir = dir;
	m_push_back_speed = amount;
}

//----------------------------------------------------------------------------------------
void Car::rotateLeft() {
	// translate to origin
	m_rootNode->translate(glm::vec3(-m_pos.x, -m_pos.y, -m_pos.z));

	// rotate
	const float angle = 4.0f;
	glm::mat4 rot_matrix = glm::rotate(degreesToRadians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
 	glm::vec4 temp = rot_matrix * glm::vec4(m_dir, 0.0f);
 	m_dir = glm::vec3(temp.x, temp.y, temp.z);
	m_rootNode->rotate('y', angle);

	// translate back
	m_rootNode->translate(glm::vec3(m_pos.x, m_pos.y, m_pos.z));
}

//----------------------------------------------------------------------------------------
void Car::rotateRight() {
	// translate to origin
	m_rootNode->translate(glm::vec3(-m_pos.x, -m_pos.y, -m_pos.z));

	// rotate
	const float angle = -4.0f;
	glm::mat4 rot_matrix = glm::rotate(degreesToRadians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
 	glm::vec4 temp = rot_matrix * glm::vec4(m_dir, 0.0f);
 	m_dir = glm::vec3(temp.x, temp.y, temp.z);
	m_rootNode->rotate('y', angle);

	// translate back
	m_rootNode->translate(glm::vec3(m_pos.x, m_pos.y, m_pos.z));
}

//----------------------------------------------------------------------------------------
void Car::recursiveDraw(const SceneNode &n, glm::mat4 matrix, const glm::mat4 &view) {
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
void Car::updateShaderUniforms(
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
void Car::setPos(glm::vec3 pos) {
	m_pos = pos;
}

//----------------------------------------------------------------------------------------
double Car::getLength() {
	return m_length;
}

//----------------------------------------------------------------------------------------
double Car::getWidth() {
	return m_width;
}

//----------------------------------------------------------------------------------------
glm::vec3 Car::getPos() {
	return m_pos;
}

//----------------------------------------------------------------------------------------
glm::vec3 Car::getDir() {
	return m_dir;
}

//----------------------------------------------------------------------------------------
glm::vec3 Car::getDirBack() {
	return glm::vec3(-m_dir.x, m_dir.y, -m_dir.z);
}

//----------------------------------------------------------------------------------------
double Car::getSpeed() {
	return m_speed;
}

//----------------------------------------------------------------------------------------
void Car::setSpeed(double speed) {
	m_speed = speed;
}

//----------------------------------------------------------------------------------------
void Car::stop() {
	m_speed = 0.0;
}
