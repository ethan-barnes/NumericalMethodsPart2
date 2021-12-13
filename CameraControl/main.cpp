// Code adapted from www.learnopengl.com, www.glfw.org

#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtc/constants.hpp>

#include "shader.h"
#include "OBJ-Loader.h"
#include "texture.hpp"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();

// Window dimensions
const GLuint WIDTH = 640, HEIGHT = 640;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Light attributes
glm::vec3 lightPos(15.0f, 15.0f, 15.0f);

GLfloat floorVector[] = {
	// Position data,                    normals,          texture coords
	-0.131034f, -0.003671f, -1.811131f, 0.0f, -0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	6.722751f, -0.003671f, -1.811131f, 0.0f, -0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	-0.131034f, -0.003671f, -8.664917f, 0.0f, -0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	6.722751f, -0.003671f, -8.664917f, 0.0f, -0.0f, 1.0f, 1.0f, 1.0f, 0.0f
};

GLint floorIndices[] = {
	0, 1, 2,
	1, 1, 2,
	3, 1, 2,
	2, 1, 2,
};

std::vector<GLfloat> loadVertices(objl::Loader loader) {
	std::vector<GLfloat> vertices;

	for (int y = 0; y < loader.LoadedMeshes.size(); y++) {

		for (int i = 0; i < loader.LoadedVertices.size(); i++) {
			vertices.push_back(loader.LoadedVertices[i].Position.X);
			vertices.push_back(loader.LoadedVertices[i].Position.Y);
			vertices.push_back(loader.LoadedVertices[i].Position.Z);

			vertices.push_back(loader.LoadedVertices[i].Normal.X);
			vertices.push_back(loader.LoadedVertices[i].Normal.Y);
			vertices.push_back(loader.LoadedVertices[i].Normal.Z);

			vertices.push_back(loader.LoadedVertices[i].TextureCoordinate.X);
			vertices.push_back(1 - (loader.LoadedVertices[i].TextureCoordinate.Y));
			vertices.push_back(0);
		}
	}

	return vertices;
}

std::vector<GLuint> loadIndices(objl::Loader loader) {
	std::vector<GLuint> indices;

	for (int y = 0; y < loader.LoadedMeshes.size(); y++) {
		for (int j = 0; j < loader.LoadedIndices.size(); j++) {
			indices.push_back(loader.LoadedIndices[j]);
		}
	}

	return indices;
}



