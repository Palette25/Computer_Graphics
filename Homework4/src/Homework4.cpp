// Include standard headers
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "common/stb_image.h"

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

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
		-0.4f, -0.4f, -0.4f, 0.0f, 0.0f,
		0.4f, -0.4f, -0.4f, 1.0f, 0.0f,
		0.4f, 0.4f, -0.4f, 1.0f, 1.0f,
		0.4f, 0.4f, -0.4f, 1.0f, 1.0f,
		-0.4f, 0.4f, -0.4f, 0.0f, 1.0f,
		-0.4f, -0.4f, -0.4f, 0.0f, 0.0f,

		-0.4f, -0.4f, 0.4f, 0.0f, 0.0f,
		0.4f, -0.4f, 0.4f, 1.0f, 0.0f,
		0.4f, 0.4f, 0.4f, 1.0f, 1.0f,
		0.4f, 0.4f, 0.4f, 1.0f, 1.0f,
		-0.4f, 0.4f, 0.4f, 0.0f, 1.0f,
		-0.4f, -0.4f, 0.4f, 0.0f, 0.0f,

		-0.4f, 0.4f, 0.4f, 1.0f, 0.0f,
		-0.4f, 0.4f, -0.4f, 1.0f, 1.0f,
		-0.4f, -0.4f, -0.4f, 0.0f, 1.0f,
		-0.4f, -0.4f, -0.4f, 0.0f, 1.0f,
		-0.4f, -0.4f, 0.4f, 0.0f, 0.0f,
		-0.4f, 0.4f, 0.4f, 1.0f, 0.0f,

		0.4f, 0.4f, 0.4f, 1.0f, 0.0f,
		0.4f, 0.4f, -0.4f, 1.0f, 1.0f,
		0.4f, -0.4f, -0.4f, 0.0f, 1.0f,
		0.4f, -0.4f, -0.4f, 0.0f, 1.0f,
		0.4f, -0.4f, 0.4f, 0.0f, 0.0f,
		0.4f, 0.4f, 0.4f, 1.0f, 0.0f,

		-0.4f, -0.4f, -0.4f, 0.0f, 1.0f,
		0.4f, -0.4f, -0.4f, 1.0f, 1.0f,
		0.4f, -0.4f, 0.4f, 1.0f, 0.0f,
		0.4f, -0.4f, 0.4f, 1.0f, 0.0f,
		-0.4f, -0.4f, 0.4f, 0.0f, 0.0f,
		-0.4f, -0.4f, -0.4f, 0.0f, 1.0f,

		-0.4f, 0.4f, -0.4f, 0.0f, 1.0f,
		0.4f, 0.4f, -0.4f, 1.0f, 1.0f,
		0.4f, 0.4f, 0.4f, 1.0f, 0.0f,
		0.4f, 0.4f, 0.4f, 1.0f, 0.0f,
		-0.4f, 0.4f, 0.4f, 0.0f, 0.0f,
		-0.4f, 0.4f, -0.4f, 0.0f, 1.0f
	};

	// Vertex Buffer Object and Element Buffer Object
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position Attributes binding
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Color Attributes binding
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data = stbi_load("blue.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    glUseProgram(programID);
    glUniform1i(glGetUniformLocation(programID, "texture1"), 0);

	glEnable(GL_DEPTH_TEST);

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
	glm::mat4 originModel = model;
	model = glm::mat4(1.0f);
	view = glm::lookAt(
		glm::vec3(2, 2, 2),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

	// Define mode bool variables
	bool translation = false, rotation = false, scale = false, enable = false, rotate_by_time = false;
	bool bonus = false;
	float x = 0.0f, y = 0.0f;
	float radius = 0.0f;
	float scale_rate = 1.0f;

	do {
		glfwPollEvents();
		// New OpenGL3 frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glUseProgram(programID);

		// Define ImGUI window elements
		{
			ImGui::Begin("Transformation Editer");
			ImGui::Checkbox("Enable Depth Test", &enable);
			ImGui::Checkbox("Cube Translation", &translation);
			ImGui::SliderFloat("Translation - X", &x, -1.0f, 1.0f);
			ImGui::SliderFloat("Translation - Y", &y, -1.0f, 1.0f);
			ImGui::Text("\n");
			ImGui::Checkbox("Cube Rotation", &rotation);
			ImGui::SliderFloat("Rotation Radius", &radius, 0.0f, 360.0f);
			ImGui::Checkbox("Rotate By Time ~", &rotate_by_time);
			ImGui::Text("\n");
			ImGui::Checkbox("Cube Scaling", &scale);
			ImGui::SliderFloat("Scale Rate", &scale_rate, 0.0f, 2.0f);
			ImGui::Text("\n");
			ImGui::Checkbox("Show Bonus", &bonus);
			ImGui::End();
		}

		unsigned int viewLoc = glGetUniformLocation(programID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		unsigned int projectionLoc = glGetUniformLocation(programID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		// Render Cube
		if (enable) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}

		if (bonus) {
			float currTime = glfwGetTime();
			for (int i = 0; i < 3; i++) {
				if (i == 0) {
					model = glm::scale(originModel, glm::vec3(0.4f, 0.4f, 0.4f));
					model = glm::rotate(model, currTime * 0.5f, glm::vec3(0.0f, 1.0f, 1.0f));
				}
				else if (i == 1) {
					model = glm::scale(originModel, glm::vec3(0.2f, 0.2f, 0.2f));
					model = glm::rotate(model, currTime, glm::vec3(0.0f, 1.0f, 1.0f));
					model = glm::translate(model, glm::vec3(cos(currTime * 1.5f) + 5.0f, sin(currTime * 1.5f) + 0.0f, 0.0f));
				}
				else if (i == 2) {
					model = glm::translate(model, glm::vec3(cos(currTime * 2), sin(currTime * 2), 0.0f));
					model = glm::rotate(model, currTime * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
				}
				unsigned int modelLoc = glGetUniformLocation(programID, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
		else {
			if (translation) {
				model = glm::translate(originModel, glm::vec3(x, y, 0.0f));
			}
			else if (rotation) {
				if (rotate_by_time) {
					model = glm::rotate(originModel, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 1.0f));
				}
				else {
					model = glm::rotate(originModel, radius, glm::vec3(0.0f, 1.0f, 1.0f));
				}
			}
			else if (scale) {
				model = glm::scale(originModel, glm::vec3(scale_rate, scale_rate, scale_rate));
			}

			unsigned int modelLoc = glGetUniformLocation(programID, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
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