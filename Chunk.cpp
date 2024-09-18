#include "Chunk.h"

Chunk::Chunk(int chunkCoordX, int chunkCoordZ) : blocks() {
	Chunk::chunkCoordX = chunkCoordX;
	Chunk::chunkCoordZ = chunkCoordZ;

	for (int x = 0; x < 16; x++)
		for (int z = 0; z < 16; z++)
			Chunk::blocks[x][0][z] = BEDROCK;
}

void Chunk::draw(GLuint modelAttribLocation) {
	for (int x = 0; x < 16; x++)
		for (int y = 0; y < 128; y++)
			for (int z = 0; z < 16; z++) {
				if (blocks[x][y][z] == AIR) continue;

				glBindTexture(GL_TEXTURE_2D, Chunk::textures[blocks[x][y][z]]);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because we want the pixel look
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glm::vec3 blockPosition = glm::vec3(
					x + Chunk::chunkCoordX * 16,
					y,
					z + Chunk::chunkCoordZ * 16
				);

				glm::mat4 model = glm::translate(glm::mat4(1.0f), blockPosition);
				glUniformMatrix4fv(modelAttribLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
}

void Chunk::checkCollision(glm::vec3 position, glm::vec3* velocity, glm::vec3 hitbox, float deltatime) {
	glm::vec3 nextPosition = position + (*velocity * deltatime);

	for (int x = 0; x < 16; x++)
		for (int y = 0; y < 128; y++)
			for (int z = 0; z < 16; z++) {
				if (blocks[x][y][z] == AIR) continue;

				glm::vec3 blockPosition = glm::vec3(
					x + Chunk::chunkCoordX * 16,
					y,
					z + Chunk::chunkCoordZ * 16
				);

				// Hitbox origin for entities is x/2, 0, z/2
				if (velocity->x != 0.0f &&
					nextPosition.x - hitbox.x / 2 < blockPosition.x && nextPosition.x + hitbox.x / 2 > blockPosition.x - 1.0f &&
						position.y 				  < blockPosition.y &&	  position.y + hitbox.y		> blockPosition.y - 1.0f &&
						position.z - hitbox.z / 2 < blockPosition.z &&	  position.z + hitbox.z / 2 > blockPosition.z - 1.0f
					)
					velocity->x = 0.0f;

				if (velocity->y != 0.0f &&
						position.x - hitbox.x / 2 < blockPosition.x &&	  position.x + hitbox.x / 2 > blockPosition.x - 1.0f &&
					nextPosition.y 				  < blockPosition.y && nextPosition.y + hitbox.y		> blockPosition.y - 1.0f &&
						position.z - hitbox.z / 2 < blockPosition.z &&	  position.z + hitbox.z / 2 > blockPosition.z - 1.0f
					)
					velocity->y = 0.0f;

				if (velocity->z != 0.0f &&
						position.x - hitbox.x / 2 < blockPosition.x &&	  position.x + hitbox.x / 2 > blockPosition.x - 1.0f &&
						position.y 				  < blockPosition.y &&	  position.y + hitbox.y		> blockPosition.y - 1.0f &&
					nextPosition.z - hitbox.z / 2 < blockPosition.z && nextPosition.z + hitbox.z / 2 > blockPosition.z - 1.0f
					)
					velocity->z = 0.0f;

			}
}

void Chunk::checkInteraction(glm::vec3 position, glm::vec3 direction, int reach, glm::vec3* pBlockOutlinePosition, glm::vec3* pBlockPlacePosition) {
	float accuracy = 50;
	glm::vec3 raycast = position;

	for (int step = 0; step < reach * accuracy; step++) {
		for (int x = 0; x < 16; x++)
			for (int y = 0; y < 128; y++)
				for (int z = 0; z < 16; z++) {
					if (blocks[x][y][z] == AIR) continue;

					glm::vec3 blockPosition = glm::vec3(
						x + Chunk::chunkCoordX * 16,
						y,
						z + Chunk::chunkCoordZ * 16
					);

					if (raycast.x < blockPosition.x && raycast.x > blockPosition.x - 1.0f &&
						raycast.y < blockPosition.y && raycast.y > blockPosition.y - 1.0f &&
						raycast.z < blockPosition.z && raycast.z > blockPosition.z - 1.0f
						) {
						*pBlockOutlinePosition = glm::vec3((int)glm::ceil(raycast.x), (int)glm::ceil(raycast.y), (int)glm::ceil(raycast.z));

						raycast -= direction * (1.0f / (float)accuracy);
						*pBlockPlacePosition = glm::vec3((int)glm::ceil(raycast.x), (int)glm::ceil(raycast.y), (int)glm::ceil(raycast.z));
						return;
					}
				}

		raycast += direction * (1.0f / (float)accuracy);
	}


	*pBlockOutlinePosition = glm::vec3(0.0f, -16.0f, 0.0f); // Signal that interaction failed
	*pBlockPlacePosition   = glm::vec3(0.0f, -16.0f, 0.0f);
}


void Chunk::loadBlockTextures(const char* folder) {
	glPixelStorei(GL_PACK_ALIGNMENT, 4);

	// Load block textures
	int blockID = DIRT;
	std::string filepath;
	bool transparent = false;

	for (int blockID = AIR; blockID <= 5; blockID++) {
		switch (blockID) {
		case OUTLINE:
			filepath = std::string(folder).append("outline.png");
			transparent = true;
			break;

		case DIRT:
			filepath = std::string(folder).append("dirt.png");
			break;

		case GRASS:
			filepath = std::string(folder).append("grass.png");
			break;

		case COBBLESTONE:
			filepath = std::string(folder).append("cobblestone.png");
			break;
			

		case BEDROCK:
			filepath = std::string(folder).append("bedrock.png");
			break;
		}

		// Load image data from file
		int width, height, nrChannels;
		const char* filepath_c = filepath.c_str();
		unsigned char* data = stbi_load(filepath_c, &width, &height, &nrChannels, 0);

		// Verify if data was read successfully
		if (!data) {
			printf("[ERROR] Failed to load image '%s'\n", filepath_c);
			stbi_image_free(data);
			continue;
		}

		// Generate a texture to store the data
		glGenTextures(1, &Chunk::textures[blockID]);
		glBindTexture(GL_TEXTURE_2D, Chunk::textures[blockID]);

		// Set parameters to tell openGL how to draw the image
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Load data into texture
		if (blockID == OUTLINE)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);

		

		// Unload image data from memory
		stbi_image_free(data);
	}
}




void Chunk::breakBlock(glm::vec3 position) {
	if (Chunk::blocks[(int)position.x][(int)position.y][(int)position.z] != AIR)
		Chunk::blocks[(int)position.x][(int)position.y][(int)position.z] = AIR;
}

void Chunk::placeBlock(glm::vec3 position, int block) {
	if(Chunk::blocks[(int)position.x][(int)position.y][(int)position.z] == AIR)
		Chunk::blocks[(int)position.x][(int)position.y][(int)position.z] = block;
}