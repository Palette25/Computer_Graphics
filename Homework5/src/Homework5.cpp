// Include standard headers
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;

// Include GLEW
#include <GL/glew.h>
#pragma comment(lib, "glew32.lib")

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <common/imgui.h>
#include <common/imgui_impl_opengl3.h>
#include <common/imgui_impl_glfw.h>
#include <common/shader.hpp>
#include <common/Camera.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 1.8f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.1f;	// time between current frame and last frame
float lastFrame = 0.0f;

void processInput(GLFWwindow *window) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// WSAD Moving
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(W, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(S, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(A, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(D, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void none_callback(GLFWwindow* window, double xoffset, double yoffset) {}

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1280, 720, "CG_HW4", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	//Set up IMGUI binding
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	// Iniy GLFW and OpenGL ImGui Window
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init(glsl_version);
	
	//Viewport
	glViewport(0, 0, 1280, 720);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Vertex Array Object Define
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	// OpenGL Drawing Vertex array
	GLfloat vertices[] = {
		-0.4f, -0.4f, -0.4f, 
		0.4f, -0.4f, -0.4f, 
		0.4f, 0.4f, -0.4f, 
		0.4f, 0.4f, -0.4f, 
		-0.4f, 0.4f, -0.4f, 
		-0.4f, -0.4f, -0.4f, 

		-0.4f, -0.4f, 0.4f, 
		0.4f, -0.4f, 0.4f, 
		0.4f, 0.4f, 0.4f, 
		0.4f, 0.4f, 0.4f, 
		-0.4f, 0.4f, 0.4f, 
		-0.4f, -0.4f, 0.4f, 

		-0.4f, 0.4f, 0.4f, 
		-0.4f, 0.4f, -0.4f, 
		-0.4f, -0.4f, -0.4f, 
		-0.4f, -0.4f, -0.4f, 
		-0.4f, -0.4f, 0.4f, 
		-0.4f, 0.4f, 0.4f, 

		0.4f, 0.4f, 0.4f, 
		0.4f, 0.4f, -0.4f, 
		0.4f, -0.4f, -0.4f, 
		0.4f, -0.4f, -0.4f, 
		0.4f, -0.4f, 0.4f, 
		0.4f, 0.4f, 0.4f, 

		-0.4f, -0.4f, -0.4f, 
		0.4f, -0.4f, -0.4f, 
		0.4f, -0.4f, 0.4f, 
		0.4f, -0.4f, 0.4f, 
		-0.4f, -0.4f, 0.4f, 
		-0.4f, -0.4f, -0.4f, 

		-0.4f, 0.4f, -0.4f, 
		0.4f, 0.4f, -0.4f, 
		0.4f, 0.4f, 0.4f, 
		0.4f, 0.4f, 0.4f, 
		-0.4f, 0.4f, 0.4f, 
		-0.4f, 0.4f, -0.4f
	};

	GLfloat colors[] = {
		192, 192, 192,
		192, 192, 192,
		192, 192, 192,
		192, 192, 192,
		192, 192, 192,
		192, 192, 192,

		135, 206, 235,
		135, 206, 235,
		135, 206, 235,
		135, 206, 235,
		135, 206, 235,
		135, 206, 235,

		127, 255, 0,
		127, 255, 0,
		127, 255, 0,
		127, 255, 0,
		127, 255, 0,
		127, 255, 0,

		255, 235, 205,
		255, 235, 205,
		255, 235, 205,
		255, 235, 205,
		255, 235, 205,
		255, 235, 205,

		218, 112, 214,
		218, 112, 214,
		218, 112, 214,
		218, 112, 214,
		218, 112, 214,
		218, 112, 214,

		255, 128, 0,
		255, 128, 0,
		255, 128, 0,
		255, 128, 0,
		255, 128, 0,
		255, 128, 0
	};

	// Vertex Buffer Object and Element Buffer Object
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint VCO;
	glGenBuffers(1, &VCO);
	glBindBuffer(GL_ARRAY_BUFFER, VCO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	glEnable(GL_DEPTH_TEST);

	// Position Attributes binding
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	// Color Attributes binding
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, VCO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Define mode bool variables
	float left = 0.0f, right = 5.0f, bottom = 0.0f, top = 5.0f, near = 0.1f, far = 17.0f, radius = 45.0f;
	float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT, zNear = 0.1f, zFar = 10.0f;
	bool ortho = false, perspective = false, view_change = false;
	bool bonus = false;

	do {
		glfwPollEvents();
		// New OpenGL3 frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);

		if(bonus){
			processInput(window);
			glfwSetCursorPosCallback(window, mouse_callback);
			glfwSetScrollCallback(window, scroll_callback);
		}
		else {
			glfwSetCursorPosCallback(window, none_callback);
			glfwSetScrollCallback(window, none_callback);
		}

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(-1.5, 0.5, -1.5));
		glm::mat4 view;
		glm::mat4 projection;
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		// Define ImGUI window elements
		{
			ImGui::Begin("Camera Editer");
			ImGui::Checkbox("Show Orthographic Projection", &ortho);
			if (ortho) {
				ImGui::Begin("Parameters Editer");
				ImGui::SliderFloat("Left", &left, -5.0f, 5.0f);
				ImGui::SliderFloat("Right", &right, 0.0f, 10.0f);
				ImGui::SliderFloat("Bottom", &bottom, -5.0f, 5.0f);
				ImGui::SliderFloat("Top", &top, 0.0f, 10.0f);
				ImGui::SliderFloat("Near", &near, 10.0f, 30.0f);
				ImGui::SliderFloat("Far", &far, 10.0f, 30.0f);
				ImGui::End();
			}
			ImGui::Checkbox("Show Perspective Projection", &perspective);
			if (perspective) {
				ImGui::Begin("Paramerers Editer");
				ImGui::SliderFloat("Radius", &radius, 0.0f, 90.0f);
				ImGui::SliderFloat("Aspect", &aspect, 0.0f, 10.0f);
				ImGui::SliderFloat("zNear", &zNear, 0.0f, 10.0f);
				ImGui::SliderFloat("zFar", &zFar, 0.0f, 20.0f);
				ImGui::End();
			}
			ImGui::Checkbox("View Changing", &view_change);
			ImGui::Checkbox("Show Bonus", &bonus);
			ImGui::End();
		}

		if (ortho) {
			model = glm::rotate(model, 15.0f, glm::vec3(1.0f, 1.0f, 1.0f));

			view = glm::translate(view, glm::vec3(3.0f, 3.5f, -7.0f));
			projection = glm::ortho(left, right, bottom, top, near, far);
		}
		if (perspective) {
			projection = glm::perspective(glm::radians(radius), aspect, zNear, zFar);
		}
		if (view_change) {
			float radius = 10.0f;
			float camX = sin(glfwGetTime())*radius;
			float camZ = cos(glfwGetTime())*radius;

			
			view = glm::lookAt(glm::vec3(camX, 2.0f, camZ), glm::vec3(-1.5, 0.5, -1.5), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		unsigned int viewLoc = glGetUniformLocation(programID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		unsigned int projectionLoc = glGetUniformLocation(programID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		unsigned int modelLoc = glGetUniformLocation(programID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// Render ImGUI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VAO);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}