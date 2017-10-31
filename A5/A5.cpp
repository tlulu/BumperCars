#include "A5.hpp"
#include "scene_lua.hpp"

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>
#include "stb_image.hpp"
#include "filesystem.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include <irrKlang.h>
using namespace irrklang;

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

ISoundEngine *SoundEngine = createIrrKlangDevice();

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A5::A5()
	: m_arc_positionAttribLocation(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
	  up_key_press(false),
	  down_key_press(false),
	  left_key_press(false),
	  right_key_press(false),
	  camera_x_plus_key_press(false),
	  camera_x_minus_key_press(false),
	  camera_y_plus_key_press(false),
	  camera_y_minus_key_press(false),
	  camera_z_plus_key_press(false),
	  camera_z_minus_key_press(false),
	  light_x_plus_key_press(false),
	  light_x_minus_key_press(false),
	  light_y_plus_key_press(false),
	  light_y_minus_key_press(false),
	  light_z_plus_key_press(false),
	  light_z_minus_key_press(false),
	  m_camera_x(0.0f),
	  m_camera_y(2.0*M_SQRT1_2 + 0.5),
	  m_camera_z(2.0*M_SQRT1_2),
	  show_bump_mapping(false),
	  show_reflection(false),
	  show_sky_cube(false),
	  radio(0),
	  playerScore(0),
	  player2Score(0),
	  gameOver(false),
	  winner(0),
	  duration(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A5::~A5()
{
	SoundEngine->drop();
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A5::init()
{
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	enableVertexShaderInputSlots();

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	std::unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});

	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	arena.init(*meshConsolidator, m_batchInfoMap, m_shader);
	ball.init(*meshConsolidator, m_batchInfoMap, m_shader);
	car.init(*meshConsolidator, m_batchInfoMap, m_shader, "Assets/car.lua", glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0));
	car2.init(*meshConsolidator, m_batchInfoMap, m_shader, "Assets/car2.lua", glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0));

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	unsigned int diffuseMap = loadTexture(FileSystem::getPath("A5/Assets/gray.jpeg").c_str());
	unsigned int normalMap = loadTexture(FileSystem::getPath("A5/Assets/bumpy.jpg").c_str());

	bumpy.init(normalMap, diffuseMap, m_normal_shader);
	floor.init(m_floor_shader);

  std::vector<std::string> faces {
    FileSystem::getPath("A5/Assets/skybox/ThickCloudsWaterRight.png"),
    FileSystem::getPath("A5/Assets/skybox/ThickCloudsWaterLeft.png"),
    FileSystem::getPath("A5/Assets/skybox/ThickCloudsWaterUp.png"),
    FileSystem::getPath("A5/Assets/skybox/ThickCloudsWaterDown.png"),
    FileSystem::getPath("A5/Assets/skybox/ThickCloudsWaterBack.png"),
    FileSystem::getPath("A5/Assets/skybox/ThickCloudsWaterFront.png")
  };
  m_cubemap_texture = loadCubemap(faces);

  skybox.init(m_skybox_shader, m_cubemap_texture);
  skycube.init(m_skycube_shader, m_cubemap_texture);


  start = std::clock();

  SoundEngine->play2D("../A5/Assets/audio/background.mp3", GL_TRUE);

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A5::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();

	m_floor_shader.generateProgramObject();
	m_floor_shader.attachVertexShader( getAssetFilePath("floor.vs").c_str() );
	m_floor_shader.attachFragmentShader( getAssetFilePath("floor.fs").c_str() );
	m_floor_shader.link();

	m_normal_shader.generateProgramObject();
	m_normal_shader.attachVertexShader( getAssetFilePath("bump.vs").c_str() );
	m_normal_shader.attachFragmentShader( getAssetFilePath("bump.fs").c_str() );
	m_normal_shader.link();

	m_normal_shader.enable();
	{
		GLint location = m_normal_shader.getUniformLocation("textureMap");
		glUniform1i(location, 0);
		location = m_normal_shader.getUniformLocation("normalMap");
		glUniform1i(location, 1);
	}
	m_normal_shader.disable();

	m_skybox_shader.generateProgramObject();
	m_skybox_shader.attachVertexShader( getAssetFilePath("skybox.vs").c_str() );
	m_skybox_shader.attachFragmentShader( getAssetFilePath("skybox.fs").c_str() );
	m_skybox_shader.link();

	m_skybox_shader.enable();
	{
		GLint location = m_skybox_shader.getUniformLocation("cubemap");
		glUniform1i(location, 0);
	}
	m_skybox_shader.disable();

	m_skycube_shader.generateProgramObject();
	m_skycube_shader.attachVertexShader( getAssetFilePath("skycube.vs").c_str() );
	m_skycube_shader.attachFragmentShader( getAssetFilePath("skycube.fs").c_str() );
	m_skycube_shader.link();

	m_skycube_shader.enable();
	{
		GLint location = m_skycube_shader.getUniformLocation("cubemap");
		glUniform1i(location, 0);
	}
	m_skycube_shader.disable();
}

