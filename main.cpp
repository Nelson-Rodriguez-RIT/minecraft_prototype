#include "main.h"

/***********************************************************************************************
This is a prototype. Please don't assume this is how I intended everything to work/ be designed.
************************************************************************************************/




float DeltaTime = 0.0f;


bool menu_activate;

Chunk chunk = Chunk(0, 0);
Entity player = Entity(glm::vec3(0.0f, 3.0f, 0.0f), true);

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));

std::map<char, Character> Characters;

float blockBreakTimer = 0.0f;
float blockPlaceTimer = 0.0f;


bool breakBlock = false;
bool placeBlock = false;


ItemID hotbar[9];
int activeHotbarSlot = 0;


bool menuButtonHighlighted = false;

int main() {

	// Temp //
	chunk.blocks[0][1][0] = DIRT;
	chunk.blocks[1][1][0] = GRASS;
	chunk.blocks[2][1][0] = COBBLESTONE;
	chunk.blocks[3][1][0] = BEDROCK;

	hotbar[0] = ITEM_DIRT;
	hotbar[1] = ITEM_GRASS; 
	hotbar[2] = ITEM_COBBLESTONE;
	hotbar[3] = ITEM_BEDROCK;


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
	glEnable(GL_BLEND);											// Allows for color bending in the fragment shader
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);

	// Registers openGL callbacks
	glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(_window, mouse_cursor_callback);
	glfwSetKeyCallback(_window, keyboard_callback);
	glfwSetMouseButtonCallback(_window, mouse_button_callback);
	glfwSetScrollCallback(_window, mouse_scroll_callback);


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
	


	// Font loading //

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		printf("[ERROR] Could not load FreeType Library\n");
		return -1;
	}

	FT_Face face;
	if (FT_New_Face(ft, "fonts/minecraft.ttf", 0, &face)) {
		printf("[ERROR] Could not load font");
		return -1;
	}

	FT_Set_Pixel_Sizes(face, 0, 48); // Set font size

	// Loads the basic 8-bit printable characters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 	// Removes the forced 4-byte alignment
	for (unsigned int c = 0; c < 128; c++) {
		// Load the glypth through FreeType
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			printf("Failed to load glyph '%c'", c);
			continue;
		}

		// Generate texture for glyph
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer);

		// Set texture settings
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because we want the pixel look
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Store this information for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}

	

	// Clean up
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Prepare gui related elements (textures and text)
	glm::mat4 projection_gui = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
	glm::mat4 model_gui = glm::mat4(1.0f);

	GLuint tempGuiHighlightButton = loadTextureFile("textures/gui/menu_button_highlight.png");
	GLuint tempGuiButton	 = loadTextureFile("textures/gui/menu_button.png");
	GLuint crosshairTexture  = loadTextureFile("textures/gui/crosshair.png", GL_RGBA);
	GLuint hotbarTexture	 = loadTextureFile("textures/gui/hotbar.png", GL_RGBA);
	GLuint hotbarActivateTex = loadTextureFile("textures/gui/hotbar_select.png", GL_RGBA);

	glm::vec2 guiButtons[] = {
		glm::vec2(50.0f, 50.0f),
	};

	//Shader guiShader = Shader("gui_texture_vertex.glsl", "gui_texture_fragment.glsl");
	//guiShader.addAttrib(
	//	0,
	//	2,
	//	GL_FLOAT,
	//	GL_FALSE,
	//	4 * sizeof(float),
	//	(void*)0);
	//guiShader.addAttrib(
	//	1,
	//	2,
	//	GL_FLOAT,
	//	GL_FALSE,
	//	4 * sizeof(float),
	//	(void*)(2 * sizeof(float)));

	Shader guiShader3D = Shader("gui_texture_vertex.glsl", "world_fragment.glsl");
	guiShader3D.addAttrib(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(float),
		(void*)0);
	guiShader3D.addAttrib(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(float),
		(void*)(3 * sizeof(float)));

	Shader textShader = Shader("gui_text_vertex.glsl", "gui_text_fragment.glsl");
	textShader.loadVBO(
		sizeof(float) * 6 * 4, 
		NULL, 
		GL_DYNAMIC_DRAW);
	textShader.addAttrib(
		0,
		4,
		GL_FLOAT,
		GL_FALSE,
		4 * sizeof(float),
		(void*)0);








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

		glm::vec3 cameraView = player.position;
		cameraView.y += 1.6f;

		glm::vec3 blockOutline, placeBlockPosition;
		chunk.checkInteraction(cameraView, camera.front, 4, &blockOutline, &placeBlockPosition);

		dynamicInputHandler(_window); // Handles keyboard and mouse inputs that aren't reliant on being held down
		player.velocity += player.acceleration * DeltaTime;

		if (breakBlock) {
			breakBlock = false;

			chunk.breakBlock(blockOutline);
		}

		if (placeBlock) {
			placeBlock = false;

			if (placeBlockPosition.y != -16.0f && hotbar[activeHotbarSlot] <= ITEM_BEDROCK)
				chunk.placeBlock(placeBlockPosition, hotbar[activeHotbarSlot]);
		}

		// Check if velocity leads to collision
		chunk.checkCollision(player.position, &player.velocity, player.hitbox, DeltaTime);

		
		//printf("Looking at x: %f, y: %f, z: %f      Position: x: %f, y: %f, z: %f\n", blockOutline.x, blockOutline.y, blockOutline.z, player.position.x, player.position.y, player.position.z);

		// Apply velocity
		player.position += player.velocity * DeltaTime;
		








		// NOTE TO SELF
		// When manipulating matrix go in this order: scale -> transform -> rotate








		/// Graphics ///

		glClearColor(0.24f, 0.26f, 0.27f, 1.0f);				// Set background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Inform openGL to draw the background and reset the z buffer

		
		

		

		// Gameworld //
		shader.bind();
		shader.loadVBO(
			sizeof(vertices_cube),
			vertices_cube,
			GL_STATIC_DRAW);

		glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));
		glActiveTexture(GL_TEXTURE0);

		camera.position = player.position + glm::vec3(0.0f, player.hitbox.y, 0.0f);
		view = camera.generateViewMatrix();
		glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));

		chunk.draw(model_location);


		// Gotta disable the z buffer for a moment as otherwise the prexisting block would prevent this outline from drawing
		glDisable(GL_DEPTH_TEST);

		if (blockOutline.y != -16.0f) {
			glBindTexture(GL_TEXTURE_2D, Chunk::textures[OUTLINE]);
			glm::mat4 blockOutlineModel = glm::translate(glm::mat4(1.0f), blockOutline);
			glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(blockOutlineModel));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glEnable(GL_DEPTH_TEST);

		
		
		// Entities //





		// GUI //
		guiShader3D.bind();

		// Hand
		if (hotbar[activeHotbarSlot] <= ITEM_BEDROCK && hotbar[activeHotbarSlot] >= ITEM_DIRT) {
			guiShader3D.loadVBO(sizeof(vertices_cube), vertices_cube, GL_STATIC_DRAW);
			glBindTexture(GL_TEXTURE_2D, Chunk::textures[hotbar[activeHotbarSlot]]); // Temp
		}
		else {
			guiShader3D.loadVBO(sizeof(vertices_hand), vertices_hand, GL_STATIC_DRAW);
			glBindTexture(GL_TEXTURE_2D, Chunk::textures[GRASS]); // Temp
		}
			

		

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because we want the pixel look
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glm::mat4 handModelPosition = glm::mat4(1.0f);
		


		if (blockBreakTimer == 0.0f) {
			if (hotbar[activeHotbarSlot] > ITEM_NONE && hotbar[activeHotbarSlot] <= ITEM_BEDROCK) {
				handModelPosition = glm::scale(handModelPosition, glm::vec3(0.6f, 0.6f, 0.6f));
				handModelPosition = glm::translate(handModelPosition, glm::vec3(1.7f, -2.1f, 0.0f));
				handModelPosition = glm::rotate(handModelPosition, glm::radians(80.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			}
			else {
				handModelPosition = glm::translate(handModelPosition, glm::vec3(0.55f, -1.4f, 0.0f));
				handModelPosition = glm::rotate(handModelPosition, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			}

			handModelPosition = glm::rotate(handModelPosition, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			handModelPosition = glm::rotate(handModelPosition, glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (blockBreakTimer < 0.05) {
			if (hotbar[activeHotbarSlot] > ITEM_NONE && hotbar[activeHotbarSlot] <= ITEM_BEDROCK) {
				handModelPosition = glm::scale(handModelPosition, glm::vec3(0.6f, 0.6f, 0.6f));
				handModelPosition = glm::translate(handModelPosition, glm::vec3(1.7f, -2.5f, 0.0f));
				handModelPosition = glm::rotate(handModelPosition, glm::radians(80.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			}
			else {
				handModelPosition = glm::translate(handModelPosition, glm::vec3(0.40f, -1.7f, 0.0f));
				handModelPosition = glm::rotate(handModelPosition, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			}

			handModelPosition = glm::rotate(handModelPosition, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			handModelPosition = glm::rotate(handModelPosition, glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else {
			if (hotbar[activeHotbarSlot] > ITEM_NONE && hotbar[activeHotbarSlot] <= ITEM_BEDROCK) {
				handModelPosition = glm::scale(handModelPosition, glm::vec3(0.6f, 0.6f, 0.6f));
				handModelPosition = glm::translate(handModelPosition, glm::vec3(1.75f - (0.25f - blockBreakTimer) * 0.75f, -1.8f + blockBreakTimer * 1.5f, (0.25f - blockBreakTimer) * 1.5f));
				handModelPosition = glm::rotate(handModelPosition, glm::radians(80.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			}
			else {
				handModelPosition = glm::translate(handModelPosition, glm::vec3(0.75f - (0.25f - blockBreakTimer) * 0.75f, -1.8f + blockBreakTimer * 1.5f, (0.25f - blockBreakTimer) * 1.5f));
				handModelPosition = glm::rotate(handModelPosition, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			}

			handModelPosition = glm::rotate(handModelPosition, glm::radians(-25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			handModelPosition = glm::rotate(handModelPosition, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
			
		glUniformMatrix4fv(glGetUniformLocation(guiShader3D.program, "model"), 1, GL_FALSE, glm::value_ptr(handModelPosition));

		if (hotbar[activeHotbarSlot] <= BEDROCK && hotbar[activeHotbarSlot] >= DIRT)
			glDrawArrays(GL_TRIANGLES, 0, 36);
		else
			glDrawArrays(GL_TRIANGLES, 0, 18);
		




		glDisable(GL_DEPTH_TEST);

		// Crosshair
		guiShader3D.loadVBO(sizeof(vertices_crosshair), vertices_crosshair, GL_STATIC_DRAW);
		glBindTexture(GL_TEXTURE_2D, crosshairTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because we want the pixel look
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glm::mat4 crosshairModelPosition = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(guiShader3D.program, "model"), 1, GL_FALSE, glm::value_ptr(crosshairModelPosition));
		glDrawArrays(GL_TRIANGLES, 0, 6);



		// Hotbar
		guiShader3D.loadVBO(sizeof(vertices_hotbar), vertices_hotbar, GL_STATIC_DRAW);
		glBindTexture(GL_TEXTURE_2D, hotbarTexture); // Temp

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because we want the pixel look
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


		glm::mat4 hotbarModelPosition = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(guiShader3D.program, "model"), 1, GL_FALSE, glm::value_ptr(hotbarModelPosition));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// Item in hotbar
		glm::mat4 hotbarSlotModelPosition = glm::mat4(1.0f);
		guiShader3D.loadVBO(sizeof(vertices_hotbarSlot), vertices_hotbarSlot, GL_STATIC_DRAW);
		for (int slot = 0; slot < 9; slot++) {
			if (hotbar[slot] != ITEM_NONE) {
				// This will need to change but for now since the only items are blocks
				// im just going to rely on preload block sprites stored in Chunk
				glBindTexture(GL_TEXTURE_2D, Chunk::textures[hotbar[slot]]);
				hotbarSlotModelPosition = glm::translate(glm::mat4(1.0f), glm::vec3(-0.4f + 0.1f * slot, 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(guiShader3D.program, "model"), 1, GL_FALSE, glm::value_ptr(hotbarSlotModelPosition));
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

			if (slot == activeHotbarSlot) {
				guiShader3D.loadVBO(sizeof(vertices_activeHotbarSlot), vertices_activeHotbarSlot, GL_STATIC_DRAW);
				glBindTexture(GL_TEXTURE_2D, hotbarActivateTex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because we want the pixel look
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				hotbarSlotModelPosition = glm::translate(glm::mat4(1.0f), glm::vec3(-0.40f + 0.1f * (float)slot, 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(guiShader3D.program, "model"), 1, GL_FALSE, glm::value_ptr(hotbarSlotModelPosition));
				glDrawArrays(GL_TRIANGLES, 0, 6);

				guiShader3D.loadVBO(sizeof(vertices_hotbarSlot), vertices_hotbarSlot, GL_STATIC_DRAW);
			}
		}




		// Main in-game menu (accessed via ESC)
		if (menu_activate) {
			// Button
			guiShader3D.loadVBO(sizeof(vertices_button), vertices_button, GL_STATIC_DRAW);
			glBindTexture(GL_TEXTURE_2D, menuButtonHighlighted ? tempGuiHighlightButton : tempGuiButton);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because we want the pixel look
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glm::mat4 buttonModelPosition = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(guiShader3D.program, "model"), 1, GL_FALSE, glm::value_ptr(buttonModelPosition));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			// text
			textShader.bind();
			glUniformMatrix4fv(glGetUniformLocation(textShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection_gui));
			renderText(textShader, "reset", 340.0f, 218.0f, 0.75f, glm::vec3(255.0f, 255.0f, 255.0f));
		}	
		glEnable(GL_DEPTH_TEST);


		blockBreakTimer = blockBreakTimer > 0.0f ? blockBreakTimer - DeltaTime * 2 : 0.0f;

		glfwSwapBuffers(_window); // Swaps buffers, drawing new content to the screen
	}

	glfwTerminate();
}




void dynamicInputHandler(GLFWwindow* window) {
	// Exit application
	if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS)
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

GLuint loadTextureFile(const char* filepath, GLuint type) {
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
	glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unload image data from memory
	stbi_image_free(data);

	return textureID;
}

void renderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color) {
	s.bind();
	glUniform3f(glGetUniformLocation(s.program, "textColor"), color.r, color.g, color.b);
	glActiveTexture(GL_TEXTURE0);

	// Interates through all the characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		// Adjust positioning based on character information
		float xpos = x + ch.bearing.x * scale;
		float ypos = (DefaultWindowHeight - y) - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;

		// Vertice + texture coord data to be loaded into the VBO
		float vertices[6][4] = {
			{xpos,		ypos + h,	0.0f, 0.0f},
			{xpos,		ypos,		0.0f, 1.0f},
			{xpos + w,	ypos,		1.0f, 1.0f},

			{xpos,		ypos + h,	0.0f, 0.0f},
			{xpos + w,	ypos,		1.0f, 1.0f},
			{xpos + w,	ypos + h,	1.0f, 0.0f},
		};

		// Bind texture
		glBindTexture(GL_TEXTURE_2D, ch.id);

		// Load VBO
		glBindBuffer(GL_ARRAY_BUFFER, s.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.advance >> 6) * scale;
	}
}


// Callbacks //

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	camera.updateFacingDirection(xpos, ypos);
	
	if (menu_activate&&
		xpos > 150 && xpos < 350 &&
		ypos > 225 && ypos < 275
		) {
		menuButtonHighlighted = true;
	}
	else {
		menuButtonHighlighted = false;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS) {
			if (menu_activate && menuButtonHighlighted) {
				player.position = glm::vec3(0.0f, 3.0f, 0.0f);
			}
			else {
				breakBlock = true;
				blockBreakTimer = 0.5f;
			}
		}
			
		break;

	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS) {
				placeBlock = true;
				blockBreakTimer = 0.5f;
		}
		break;
	}
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
				menu_activate = !menu_activate;
				camera.mouseEnabled = !menu_activate;

				if (menu_activate)
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				else
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


				break;


			case GLFW_KEY_1:
				activeHotbarSlot = 0;
				break;
			case GLFW_KEY_2:
				activeHotbarSlot = 1;
				break;
			case GLFW_KEY_3:
				activeHotbarSlot = 2;
				break;
			case GLFW_KEY_4:
				activeHotbarSlot = 3;
				break;
			case GLFW_KEY_5:
				activeHotbarSlot = 4;
				break;
			case GLFW_KEY_6:
				activeHotbarSlot = 5;
				break;
			case GLFW_KEY_7:
				activeHotbarSlot = 6;
				break;
			case GLFW_KEY_8:
				activeHotbarSlot = 7;
				break;
			case GLFW_KEY_9:
				activeHotbarSlot = 8;
				break;
		}
	}

}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	

	if (yoffset > 0.0f) // Up
		activeHotbarSlot = activeHotbarSlot <= 0 ? 8 : activeHotbarSlot - 1;
		

	if (yoffset < 0.0f) // Down
		activeHotbarSlot = (activeHotbarSlot >= 8) ? 0 : activeHotbarSlot + 1;
}