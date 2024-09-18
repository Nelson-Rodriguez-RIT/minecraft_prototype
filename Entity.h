#ifndef ENTITY_H
#define ENTITY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

class Camera;


class Entity {
public:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	// Hitbox origin is x/2, 0, z/2
	glm::vec3 hitbox;
	float movementSpeed;

	GLuint texture;

	GLuint cameraTexture;
	bool cameraView;



	Entity(glm::vec3 position = glm::vec3(), bool cameraView = false);


	void inputHandler(GLFWwindow* window, Camera camera);
	void draw(GLuint modelAttribLocation);
	void loadTexture(const char* filepath);
};



#endif