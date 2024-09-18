#ifndef CHUNK_H
#define CHUNK_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "main.h"

class Chunk {
public:
    static inline GLuint textures[256];

    int chunkCoordX;
    int chunkCoordZ;

    int blocks[16][128][16];

    Chunk(int chunkCoordX, int chunkCoordZ);

    void draw(GLuint modelAttribLocation);
    void checkCollision(glm::vec3 position, glm::vec3* velocity, glm::vec3 hitbox, float deltatime);
    void checkInteraction(glm::vec3 position, glm::vec3 direction, int reach, glm::vec3* pBlockOutlinePosition, glm::vec3* pBlockPlacePosition);

    void breakBlock(glm::vec3 position);
    void placeBlock(glm::vec3 position, int block);

    static void loadBlockTextures(const char* folder);
};


#endif
