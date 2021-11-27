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

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();

// Window dimensions
const GLuint WIDTH = 640, HEIGHT = 640;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
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

	//++++++++++Set up vertex data (and buffer(s)) and attribute pointers+++++++++
	GLfloat vertices1[] = {
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f
	};
	// Read our .obj file
	objl::Loader loader;
	loader.LoadFile("boxstack.obj");

	std::vector<GLfloat> vertices;
	for (int i = 0; i < loader.LoadedVertices.size(); i++) {
		vertices.push_back(loader.LoadedVertices[i].Position.X);
		vertices.push_back(loader.LoadedVertices[i].Position.Y);
		vertices.push_back(loader.LoadedVertices[i].Position.Z);
		vertices.push_back(0.7f);
		vertices.push_back(1.0f);
		vertices.push_back(0.1f);
	}

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    // ================================
    // buffer setup
    // ===============================
    glBindVertexArray(VAO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);	// Vertex attributes stay the same
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Color attribute
	glEnableVertexAttribArray(1);
    glBindVertexArray(0);

	//++++++++++Build and compile shader program+++++++++++++++++++++
	GLuint shaderProgram = initShader("vert.glsl","frag.glsl");

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
		
		// Draw the cube
		// use shader
		glUseProgram(shaderProgram);

		// Create transformations
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 transform;
		//model = glm::rotate(model, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		
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

		// draw object
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3*12);
		// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
		//glDrawElements(GL_TRIANGLES, vertices.size() * sizeof(glm::vec3), GL_UNSIGNED_SHORT, (void*)0);
	
		glBindVertexArray(0);

		// create second cube
		glm::mat4 model2;
		model2 = glm::rotate(model2, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		model2 = glm::translate(transform, glm::vec3(1.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);

		glBindVertexArray(0);

		// create third cube
		glm::mat4 model3;
		model3 = glm::translate(transform, glm::vec3(1.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);

		glBindVertexArray(0);

		// create forth cube
		glm::mat4 model4;
		model4 = glm::translate(transform, glm::vec3(2.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);

		glBindVertexArray(0);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

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