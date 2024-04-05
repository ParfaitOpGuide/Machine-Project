#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

//GLM HEADERS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Light/Light.h"
#include "Light/PointLight.h"
#include "Light/DirectionLight.h"

#include "Object/ModelExtra.h"
#include "Object/Model3D.h"
#include "Object/Model.h"


#include "Camera/MyCamera.h"
#include "Camera/OrthoCamera.h"
#include "Camera/FirstPersonCamera.h"
#include "Camera/ThirdPersonCamera.h"

#include "ShaderClasses/Shader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//window size`
float width = 600.0f, height = 600.0f;

//Modifier for camera type 1 ThirdPerson 2 First Person 3 Ortho
int camType = 1;
//create cams
ThirdPersonCamera* ThirdPerson;
FirstPersonCamera* FirstPerson;
OrthoCamera* Orthographic;
//camera vector to store both cameras
std::vector<MyCamera*> vecCameras;

//lights
PointLight* Point = new PointLight(glm::vec3(-4, 5, 0));
DirectionLight* Direction = new DirectionLight(glm::vec3(0, 20, 0));



//uo direction, usually just 1 in y
glm::vec3 worldUp = glm::vec3(0, 1.0f, 0);


//for mouse movement
bool mouse_start = true;
bool mouse_pause = false;
double mouse_x_last;
double mouse_sens = 0.01;

//for model drawing
int drawAmount = 1;
std::vector<Model*> vecModels;

Model3D* MainModel;
ModelExtra* ExtraModel;
ModelExtra* ExtraModel2;
ModelExtra* ExtraModel3;
ModelExtra* ExtraModel4;
ModelExtra* ExtraModel5;
ModelExtra* ExtraModel6;

//timer function
float start, end;

//bool to close window with a button because im tired of having to press window when my mouse gets locked
bool close = false;

//light state switch
int lightState = 0;

//speed multiplier for movement
float fSpeed = 2.f;
float fOffset = 0.f;

void Key_Callback(GLFWwindow* window, //pointer to window
	int key, //keycode of press
	int scancode, //physical position of the press
	int action,//either press/release
	int mods) //modifier keys
{
	if (action == GLFW_REPEAT || action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_A: //camtype > 0 means can be controlled
			if (camType)
				MainModel->yaw = MainModel->yaw + 10;
			break;
		case GLFW_KEY_D:
			if (camType)
				MainModel->yaw = MainModel->yaw - 10;
			break;
			//WS = forward backward
		case GLFW_KEY_W:
			if (camType) { //push forward in direction
				if (fOffset) {
					MainModel->x -= MainModel->objDir.x * fSpeed * fOffset;
					MainModel->y -= MainModel->objDir.y * fSpeed * fOffset;
					MainModel->z -= MainModel->objDir.z * fSpeed * fOffset;
				}
				else {
					MainModel->x -= MainModel->objDir.x * fSpeed;
					MainModel->y -= MainModel->objDir.y * fSpeed;
					MainModel->z -= MainModel->objDir.z * fSpeed;
				}
			}
			break;
		case GLFW_KEY_S:
			if (camType) { // push backwards in direction
				if (fOffset) {
					MainModel->x += MainModel->objDir.x * fSpeed * fOffset;
					MainModel->y += MainModel->objDir.y * fSpeed * fOffset;
					MainModel->z += MainModel->objDir.z * fSpeed * fOffset;
				}
				else {
					MainModel->x += MainModel->objDir.x * fSpeed;
					MainModel->y += MainModel->objDir.y * fSpeed;
					MainModel->z += MainModel->objDir.z * fSpeed;
				}
			}
			break;
			//QE = Z Axis
		case GLFW_KEY_Q:
			if (camType)
				MainModel->y = MainModel->y + (fSpeed/14);
			break;
		case GLFW_KEY_E:
			if (camType)
				MainModel->y = MainModel->y - (fSpeed / 14);
			break;
		case GLFW_KEY_F: // point changes when f pressed
			switch (lightState) {
			case 0: //low
				Point->strength = 150;
				lightState = 1;
				break;
			case 1: //med
				Point->strength = 2000;
				lightState = 2;
				break;
			case 2: //hi
				Point->strength = 5000;
				lightState = 0;
				break;
			}
			std::cout << Point->strength;
			break;
		case GLFW_KEY_UP: //orthographic offsets only enabled when ortho is on
			if (camType == 0)
				Orthographic->vecOffset.z += 0.3f;
			break;
		case GLFW_KEY_DOWN:
			if (camType == 0)
				Orthographic->vecOffset.z -= 0.3f;
			break;

		case GLFW_KEY_LEFT: 
			if (camType == 0)
				Orthographic->vecOffset.x += 0.3f;
			break;
		case GLFW_KEY_RIGHT:
			if (camType == 0)
				Orthographic->vecOffset.x -= 0.3f;
			break;
		case GLFW_KEY_1: // set camtype to 1 for third or 2 first
			if (camType != 1) {
				camType = 1;
				MainModel->scale = .03f; //return to reg scale
				fOffset = 0.f;
			}
			else
			{
				camType = 2;
				MainModel->scale = .0003f; //100 times smaller to give the illusion its invis
				fOffset = 100.f;
			}
			mouse_pause = false;
			break;
		case GLFW_KEY_2: //camtype to 0 for ortho
			camType = 0;
			mouse_start = true;
			mouse_pause = true;
			//reset offset
			Orthographic->vecOffset = { 0,0,0 };
			break;

		case GLFW_KEY_ESCAPE:
			close = true;
			break;
		}
	}
}

