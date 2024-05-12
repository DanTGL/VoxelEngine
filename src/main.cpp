#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include "mesh.h"
#include "chunk.h"
#include "chunkmanager.h"

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	float cameraSpeed = 10;

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(0);
	}
}

float lastX = 400, lastY = 300;

bool firstMouse = true;

float pitch = 0.0f, yaw = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

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

void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

int main(void) {
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	// Initialize the library
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	// Create a windowed mode and its OpenGL context
	window = glfwCreateWindow(640, 480, "Hello World!", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	// Make the windows context current
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (glewInit() != GLEW_OK) {
		std::cout << "Error!" << std::endl;
	}


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	Shader myShader("assets/shaders/shader.vs", "assets/shaders/shader.fs");

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, -20.0f, 0.0f));

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -50.0f));
	
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);

	FrustumG* frustum = new FrustumG();

	ChunkManager* manager = new ChunkManager(frustum);

	double fps, t;
	double t0 = glfwGetTime();
	int frames = 0;


	char title_string[15];

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		glm::vec3 cameraDir = cameraPos + cameraFront;

		// Render here
		glClearColor(0.0f, 0.3f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myShader.use();
		view = glm::lookAt(cameraPos, cameraDir, cameraUp);
		frustum->setCamDef(cameraPos, cameraDir, cameraUp);

		manager->Update(0, cameraPos, cameraDir);

		int viewLoc = glGetUniformLocation(myShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		int projectionLoc = glGetUniformLocation(myShader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		manager->Render(myShader);
		
		t = glfwGetTime();

		if ((t - t0) > 1.0 || frames == 0) {
			fps = (double) frames / (t - t0);
			sprintf_s(title_string, "FPS: %.1f", fps);
			glfwSetWindowTitle(window, title_string);
			t0 = t;
			frames = 0;
		}

		frames++;

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glUseProgram(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}