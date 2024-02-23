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

static unsigned int CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*) malloc(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glBindAttribLocation(program, 0, "aPos");
	glBindAttribLocation(program, 1, "aColor");

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

class Vector3 {
public:
	float x, y, z;
	Vector3() {}
	Vector3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

struct Cube {
	float width, height, depth;
	Vector3 colorFront, colorBack, colorLeftSide, colorRightSide, colorTop, colorBottom;
	void draw();
};

void Cube::draw() {

}

bool openMouth = true;
float i = 1;

void drawCube(float width, float height, float depth) {
	glBegin(GL_QUADS); {
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(width, -height, -depth);
		glVertex3f(-width, -height, -depth);
		glVertex3f(-width, height, -depth);
		glVertex3f(width, height, -depth);

		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-width, -height, depth);
		glVertex3f(width, -height, depth);
		glVertex3f(width, height, depth);
		glVertex3f(-width, height, depth);

		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(width, -height, depth);
		glVertex3f(width, -height, -depth);
		glVertex3f(width, height, -depth);
		glVertex3f(width, height, depth);

		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(-width, -height, -depth);
		glVertex3f(-width, -height, depth);
		glVertex3f(-width, height, depth);
		glVertex3f(-width, height, -depth);

		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-width, -height, -depth);
		glVertex3f(width, -height, -depth);
		glVertex3f(width, -height, depth);
		glVertex3f(-width, -height, depth);

		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(width, height, -depth);
		glVertex3f(-width, height, -depth);
		glVertex3f(-width, height, depth);
		glVertex3f(width, height, depth);

	} glEnd();
}

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

/*void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}*/

void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

int main(void) {
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	// Initialize the library
	if (!glfwInit())
		return -1;

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Create a windowed mode and its OpenGL context
	window = glfwCreateWindow(640, 480, "Hello World!", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	//glfwMakeContextCurrent(window);

	// Make the windows context current
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (glewInit() != GLEW_OK) {
		std::cout << "Error!" << std::endl;
	}


	float positions[6] = {
		-0.5f, -0.5f,
		0.0f, 0.5f,
		0.5f, -0.5f
	};


	std::ifstream s1("shaders/shader.fs");
	std::ifstream s2("shaders/shader.vs");
	
	std::stringstream ss1;
	std::stringstream ss2;
	ss1 << s1.rdbuf();
	ss2 << s2.rdbuf();
	std::string fs = ss1.str();
	std::string vs = ss2.str();
	s1.close();
	s2.close();

	s1.clear();
	s2.clear();

	GLfloat vertices[] = {
		// Positions	 // Colors
		-0.05f,  0.05f,	 1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,	 0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,	 0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,	 0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};

	/*unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, 5 * 6 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	*/
	unsigned int shader = CreateShader(vs, fs);

	/*glm::vec2 translations[100];
	int index = 0;
	float offset = 0.1f;

	for (int y = -10; y < 10; y += 2) {
		for (int x = -10; x < 10; x += 2) {
			glm::vec2 translation;
			translation.x = (float)x / 10.0f + offset;
			translation.y = (float)y / 10.0f + offset;
			translations[index++] = translation;
		}
	}

	//glUseProgram(shader);
	for (unsigned int i = 0; i < 100; i++) {
		std::stringstream ss;
		std::string index;
		ss << i;
		index = ss.str();
		GLint loc = glGetUniformLocation(shader, ("offsets[" + index + "]").c_str());
		glUniform2fv(loc, 1, glm::value_ptr(translations[i]));
	}*/
	
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);

	glm::mat4 View = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 Model = glm::mat4(1.0f);

	glm::mat4 MVP = Projection * View * Model;


	//ourShader.setFloat("someUniform", 1.0f);
	//glBindVertexArray(vao);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glRotatef(2.0f, 3, 0, 0);

	Shader myShader("assets/shaders/shader.vs", "assets/shaders/shader.fs");

	//Chunk chunk;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, -20.0f, 0.0f));

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -50.0f));
	
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);

	FrustumG* frustum = new FrustumG();

	ChunkManager* manager = new ChunkManager(frustum);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		glm::vec3 cameraDir = cameraPos + cameraFront;
		//trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//glUniformMatrix4fv(unitrans, 1, GL_FALSE, glm::value_ptr(trans));
		//float ratio = 640 / 480.0f;

		// Render here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glPushMatrix();
		//ourShader.use()
		//glTranslatef(0.5f, 0.5f, 1);
		myShader.use();
		view = glm::lookAt(cameraPos, cameraDir, cameraUp);
		frustum->setCamDef(cameraPos, cameraDir, cameraUp);

		manager->Update(0, cameraPos, cameraDir);

		/*model = glm::rotate(model, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		int modelLoc = glGetUniformLocation(myShader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		*/
		int viewLoc = glGetUniformLocation(myShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		int projectionLoc = glGetUniformLocation(myShader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		manager->Render(myShader);

		/*chunk.CreateMesh();
		chunk.Render(myShader);*/



		//glPushMatrix();
		
		
		//glScalef(0.5f, 0.5f, 0.5f);
		/*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (int x = 0; x < 32; x++) {
			for (int y = 0; y < 32; y++) {
				for (int z = 0; z < 32; z++) {
					glTranslatef(x, y, z);
					drawCube(1, 1, 1);
					glTranslatef(-x, -y, -z);
				}
			}
		}*/
		//glPopMatrix();
		
		///glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
		
		//GLuint MatrixID = glGetUniformLocation(shader, "MVP");
		//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glUseProgram(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	//glDeleteProgram(shader);
	///glDeleteBuffers(1, &vbo);
	///glDeleteVertexArrays(1, &vao);
	s1.clear();
	s2.clear();

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}