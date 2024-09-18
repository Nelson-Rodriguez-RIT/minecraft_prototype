#include "main.h"

float DeltaTime = 0.0f;

Chunk chunk = Chunk(0, 0);
Entity player = Entity(glm::vec3(0.0f, 3.0f, 0.0f));

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));


int main() {

	// Temp //
	chunk.blocks[0][1][0] = DIRT;
	chunk.blocks[1][1][0] = GRASS;
	chunk.blocks[0][1][1] = GRASS;
	chunk.blocks[1][1][1] = DIRT;



	// Used to determine delta time
	float currentFrame = 0.0f;
	float lastFrame = 0.0f; 


	stbi_set_flip_vertically_on_load(true);	// Prevents loaded textures from appearing upside down
	glfwInit();					// Initialize GLFW functionality

	// Configures GLFW for the desired openGL version and package
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Generates the primary window
	GLFWwindow* _window = glfwCreateWindow(
		DefaultWindowWidth, DefaultWindowHeight,	// Window dimensions
		":D",										// Window title
		NULL, NULL);								// Unused

	if (_window == NULL) { // Verify that window was successfully initialized
		printf("[ERROR] Failed to generate GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(_window);


	// Initialize GLAD (required for function pointers for openGL)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("[ERROR] Failed to initialize GLAD");
		glfwTerminate();
		return -1;
	}

	
	glViewport(0, 0, DefaultWindowWidth, DefaultWindowHeight);	// Configs openGL workable space within the window
	glEnable(GL_DEPTH_TEST);									// Activates the z buffer to draw based on depth

	// Registers openGL callbacks
	glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(_window, mouse_cursor_callback);

	Shader shader = Shader("world_vertex.glsl", "world_fragment.glsl");
	shader.loadVBO(
		sizeof(vertices_cube),
		vertices_cube,
		GL_STATIC_DRAW);

	shader.addAttrib( // Vertex coordinates
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			5 * sizeof(float),
			(void*)0);
	shader.addAttrib( // Texture coordinates
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			5 * sizeof(float),
			(void*)(3 * sizeof(float)));

	











	//// Generate buffers and loads them with relevant vertices, indices, and attributes
	//GLuint VBO, EBO, VAO;

	//glGenVertexArrays(1, &VAO); // VAO allows use to store attribute configurations
	//glBindVertexArray(VAO);

	//glGenBuffers(1, &VBO);					// Create buffer to store vertices 
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);		// Tell openGL we want to use this buffer
	//glBufferData(							// Loads data onto the buffer
	//	GL_ARRAY_BUFFER,		// Type of buffer
	//	sizeof(vertices_cube),  // Size of data
	//	vertices_cube,			// Data
	//	GL_STATIC_DRAW);		// Expected use of data

	//// Tells openGL how to interpret vertex data when feeding data to vertex shader
	//glVertexAttribPointer(
	//	0,					// Layout
	//	3,					// Size of vertex attribute
	//	GL_FLOAT,			// Attribute data type
	//	GL_FALSE,			// Whether to normalize data
	//	5 * sizeof(float),	// Total size of one complete set of data (this would need to be change upon adding another attribute)
	//	(void*)0);			// Offset from beginning of attribute (in this case 0 since this is the first attribute)
	//glEnableVertexAttribArray(0);

	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);



	//// Get and compile shader files
	//GLuint shader_program, shader_vertex, shader_fragment;

	//// Used for shader compilation/linking error reporting
	//int shader_success;
	//char shader_log[512];


	//shader_vertex = glCreateShader(GL_VERTEX_SHADER);						// Setup vertex shader

	//std::string shaderSource_vertex_buffer = loadShaderFile("world_vertex.glsl");	// Get source code from file
	//const char* shaderSource_vertex = shaderSource_vertex_buffer.c_str();	

	//glShaderSource(shader_vertex, 1, &shaderSource_vertex, NULL);			// Tell vertex shader to use this source code
	//glCompileShader(shader_vertex);											// Compile shader code

	//glGetShaderiv(shader_vertex, GL_COMPILE_STATUS, &shader_success);		// Verify if shader was compiled successfully
	//if (!shader_success) {											 
	//	glGetShaderInfoLog(shader_vertex, 512, NULL, shader_log);
	//	printf("[ERROR LOG] Failed to compile vertex shader...\n%s", shader_log);
	//}


	//shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);

	//std::string shaderSource_fragment_buffer = loadShaderFile("world_fragment.glsl");
	//const char* shaderSource_fragment = shaderSource_fragment_buffer.c_str();

	//glShaderSource(shader_fragment, 1, &shaderSource_fragment, NULL);
	//glCompileShader(shader_fragment);

	//glGetShaderiv(shader_fragment, GL_COMPILE_STATUS, &shader_success);
	//if (!shader_success) {
	//	glGetShaderInfoLog(shader_fragment, 512, NULL, shader_log);
	//	printf("[ERROR LOG] Failed to compile fragment shader...\n%s", shader_log);
	//}


	//// Generate a shader program with compiled shaders
	//shader_program = glCreateProgram();					// Create shader program

	//glAttachShader(shader_program, shader_vertex);		// Link vertex shader
	//glAttachShader(shader_program, shader_fragment);	// Link fragment shader

	//glLinkProgram(shader_program);						// Link all shaders together (allows them to communicate)

	//// Clean up
	//glDeleteShader(shader_vertex);
	//glDeleteShader(shader_fragment);



	// Textures
	Chunk::loadBlockTextures("textures/blocks/");
	glUniform1i(glGetUniformLocation(shader.program, "Texture"), 0);



	// Transformation Matrices //

	// Locations of model, view, and projection used to provide and update data
	int model_location = glGetUniformLocation(shader.program, "model");
	int view_location = glGetUniformLocation(shader.program, "view");
	int projection_location = glGetUniformLocation(shader.program, "projection");


	// Use to modify how the model appears
	glm::mat4 model = glm::mat4(1.0f); // Self Reminder: Always initialize matrices with glm::mat4(1.0f) before applying tansformations


	// Used to represent a camera
	glm::mat4 view;
	

	// Used to project 3D to a 2D screen (this shouldn't be modified often outside of FOV and resolution changes)
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(
		glm::radians(DefaultFOV),								// FOV
		(float)DefaultWindowWidth / (float)DefaultWindowHeight, // Window resolution ratio
		0.1f,													// How close before an object is clipped
		100.0f);												// How far befroe an object is clipped
	



	while (!glfwWindowShouldClose(_window)) {
		
		// Environment Update //

		glfwPollEvents(); // Enables window interaction

		// Update DeltaTime
		currentFrame = glfwGetTime();
		DeltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// Gameplay //

		// Set up environment
		player.acceleration = glm::vec3(0.0f, -15.0f, 0.0f); 
		player.velocity = glm::vec3(0.0f, player.velocity.y, 0.0f);

		// Get final veloctity
		staticInputHandler(_window); // Handles keyboard and mouse inputs that aren't reliant on being held down
		player.velocity += player.acceleration * DeltaTime;

		// Check if velocity leads to collision
		chunk.checkCollision(player.position, &player.velocity, player.hitbox, DeltaTime);

		// Apply velocity
		player.position += player.velocity * DeltaTime;
		//printf("x: %f, y: %f, z: %f\n", player.velocity.x, player.velocity.y, player.velocity.z);



		// Graphics //

		glClearColor(0.24f, 0.26f, 0.27f, 1.0f);				// Set background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Inform openGL to draw the background and reset the z buffer

		//glUseProgram(shader.program);
		//glActiveTexture(GL_TEXTURE0);
		//glBindVertexArray(shader.VAO);

		shader.bind();
		glActiveTexture(GL_TEXTURE0);
		

		glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

		// Gameworld
		camera.position = player.position + glm::vec3(0.0f, player.hitbox.y, 0.0f);
		view = camera.generateViewMatrix();
		glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));

		// GUI
		


		chunk.draw(model_location);

		glfwSwapBuffers(_window); // Swaps buffers, drawing new content to the screen
	}

	glfwTerminate();
}




void staticInputHandler(GLFWwindow* window) {
	// Exit application
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Enables/disables controlling the camera with the mouse
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		camera.mouseEnabled = true;
	}
		
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		camera.mouseEnabled = false;
	}
		
	player.inputHandler(window, camera);
}


std::string loadShaderFile(const char* filepath) {
	// Attempt to open shader file for reading
	std::ifstream in_file(filepath);
	if (!in_file.is_open()) {
		printf("[ERROR] Failed to open file '%s'", filepath);
		return "";
	}

	// Read file data
	std::stringstream in_buffer;
	in_buffer << in_file.rdbuf();

	// Release file
	in_file.close();

	return in_buffer.str();
}

GLuint loadTextureFile(const char* filepath) {
	// Load image data from file
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

	// Verify if data was read successfully
	if (!data) {
		printf("[ERROR] Failed to load image '%s'", filepath);
		stbi_image_free(data);
		return 0;
	}


	// Generate a texture to store the data
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Load data into texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unload image data from memory
	stbi_image_free(data);

	return textureID;
}

// Callbacks //

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	camera.updateFacingDirection(xpos, ypos);
}