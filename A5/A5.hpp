#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Car.hpp"
#include "Ball.hpp"
#include "Arena.hpp"
#include "Collision.hpp"
#include "Bumpy.hpp"
#include "Floor.hpp"
#include "Skybox.hpp"
#include "Skycube.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <stack>
#include <vector>

#include <cstdio>
#include <ctime>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};


class A5 : public CS488Window {
public:
	A5();
	virtual ~A5();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderArcCircle();

	// Helper methods
	unsigned int loadTexture(char const * path);
	unsigned int loadCubemap(std::vector<std::string> faces);
	void reset();

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	// Shader
	ShaderProgram m_shader;
	ShaderProgram m_normal_shader;
	ShaderProgram m_floor_shader;
	ShaderProgram m_skybox_shader;
	ShaderProgram m_skycube_shader;
	unsigned int m_cubemap_texture;

	// trackball TODO remove?
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	// Camera
	float m_camera_x;
	float m_camera_y;
	float m_camera_z;
	bool camera_x_plus_key_press;
	bool camera_x_minus_key_press;
	bool camera_y_plus_key_press;
	bool camera_y_minus_key_press;
	bool camera_z_plus_key_press;
	bool camera_z_minus_key_press;

	// Light
	bool light_x_plus_key_press;
	bool light_x_minus_key_press;
	bool light_y_plus_key_press;
	bool light_y_minus_key_press;
	bool light_z_plus_key_press;
	bool light_z_minus_key_press;

	// IO
	bool right_key_press;
	bool left_key_press;
	bool up_key_press;
	bool down_key_press;
	bool w_key_press;
	bool a_key_press;
	bool s_key_press;
	bool d_key_press;

	// GUI
	int radio;
	bool show_bump_mapping;
	bool show_reflection;
	bool show_sky_cube;

	// Objects
	Arena arena;
	Car car;
	Car car2;
	Ball ball;
	Collision collision;

	Bumpy bumpy;
	Floor floor;
	Skybox skybox;
	Skycube skycube;

	int playerScore;
	int player2Score;
	bool gameOver;
	std::clock_t start;
  double duration;
  int winner;

  int max_duration = 60;
};
