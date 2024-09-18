#ifndef MAIN_H
#define MAIN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

#include "Entity.h"
#include "Camera.h"
#include "Chunk.h"
#include "Shader.h"


const int DefaultWindowWidth = 500;
const int DefaultWindowHeight = 500;

const float DefaultFOV = 90.0f;
const float DefaultSensetivity = 0.1f;


struct Character {
    GLuint     id;      // Texture location within memory
    glm::ivec2 size;    // Size of glypth
    glm::ivec2 bearing; // Offset from baseline to top/left of glypth
    GLuint     advance; // Offset to advance to next glypth
};

enum BlockID {
    AIR,
    OUTLINE,

    DIRT,
    GRASS,
    COBBLESTONE,
    BEDROCK
};

enum ItemID {
    ITEM_NONE, // Displayed as the character hands when held
    ITEM_SLOT_RESERVED, // For convience I wanted to match block ids with there respecitive item

    ITEM_DIRT,
    ITEM_GRASS,
    ITEM_COBBLESTONE,
    ITEM_BEDROCK,
};



// Models
const float vertices_cube[] = {
    // Vertex Positions	// Texture Coordinates
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
     0.0f, -1.0f, -1.0f,  1.0f, 0.0f,
     0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -1.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,

    -1.0f, -1.0f,  0.0f,  0.0f, 0.0f,
     0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f,  0.0f, 0.0f,

    -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -1.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.0f, -1.0f, -1.0f,  0.0f, 1.0f,
     0.0f, -1.0f, -1.0f,  0.0f, 1.0f,
     0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
     0.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
     0.0f, -1.0f, -1.0f,  1.0f, 1.0f,
     0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -1.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,

    -1.0f,  0.0f, -1.0f,  0.0f, 1.0f,
     0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -1.0f,  0.0f, -1.0f,  0.0f, 1.0f
};

const float vertices_button[] = {
    -0.4f, -0.10f, 0.0f,  0.0f, 0.0f,
    -0.4f,  0.10f, 0.0f,  0.0f, 1.0f,
     0.4f,  0.10f, 0.0f,  1.0f, 1.0f,

    -0.4f, -0.10f, 0.0f,  0.0f, 0.0f,
     0.4f, -0.10f, 0.0f,  1.0f, 0.0f,
     0.4f,  0.10f, 0.0f,  1.0f, 1.0f,
};

const float vertices_crosshair[] = {
    -0.05f, -0.05f, 0.0f,  0.0f, 0.0f,
    -0.05f,  0.05f, 0.0f,  0.0f, 1.0f,
     0.05f, -0.05f, 0.0f,  1.0f, 0.0f,

    -0.05f,  0.05f, 0.0f,  0.0f, 1.0f,
     0.05f,  0.05f, 0.0f,  1.0f, 1.0f,
     0.05f, -0.05f, 0.0f,  1.0f, 0.0f,
};

const float vertices_player[] = {
    -0.3f, -0.0f, -0.3f, 0.0f, 0.0f, // Front
    -0.3f,  1.6f, -0.3f, 0.0f, 1.0f,
     0.3f, -0.0f, -0.3f, 1.0f, 0.0f,

    -0.3f,  1.6f, -0.3f, 0.0f, 1.0f,
     0.3f,  1.6f, -0.3f, 1.0f, 1.0f,
     0.3f, -0.0f, -0.3f, 1.0f, 0.0f,

    -0.3f, -0.0f,  0.3f, 0.0f, 0.0f, // Back
    -0.3f,  1.6f,  0.3f, 0.0f, 1.0f,
     0.3f, -0.0f,  0.3f, 1.0f, 0.0f,

    -0.3f,  1.6f,  0.3f, 0.0f, 1.0f,
     0.3f,  1.6f,  0.3f, 1.0f, 1.0f,
     0.3f, -0.0f,  0.3f, 1.0f, 0.0f,

    -0.3f, -0.0f, -0.3f, 0.0f, 0.0f, // Left side
    -0.3f,  1.6f, -0.3f, 0.0f, 1.0f,
    -0.3f, -0.0f,  0.3f, 1.0f, 0.0f,

    -0.3f,  1.6f, -0.3f, 0.0f, 1.0f,
    -0.3f,  1.6f,  0.3f, 1.0f, 1.0f,
    -0.3f, -0.0f,  0.3f, 1.0f, 0.0f,

     0.3f, -0.0f, -0.3f, 0.0f, 0.0f, // Right side
     0.3f,  1.6f, -0.3f, 0.0f, 1.0f,
     0.3f, -0.0f,  0.3f, 1.0f, 0.0f,

     0.3f,  1.6f, -0.3f, 0.0f, 1.0f,
     0.3f,  1.6f,  0.3f, 1.0f, 1.0f,
     0.3f, -0.0f,  0.3f, 1.0f, 0.0f,
};

