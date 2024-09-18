#include "Shader.h"


Shader::Shader(const char* vertexShaderFilepath, const char* fragmentShaderFilepath) {
	// Set up buffers
	glGenVertexArrays(1, &(Shader::VAO));	// VAO allows us to store attribute configurations
	glBindVertexArray(Shader::VAO);

	glGenBuffers(1, &(Shader::VBO));		// VBO allows us to store vertex data
	glBindBuffer(GL_ARRAY_BUFFER, Shader::VBO);	


	// Load shaders
	GLuint vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	Shader::loadShaderFile(vertexShaderFilepath, vertex);

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	Shader::loadShaderFile(fragmentShaderFilepath, fragment);

	// Compile shaders
	Shader::program = glCreateProgram();

	glAttachShader(Shader::program, vertex);
	glAttachShader(Shader::program, fragment);

	glLinkProgram(Shader::program);

	// Clean up
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::bind() {
	glUseProgram(Shader::program);
	glBindVertexArray(Shader::VAO);
}

void Shader::loadShaderFile(const char* filepath, GLuint shader) {
	// Attempt to open shader file for reading
	std::ifstream in_file(filepath);
	if (!in_file.is_open()) {
		printf("[ERROR] Failed to open file '%s'", filepath);
		return;
	}


	// Read and compile file data
	std::stringstream in_buffer;
	in_buffer << in_file.rdbuf();

	std::string sourceString = in_buffer.str();
	const char* source = sourceString.c_str();

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);


	// Verify shader was compiled
	int shader_success;
	char shader_log[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_success);
	if (!shader_success) {
		glGetShaderInfoLog(shader, 512, NULL, shader_log);
		printf("[ERROR LOG] Failed to compile shader '%s':\n%s", filepath, shader_log);
	}

	// Release file
	in_file.close();
}

void Shader::loadVBO(GLsizeiptr dataSize, const void* data, GLenum usage) {
	// Buffers must be binded before loading data
	glBindVertexArray(Shader::VAO);
	glBindBuffer(GL_ARRAY_BUFFER, Shader::VBO);

	glBufferData( // Loads data onto the buffer
		GL_ARRAY_BUFFER,	// Type of buffer
		dataSize,			// Size of data
		data,				// Data
		usage);				// Expected use of data
}

void Shader::addAttrib(int layout, GLint size, GLenum type, GLboolean normalize, GLsizei stride, void* offset) {
	// Buffers must be binded before adding an attribute
	glBindVertexArray(Shader::VAO);
	glBindBuffer(GL_ARRAY_BUFFER, Shader::VBO);

	// Tells openGL how to interpret vertex data when feeding data to vertex shader
	glVertexAttribPointer(
		layout,		// Layout
		size,		// Size of vertex attribute
		type,		// Attribute data type
		normalize,	// Whether to normalize data
		stride,		// Total size of one complete set of data (this would need to be change upon adding another attribute)
		offset);	// Offset from beginning of attribute (in this case 0 since this is the first attribute)
	glEnableVertexAttribArray(layout);
}