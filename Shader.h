#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader {
private:
	void loadShaderFile(const char* filepath, GLuint shader);

public:
	GLuint VBO, VAO;
	GLuint program;

	Shader(const char* vertexShaderFilepath, const char* fragmentShaderFilepath);

	void bind();
	void loadVBO(GLsizeiptr dataSize, const void* data, GLenum usage);
	void addAttrib(int layout, GLint size, GLenum type, GLboolean normalize, GLsizei stride, void* offset);
};


#endif
