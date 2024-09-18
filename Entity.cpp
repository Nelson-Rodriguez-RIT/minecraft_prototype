#include "Entity.h"

Entity::Entity(glm::vec3 position, bool cameraView) {
	Entity::position	 = position;
	Entity::velocity	 = glm::vec3();
	Entity::acceleration = glm::vec3();

	// Theses should be set individually by their respective entities
	Entity::hitbox = glm::vec3(0.6f, 1.8f, 0.6f);
	Entity::movementSpeed = 4.317f;

	Entity::cameraView = cameraView;
}


void Entity::inputHandler(GLFWwindow* window, Camera camera) {

	// WASD
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Entity::velocity += Entity::movementSpeed * glm::normalize((camera.front * glm::vec3(1.0f, 0.0f, 1.0f)));
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Entity::velocity += -Entity::movementSpeed * glm::normalize((camera.front * glm::vec3(1.0f, 0.0f, 1.0f)));

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Entity::velocity += -Entity::movementSpeed * glm::normalize(glm::cross(camera.front * glm::vec3(1.0f, 0.0f, 1.0f), camera.up));
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Entity::velocity += Entity::movementSpeed * glm::normalize(glm::cross(camera.front * glm::vec3(1.0f, 0.0f, 1.0f), camera.up));


	// Jump
	if (Entity::velocity.y == 0.0f && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		Entity::velocity.y += 6.0f;
}

void Entity::draw(GLuint modelAttribLocation) {
	if (Entity::cameraView) return;

	glBindTexture(GL_TEXTURE_2D, Entity::texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because we want the pixel look
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glm::mat4 model = glm::translate(glm::mat4(1.0f), Entity::position);
	glUniformMatrix4fv(modelAttribLocation, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Entity::loadTexture(const char* filepath) {
	// Load image data from file
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

	// Verify if data was read successfully
	if (!data) {
		printf("[ERROR] Failed to load image '%s'", filepath);
		stbi_image_free(data);
		return;
	}


	// Generate a texture to store the data
	glGenTextures(1, &(Entity::texture));
	glBindTexture(GL_TEXTURE_2D, Entity::texture);

	// Load data into texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unload image data from memory
	stbi_image_free(data);
}