//----------------------------------------------------------------------------------------
void A5::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A5::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A5::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A5::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A5::initViewMatrix() {
	m_view = glm::lookAt( 
		glm::vec3( m_camera_x, m_camera_y, m_camera_z ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );
}

//----------------------------------------------------------------------------------------
void A5::initLightSources() {
	// World-space position
	m_light.position = glm::vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = glm::vec3(0.8f); // White light
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int A5::loadTexture(char const * path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1) {
      format = GL_RED;
    }
    else if (nrComponents == 3) {
      format = GL_RGB;
    }
    else if (nrComponents == 4) {
      format = GL_RGBA;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order: right, left, top, bottom, front, back
// -------------------------------------------------------
unsigned int A5::loadCubemap(std::vector<std::string> faces) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    }
    else {
      std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
      stbi_image_free(data);
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
}

//----------------------------------------------------------------------------------------
void A5::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		//-- Set LightSource uniform for the scene:
		location = m_shader.getUniformLocation("light.position");
		glUniform3fv(location, 1, value_ptr(m_light.position));
		location = m_shader.getUniformLocation("light.rgbIntensity");
		glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("ambientIntensity");
		glm::vec3 ambientIntensity(0.05f);
		glUniform3fv(location, 1, value_ptr(ambientIntensity));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("overrideColor");
		glUniform3f(location, 1, 1, 1);
		CHECK_GL_ERRORS;
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A5::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A5::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);

		if (winner == 1) {
			ImGui::Text( "Winner: Player 1!" );
		} else if (winner == 2) {
			ImGui::Text( "Winner: Player 2!" );
		} else if (winner == 3) {
			ImGui::Text( "Tie!" );
		}

		ImGui::Text( "Duration: %f", max_duration - duration * 10 );

		ImGui::Text( "Player 1 score: %d", playerScore );

		ImGui::Text( "Player 2 score: %d", player2Score );

		if( ImGui::Button( "Restart" ) ) {
			reset();
			winner = 0;
			duration = 0;
			playerScore = 0;
			player2Score = 0;
			start = std::clock();
		}

		if( ImGui::RadioButton( "Phong", &radio, 0 ) ) {
			show_bump_mapping = false;
			show_reflection = false;
			show_sky_cube = false;
		}
		if( ImGui::RadioButton( "Bump map", &radio, 1 ) ) {
			show_bump_mapping = true;
			show_reflection = false;
			show_sky_cube = false;
		}
		if( ImGui::RadioButton( "Reflection (stencil)", &radio, 2 ) ) {
			show_reflection = true;
			show_bump_mapping = false;
			show_sky_cube = false;
		}
		if( ImGui::RadioButton( "Reflection (skybox)", &radio, 3 ) ) {
			show_sky_cube = true;
			show_bump_mapping = false;
			show_reflection = false;
		}

		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A5::draw() {
	// Controls
	if (camera_x_plus_key_press) {
		m_camera_x += 0.03;
	}
	if (camera_x_minus_key_press) {
		m_camera_x -= 0.03;
	}
	if (camera_y_plus_key_press) {
		m_camera_y += 0.03;
	}
	if (camera_y_minus_key_press) {
		m_camera_y -= 0.03;
	}
	if (camera_z_plus_key_press) {
		m_camera_z += 0.03;
	}
	if (camera_z_minus_key_press) {
		m_camera_z -= 0.03;
	}
	if (light_x_plus_key_press) {
		m_light.position.x += 0.05;
	}
	if (light_x_minus_key_press) {
		m_light.position.x -= 0.05;
	}
	if (light_y_plus_key_press) {
		m_light.position.y += 0.05;
	}
	if (light_y_minus_key_press) {
		m_light.position.y -= 0.05;
	}
	if (light_z_plus_key_press) {
		m_light.position.z += 0.05;
	}
	if (light_z_minus_key_press) {
		m_light.position.z -= 0.05;
	}

	if (m_camera_y < 0) {
		m_camera_y = 0.0;
	}

	// Car 1
	if (right_key_press) {
		car.rotateRight();
	}
	if (left_key_press) {
		car.rotateLeft();
	}
	if (down_key_press) {
		if (!collision.carArenaCollision(car, arena, true)) {
			car.moveBackward();
		}
	}

	// Car 2
	if (d_key_press) {
		car2.rotateRight();
	}
	if (a_key_press) {
		car2.rotateLeft();
	}
	if (s_key_press) {
		if (!collision.carArenaCollision(car2, arena, true)) {
			car2.moveBackward();
		}
	}

	m_view = glm::lookAt( 
		glm::vec3( m_camera_x, m_camera_y, m_camera_z ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	// draw skybox
	skybox.draw(m_perpsective, m_view);	

	int goal = ball.hitGoal();
	if (goal != 0) {
		if (goal == 1) {
			player2Score++;
		} else if (ball.hitGoal() == 2) {
			playerScore++;
		}
		SoundEngine->play2D("../A5/Assets/audio/win.wav", GL_FALSE);
		// Pause a bit
		std::this_thread::sleep_for (std::chrono::seconds(3));
		reset();
		return;
	}

	// Time's up
	if (duration > 0.1 * max_duration) {
		if (playerScore > player2Score) {
			winner = 1;
		}
		else if (playerScore < player2Score) {
			winner = 2;
		} else {
			winner = 3;
		}
		return;
	} else {
		duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	}

	glEnable( GL_DEPTH_TEST );

	if (show_sky_cube) {
		skycube.draw(m_perpsective, m_view, glm::vec3(m_camera_x, m_camera_y, m_camera_z));
	}

	if (show_bump_mapping) {
		bumpy.draw(m_perpsective, m_view, glm::vec3(m_camera_x, m_camera_y, m_camera_z), m_light.position);
	}

	// Draw ball
	ball.draw(m_view);

	// Draw cars
	car.draw(m_view);
	car2.draw(m_view);

	if (show_reflection) {
		glEnable(GL_STENCIL_TEST);
		// Set any stencil to 1
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // Write to stencil buffer
    glStencilMask(0xFF);
    // Disable depth testing to show reflection
    glDepthMask(GL_FALSE);
    glClear(GL_STENCIL_BUFFER_BIT);

    // Draw floor
		m_floor_shader.enable();
		floor.draw(m_perpsective, m_view);
		m_floor_shader.disable();

    // Pass test if stencil value is 1
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    // Don't write anything to stencil buffer to only show reflection in plane and not under it
    glStencilMask(0x00);
    glDepthMask(GL_TRUE);
    m_shader.enable();
    // Add darkening effect to reflections
    GLint location = m_shader.getUniformLocation("overrideColor");
		glUniform3f(location, 0.4f, 0.4f, 0.4f);
		m_shader.disable();
		// Draw reflections
		ball.drawInverted(m_view);
    car.drawInverted(m_view);
    car2.drawInverted(m_view);
    m_shader.enable();
    glUniform3f(location, 1, 1, 1);
    m_shader.disable();
  	glDisable(GL_STENCIL_TEST);
	}
	else {
		if (!show_sky_cube) {
			// Draw arena
			arena.draw(m_view);
		}
	}

	double carSpeed = car.getSpeed();
	double car2Speed = car2.getSpeed();
	if (collision.carCarCollision(car, car2)) {
		if (carSpeed + car2Speed > 0.1) {
			SoundEngine->play2D("../A5/Assets/audio/bounce.wav", GL_FALSE);
		}
	}

	//bool ballHitsArena = collision.ballArenaCollision(ball, arena);

	// Car 1
	if (collision.carArenaCollision(car, arena, false)) {
		car.stop();
	} else {
		if (up_key_press) {
			car.moveForward();
		}
	}
	if (collision.carBallCollision(car, ball)) {
		if (car.getSpeed() < 0.01) {
			glm::vec3 ballDir = ball.getDir();
			ball.move(glm::vec3(-ballDir.x, ballDir.y, -ballDir.z), ball.getSpeed() * 0.8);
		} else {
			if (down_key_press) {
				ball.move(car.getDirBack(), 0.25);
			} else {
				glm::vec3 ballPos = ball.getPos();
				float delta = car.getSpeed() * 0.05;
				float amountX = car.getDir().x * delta;
				float amountZ = car.getDir().z * delta;
				if (ballPos.x + amountX + ball.getRadius() < 1.4 && ballPos.x + amountX - ball.getRadius() > -1.4 && ballPos.z + amountZ + ball.getRadius() < 0.7 && ballPos.z + amountZ - ball.getRadius() > -0.7) {
					ball.move(car.getDir(), car.getSpeed());
				}
			}
		}
	} 

	// Car 2
	if (collision.carArenaCollision(car2, arena, false)) {
		car2.stop();
	} else {
		if (w_key_press) {
			car2.moveForward();
		}
	}
	if (collision.carBallCollision(car2, ball)) {
		if (car2.getSpeed() < 0.01) {
			glm::vec3 ballDir = ball.getDir();
			ball.move(glm::vec3(-ballDir.x, ballDir.y, -ballDir.z), ball.getSpeed() * 0.8);
		} else {
			if (down_key_press) {
				ball.move(car2.getDirBack(), 0.25);
			} else {
				glm::vec3 ballPos = ball.getPos();
				float delta = car2.getSpeed() * 0.05;
				float amountX = car2.getDir().x * delta;
				float amountZ = car2.getDir().z * delta;
				if (ballPos.x + amountX + ball.getRadius() < 1.4 && ballPos.x + amountX - ball.getRadius() > -1.4 && ballPos.z + amountZ + ball.getRadius() < 0.7 && ballPos.z + amountZ - ball.getRadius() > -0.7) {
					ball.move(car2.getDir(), car2.getSpeed());
				}
			}
		}
	} 

	glDisable( GL_DEPTH_TEST );
	// renderArcCircle();
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A5::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A5::reset() {
	car = Car();
	car2 = Car();
	ball = Ball();

	std::unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});

	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	ball.init(*meshConsolidator, m_batchInfoMap, m_shader);
	car.init(*meshConsolidator, m_batchInfoMap, m_shader, "Assets/car.lua", glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0));
	car2.init(*meshConsolidator, m_batchInfoMap, m_shader, "Assets/car2.lua", glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0));
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A5::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A5::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A5::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A5::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS) {
		
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A5::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A5::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A5::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
	}

	//----------------------------------------------------------------------------------------
	/*
	 * Camera movement controls.
	 */
	if (key == GLFW_KEY_1) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			camera_x_plus_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			camera_x_plus_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_2) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			camera_x_minus_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			camera_x_minus_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_3) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			camera_y_plus_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			camera_y_plus_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_4) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			camera_y_minus_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			camera_y_minus_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_5) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			camera_z_plus_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			camera_z_plus_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_6) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			camera_z_minus_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			camera_z_minus_key_press = false;
		}
		eventHandled = true;
	}

	//----------------------------------------------------------------------------------------
	/*
	 * Light movement controls
	 */
	if (key == GLFW_KEY_7) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			light_x_plus_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			light_x_plus_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_8) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			light_x_minus_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			light_x_minus_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_9) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			light_y_plus_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			light_y_plus_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_0) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			light_y_minus_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			light_y_minus_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_MINUS) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			light_z_plus_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			light_z_plus_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_EQUAL) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			light_z_minus_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			light_z_minus_key_press = false;
		}
		eventHandled = true;
	}
	

	//----------------------------------------------------------------------------------------
	/*
	 * Car movement controls
	 */
	if (key == GLFW_KEY_RIGHT) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			right_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			right_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_LEFT) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			left_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			left_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_UP) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			up_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			up_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_DOWN) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			down_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			down_key_press = false;
		}
		eventHandled = true;
	}

	if (key == GLFW_KEY_W) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			w_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			w_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_A) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			a_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			a_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_S) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			s_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			s_key_press = false;
		}
		eventHandled = true;
	}
	if (key == GLFW_KEY_D) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			d_key_press = true;
		} 
		else if (action == GLFW_RELEASE) {
			d_key_press = false;
		}
		eventHandled = true;
	}

	return eventHandled;
}
