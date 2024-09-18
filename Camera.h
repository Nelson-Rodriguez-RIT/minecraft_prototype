#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "main.h"


class Camera {
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;

	float yaw;
	float pitch;
	float cursorLastX;
	float cursorLastY;

	bool mouseEnabled;
	float sensetivity;
	float FOV;


	Camera(glm::vec3 position);


	glm::mat4 generateViewMatrix();
	void updateFacingDirection(float xpos, float ypos);
};


#endif