int main(void)
{
	//++++create a glfw window+++++++++++++++++++++++++++++++++++++++
	GLFWwindow* window;

	if (!glfwInit()) //Initialize the library
		return -1;

	window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);//Make the window's context current
								   
	glfwSetKeyCallback(window, key_callback);// Set the required callback functions

	glfwSetCursorPosCallback(window, mouse_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	//++++Initialize GLEW to setup the OpenGL Function pointers+++++++
	glewExperimental = GL_TRUE;
	glewInit();

	//++++Define the viewport dimensions++++++++++++++++++++++++++++
	glViewport(0, 0, HEIGHT, HEIGHT);

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Load textures
	GLuint WatchTexture = loadDDS("textures/watchtower.dds", 0);
	GLuint FirTexture = loadDDS("textures/fir.dds", 1);
	GLuint FloorTexture = loadDDS("textures/floor1.dds", 2);
	GLuint RavenTexture = loadDDS("textures/raven.dds", 3);

	// Read the .obj file
	std::string watchTowerLoc = "objects/watchtower.obj";
	objl::Loader loader;
	if (!loader.LoadFile(watchTowerLoc)) {
		std::cout << "Obj file not found";
		glfwTerminate();
		return -1;
	}

	std::vector<GLfloat> vertices = loadVertices(loader);
	std::vector<GLuint> indices = loadIndices(loader);

	// Texture coords
	glm::vec2 UV;

	GLuint VBOs[5], VAOs[5], EBOs[5];
	glGenVertexArrays(5, VAOs);
	glGenBuffers(5, VBOs);
	glGenBuffers(5, EBOs);

    // ================================
    // buffer setup shape 1 watchtower
    // ===============================

    glBindVertexArray(VAOs[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	
	// Vertex attributes stay the same
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);	
    glEnableVertexAttribArray(0);

	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

	// Texture coords attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	// ================================
	// buffer setup shape 2 tree
	// ===============================

	std::string treeLoc = "objects/fir.obj";
	objl::Loader loader1;
	if (!loader1.LoadFile(treeLoc)) {
		std::cout << "Obj file not found";
		glfwTerminate();
		return -1;
	}

	std::vector<GLfloat> vertices1 = loadVertices(loader1);
	std::vector<GLuint> indices1 = loadIndices(loader1);

	glBindVertexArray(VAOs[1]);
	glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(GLfloat), &vertices1[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(GLfloat), &vertices1[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices1.size() * sizeof(GLuint), &indices1[0], GL_STATIC_DRAW);

	// Vertex attributes stay the same
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Texture coords attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ================================
	// buffer setup shape 3 ground
	// ===============================

	glBindVertexArray(VAOs[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVector), floorVector, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVector), floorVector, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

	// Vertex attributes stay the same
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Texture coords attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ================================
	// buffer setup shape 4 raven
	// ===============================
	std::string ravenLoc = "objects/raven.obj";
	objl::Loader loader2;
	if (!loader2.LoadFile(ravenLoc)) {
		std::cout << "Obj file not found";
		glfwTerminate();
		return -1;
	}

	std::vector<GLfloat> vertices2 = loadVertices(loader2);
	std::vector<GLuint> indices2 = loadIndices(loader2);

	glBindVertexArray(VAOs[3]);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(GLfloat), &vertices2[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(GLfloat), &vertices2[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(GLuint), &indices2[0], GL_STATIC_DRAW);

	// Vertex attributes stay the same
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Texture cooards attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//++++++++++Build and compile shader program+++++++++++++++++++++
	GLuint shaderProgram = initShader("vert.glsl","frag.glsl");

	// use shader
	glUseProgram(shaderProgram);
	GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

	//++++++++++++++++++++++++++++++++++++++++++++++
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = (GLfloat) glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		do_movement();

		/* Render here */
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//glm::vec3 lightPos(cameraPos.x, cameraPos.y, cameraPos.z);

		// ==================
		// draw first object
		// ==================

		glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), WatchTexture - 1);
		// Create transformations
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 transform;
		//model = glm::rotate(model, (GLfloat)glfwGetTime() * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
		
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// Get their uniform location
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
		GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");

		// Pass them to the shaders
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		// draw object
		glBindVertexArray(VAOs[0]);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		// ==================
		// draw second object
		// ==================
		
		glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), FirTexture - 1);
		//model = glm::rotate(model, (GLfloat)glfwGetTime() * -1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, -7.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAOs[1]);
		glDrawElements(GL_TRIANGLES, indices1.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		// ==================
		// draw third object
		// ==================

		glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), FloorTexture - 1);
		model = glm::translate(model, glm::vec3(-10.0f, 0.46f, 40.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAOs[2]);
		glDrawElements(GL_TRIANGLES, sizeof(floorIndices), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		// ==================
		// draw fourth object
		// ==================
		glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), RavenTexture - 1);

		model = glm::translate(model, glm::vec3(0.85f, 0.35f, -3.55f));
		model = glm::rotate(model, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f)); 
		model = glm::translate(model, glm::vec3(0.15f, 0.0f, -0.45f));
		model = glm::rotate(model, (GLfloat)(3.14 / 4), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (GLfloat)(3.14 / 2), glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAOs[3]);
		glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, VAOs);
	glDeleteBuffers(1, VBOs);

	glfwTerminate();
	return 0;
}

 //Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}



void do_movement()
{
	// Camera controls
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (GLfloat) xpos;
		lastY = (GLfloat) ypos;
		firstMouse = false;
	}

	GLfloat xoffset = (GLfloat) xpos - lastX;
	GLfloat yoffset = lastY - (GLfloat) ypos; // Reversed since y-coordinates go from bottom to left
	lastX = (GLfloat) xpos;
	lastY = (GLfloat) ypos;

	GLfloat sensitivity = (GLfloat) 0.05;	// Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}