void Mouse_Callback(GLFWwindow* window, double mouse_x_mod, double mouse_y_mod) {
	if (mouse_start && !mouse_pause) {
		mouse_x_last = mouse_x_mod; //sets up last known mouse pos
		mouse_start = false;
	}

	if (!mouse_pause) {
		ThirdPerson->cam_theta_mod += (mouse_x_mod - mouse_x_last) * mouse_sens;
		mouse_x_last = mouse_x_mod;
	}
}

int main(void)
{
	GLFWwindow* window;


	/* Initialize the library */
	if (!glfwInit())
		return -1;

	float width = 600.0f, height = 600.0f;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Submarine", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}



	//IDENTITY MATRIXES
	glm::mat3 identity_matrix3 = glm::mat3(1.0f);
	glm::mat4 identity_matrix4 = glm::mat4(1.0f);

	//create shader
	Shader* ShaderSkybox = new Shader(window, "Shaders/skyblock.vert", "Shaders/skyblock.frag");
	Shader* Shader1 = new Shader(window, "Shaders/sampleNew.vert", "Shaders/sampleNew.frag");
	Shader* ShaderNormals = new Shader(window, "Shaders/normals.vert", "Shaders/normals.frag");

	std::vector<Shader*> vecShaders;
	vecShaders.push_back(ShaderNormals);
	vecShaders.push_back(Shader1);
	//create objects and pass the path to the things needed for it specifically, obj first then tex, then normal if needed
	
	MainModel = new Model3D(window, { 0, 0, 0 }, Shader1->shaderProg, "3D/YellowSub/SubLow0Smooth.obj", "3D/YellowSub/SubLow0Smooth_DefaultMaterial_BaseColor.png", .03f);
	ExtraModel = new ModelExtra(window, { -20, -20, 2 }, Shader1->shaderProg, "3D/nitrosub/nitro sub.obj", "3D/nitrosub/nitro sub.jpg", 1.f, 0, 0, 0);
	ExtraModel2 = new ModelExtra(window, { -55, -15, 1 }, Shader1->shaderProg, "3D/Sub406/Sub406.obj", "3D/Sub406/Subs Shadow Edited Tex.png", .1f, 0, 0, 0);
	ExtraModel3 = new ModelExtra(window, { -5, -10, 15 }, Shader1->shaderProg, "3D/capsule/Capsule_done.obj", "3D/capsule/Default_Base_Color.png", .03f, 0, 0, 0);
	ExtraModel4 = new ModelExtra(window, { -40, -20, 5 }, Shader1->shaderProg, "3D/Nemo-240K/Nemo-240K.obj", "3D/Nemo-240K/Nemo-240K01.jpg", 0.3f, -90, 0, 0);
	ExtraModel5 = new ModelExtra(window, { -90, -60, -10 }, Shader1->shaderProg, "3D/Retopo/Retopo.OBJ", "3D/Retopo/retopo_Transferred Texture from Mesh.jpg", .05f, 0, 0, 0);
	ExtraModel6 = new ModelExtra(window, { -130, -39, 1 }, Shader1->shaderProg, "3D/saca/sacabambaspis7_3.obj", "3D/saca/sacabambaspis7_3_initialShadingGroup_BaseColor.png", 0.5f, 0, 0, 0);


	ThirdPerson = new ThirdPersonCamera(MainModel, worldUp, height, width);
	FirstPerson = new FirstPersonCamera(MainModel, worldUp, height, width);
	Orthographic = new OrthoCamera(MainModel, worldUp);
	
	//Load shader file into a strin steam
	std::fstream vertSrc("Shaders/sampleNew.vert");
	std::stringstream vertBuff;
	std::fstream skyboxVertSrc("Shaders/skyblock.vert");
	std::stringstream skyboxVertBuff;

	vertBuff << vertSrc.rdbuf();
	skyboxVertBuff << skyboxVertSrc.rdbuf();

	std::string vertS = vertBuff.str();
	const char* v = vertS.c_str();
	std::string skyboxVertS = skyboxVertBuff.str();
	const char* sky_v = skyboxVertS.c_str();

	//same but for frag
	std::fstream fragSrc("Shaders/sampleNew.frag");
	std::stringstream fragBuff;
	std::fstream skyboxFragSrc("Shaders/skyblock.frag");
	std::stringstream skyboxFragBuff;

	fragBuff << fragSrc.rdbuf();
	skyboxFragBuff << skyboxFragSrc.rdbuf();

	std::string fragS = fragBuff.str();
	const char* f = fragS.c_str();
	std::string skyboxFragS = skyboxFragBuff.str();
	const char* sky_f = skyboxFragS.c_str();
	
	

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	gladLoadGL();


	//set the callback function to the window
	glfwSetKeyCallback(window, Key_Callback);
	//maus
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, Mouse_Callback);
	


	/*
  7--------6
 /|       /|
4--------5 |
| |      | |
| 3------|-2
|/       |/
0--------1
*/
//Vertices for the cube
	float skyboxVertices[]{
		-1.f, -1.f, 1.f, //0
		1.f, -1.f, 1.f,  //1
		1.f, -1.f, -1.f, //2
		-1.f, -1.f, -1.f,//3
		-1.f, 1.f, 1.f,  //4
		1.f, 1.f, 1.f,   //5
		1.f, 1.f, -1.f,  //6
		-1.f, 1.f, -1.f  //7
	};

	//Skybox Indices
	unsigned int skyboxIndices[]{
		1,2,6,
		6,5,1,

		0,4,7,
		7,3,0,

		4,5,6,
		6,7,4,

		0,3,2,
		2,1,0,

		0,1,5,
		5,4,0,

		3,7,6,
		6,2,3
	};

	std::string facesSkybox[]
	{
		"Skybox/bhop_otter-1RT.png",
		"Skybox/bhop_otter-1LF.png",
		"Skybox/bhop_otter-1UP.png",
		"Skybox/bhop_otter-1DN.png",
		"Skybox/bhop_otter-1FT.png",
		"Skybox/bhop_otter-1BK.png",
	};
	unsigned int skyboxTex;

	glGenTextures(1, &skyboxTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < 6; i++) {
		int w, h, skyCChannel;
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load(facesSkybox[i].c_str(), &w, &h, &skyCChannel, 0);

		if (data) {
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				w,
				h,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);

			stbi_image_free(data);
		}

	}

	stbi_set_flip_vertically_on_load(true);


	


	GLuint skyboxVBO, skyboxVAO, skyboxEBO; //openGL unsigned int
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);


	

	glBindVertexArray(skyboxVAO);
	// editing VBO = null

	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(float),
		(void*)0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//blend yo mama
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, //source factor
		GL_ONE_MINUS_SRC_ALPHA);

	//camera stuff gets pushed into the vector
	vecCameras.push_back(Orthographic);
	vecCameras.push_back(ThirdPerson);
	vecCameras.push_back(FirstPerson);
	

	//init obj
	vecModels.push_back(MainModel);
	vecModels.push_back(ExtraModel);
	vecModels.push_back(ExtraModel2);
	vecModels.push_back(ExtraModel3);
	vecModels.push_back(ExtraModel4);
	vecModels.push_back(ExtraModel5);
	vecModels.push_back(ExtraModel6);


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window) && !close)
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		//processes events depending on camtype
		vecCameras[camType]->processEvents({ -2 * vecModels[0]->x,  -2 * vecModels[0]->y, -2*vecModels[0]->z }, MainModel->objDir);

		

		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);
		glUseProgram(ShaderSkybox->shaderProg);

		glm::mat4 sky_view = glm::mat4(1.f);
		sky_view = glm::mat4(glm::mat3(vecCameras[camType]->viewMatrix));

		unsigned int skyboxViewLoc = glGetUniformLocation(ShaderSkybox->shaderProg, "view");
		glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));
		unsigned int skyboxProjLoc = glGetUniformLocation(ShaderSkybox->shaderProg, "projection");
		glUniformMatrix4fv(skyboxProjLoc, 1, GL_FALSE, glm::value_ptr(vecCameras[camType]->projectionMatrix));
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		int shaderCount;
		//pass all necessary drawing stuff to the objects draw, call all objects draw in vector
		for (int i = 0; i < vecModels.size(); i++) {

			
			//std::cout << shaderCount << std::endl;
			vecModels[i]->processEvents(identity_matrix4,
				ShaderNormals->shaderProg,
				vecCameras[camType],
				Point, Direction);
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	for (int i = 0; i < vecModels.size(); i++) {
		vecModels[i]->explodeAndDie(); //deletes all vaos since we cant do that from main anymore
	}
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &skyboxEBO);


	glfwTerminate();
	return 0;
}