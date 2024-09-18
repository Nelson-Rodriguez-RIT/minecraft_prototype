#include "Camera.h"

Camera::Camera(glm::vec3 position) {
	// Handles camera's physical attributes
	Camera::position	   = position;
	Camera::front		   = glm::vec3(0.0f, 0.0f, 0.0f);
	Camera::up			   = glm::vec3(0.0f, 1.0f, 0.0f);

	// Used to change camera facing direction based on mouse movements
	Camera::yaw			  = 0.0f;
	Camera::pitch		  = 0.0f;
	Camera::cursorLastX  = DefaultWindowWidth / 2;
	Camera::cursorLastY  = DefaultWindowHeight / 2;

	// Camera attributes
	Camera::mouseEnabled = false;
	Camera::sensetivity  = DefaultSensetivity;
	Camera::FOV			 = DefaultFOV;
}

glm::mat4 Camera::generateViewMatrix() {
	return glm::lookAt(Camera::position, Camera::position + Camera::front, Camera::up);
}

void Camera::updateFacingDirection(float xpos, float ypos) {
	if (Camera::mouseEnabled) {
		// Update offsets
		Camera::yaw += (xpos - Camera::cursorLastX) * Camera::sensetivity;
		Camera::pitch += (Camera::cursorLastY - ypos) * Camera::sensetivity;

		// Prevents camera from rotating upside down
		if (Camera::pitch > 89.0f)
			Camera::pitch = 89.0f;
		if (Camera::pitch < -89.0f)
			Camera::pitch = -89.0f;

		// Generate new facing direction
		Camera::front = glm::normalize(glm::vec3(
			cos(glm::radians(Camera::yaw)) * cos(glm::radians(Camera::pitch)),
			sin(glm::radians(Camera::pitch)),
			sin(glm::radians(Camera::yaw)) * cos(glm::radians(Camera::pitch))
		));
	}

	Camera::cursorLastX = xpos;
	Camera::cursorLastY = ypos;
}