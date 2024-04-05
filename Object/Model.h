#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>

//GLM HEADERS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Light/PointLight.h"
#include "../Light/DirectionLight.h"

#include "../Camera/MyCamera.h"

#include "../tiny_obj_loader.h"
#include "../stb_image.h"

class Model {
public:
	float x,
		y,
		z,
		scale,
		opacity;

	float thetaX, thetaY, thetaZ;
	//model data required for drawing, same for all models so its inherited
	GLuint texture;
	std::vector<float> fullVertexData;
	GLuint VAO, VBO;

public:
	Model(GLFWwindow* window, glm::vec3 vecPos, GLuint shaderProg, std::string objStr, std::string texStr, std::string normStr);
	Model(GLFWwindow* window, glm::vec3 vecPos, GLuint shaderProg, std::string objStr, std::string texStr); //different constructors based on whether normals are involved or not

	void explodeAndDie();

	virtual void processEvents(glm::mat4 identity_matrix4,
		GLuint shaderProg,
		MyCamera* Camera,
		PointLight* Point, DirectionLight* Direction) = 0;

	virtual void draw(glm::mat4 identity_matrix4,
		GLuint shaderProg,
		MyCamera* Camera, //next is light content
		PointLight* Point, DirectionLight* Direction) = 0;

};