const float vertices_hand[] = {
    -0.15f, -0.15f,  0.0f,  0.0f, 0.0f, // Front
    -0.15f,  0.15f,  0.0f,  0.0f, 1.0f,
     0.15f,  0.15f,  0.0f,  1.0f, 1.0f,

    -0.15f, -0.15f,  0.0f,  0.0f, 0.0f,
     0.15f, -0.15f,  0.0f,  1.0f, 0.0f,
     0.15f,  0.15f,  0.0f,  1.0f, 1.0f,


    -0.15f,  0.15f,  0.0f,  0.0f, 0.0f, // Top
    -0.15f,  0.15f, -1.0f,  0.0f, 1.0f,
     0.15f,  0.15f, -1.0f,  1.0f, 1.0f,

    -0.15f,  0.15f,  0.0f,  0.0f, 0.0f,
     0.15f,  0.15f,  0.0f,  1.0f, 0.0f,
     0.15f,  0.15f, -1.0f,  1.0f, 1.0f,

    -0.15f, -0.15f,  0.0f,  0.0f, 0.0f, // Left
    -0.15f, -0.15f, -1.0f,  0.0f, 1.0f,
    -0.15f,  0.15f, -1.0f,  1.0f, 1.0f,

    -0.15f, -0.15f,  0.0f,  0.0f, 0.0f,
    -0.15f,  0.15f,  0.0f,  1.0f, 0.0f,
    -0.15f,  0.15f, -1.0f,  1.0f, 1.0f,
};

const float vertices_hotbar[] = {
    -0.45f, -1.0f, 0.0f,  0.0f, 0.0f,
    -0.45f, -0.9f, 0.0f,  0.0f, 1.0f,
     0.45f, -0.9f, 0.0f,  1.0f, 1.0f,

    -0.45f, -1.0f, 0.0f,  0.0f, 0.0f,
     0.45f, -1.0f, 0.0f,  1.0f, 0.0f,
     0.45f, -0.9f, 0.0f,  1.0f, 1.0f,
};

//const float vertices_hotbarSelect[] = {
//    1.0f,
//};

const float vertices_hotbarSlot[] = {
    -0.035f, -0.985f, 0.0f,  0.0f, 0.0f,
    -0.035f, -0.915f, 0.0f,  0.0f, 1.0f,
     0.035f, -0.915f, 0.0f,  1.0f, 1.0f,

    -0.035f, -0.985f, 0.0f,  0.0f, 0.0f,
     0.035f, -0.985f, 0.0f,  1.0f, 0.0f,
     0.035f, -0.915f, 0.0f,  1.0f, 1.0f,



    /*-0.465f, -9.85f, 0.0f,  0.0f, 0.0f,
    -0.465f, -9.15f, 0.0f,  0.0f, 1.0f,
    -0.365f, -9.15f, 0.0f,  1.0f, 1.0f,

    -0.465f, -9.85f, 0.0f,  0.0f, 0.0f,
    -0.365f, -9.15f, 0.0f,  1.0f, 0.0f,
    -0.365f, -9.15f, 0.0f,  1.0f, 1.0f,*/
};

const float vertices_activeHotbarSlot[] = {
    -0.05f, -1.0f, 0.0f,  0.0f, 0.0f,
    -0.05f, -0.9f, 0.0f,  0.0f, 1.0f,
     0.05f, -0.9f, 0.0f,  1.0f, 1.0f,

    -0.05f, -1.0f, 0.0f,  0.0f, 0.0f,
     0.05f, -1.0f, 0.0f,  1.0f, 0.0f,
     0.05f, -0.9f, 0.0f,  1.0f, 1.0f,
};


void dynamicInputHandler(GLFWwindow* window);
std::string loadShaderFile(const char* filepath);
GLuint loadTextureFile(const char* filepath, GLuint type = GL_RGB);
void renderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color);


// TODO: add global functions via a header named global.h
// TODO: add global collision checking function

// Callbacks //
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#endif
