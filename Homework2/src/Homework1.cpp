// Include standard headers
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
using namespace glm;

#include <common/imgui.h>
#include <common/imgui_impl_opengl3.h>
#include <common/imgui_impl_glfw.h>
#include <common/shader.hpp>

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1280, 720, "CG_HW2", NULL, NULL);
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
	
	// Define Color selection storage
	bool drawAll = false, showBonus = true;
	vector<ImVec4> colors;
	ImVec4 color_all = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
	colors.push_back(ImVec4(0.0f, 0.0f, 1.0f, 1.00f));
	colors.push_back(ImVec4(0.0f, 1.0f, 0.0f, 1.00f));
	colors.push_back(ImVec4(1.0f, 0.0f, 0.0f, 1.00f));
	
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
		// Left Part (Three float numbers are locations -- (x, y, z))
		// Right Part (Three float numbers are colors -- (r, g, b))
		// First Triangle
		0.8f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
		0.0f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		0.4f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
		// Second Triangle
		-0.25f, -0.65f, 0.0f, 1.0f, 1.0f, 0.0f,
		-0.75f, -0.65f, 0.0f,  1.0f, 1.0f, 0.0f,
		-0.5f, -0.2f, 0.0f,  1.0f, 1.0f, 0.0f,
		// Lines
		-0.2f, -0.8f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.8f, -0.8f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.2f, -0.1f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.8f, -0.1f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.8f, -0.8f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.8f, -0.1f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.2f, -0.1f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.2f, -0.8f, 0.0f,  0.0f, 1.0f, 0.0f,
		// Points
		-0.1f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.15f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.2f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.25f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.3f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.35f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.4f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.45f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.55f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.6f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.65f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.7f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.75f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.8f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.85f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.9f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.95f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.95f, -0.1f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.95f, -0.2f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.95f, -0.3f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.95f, -0.4f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.95f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.95f, -0.6f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.95f, -0.7f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.95f, -0.8f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.95f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.1f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.15f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.2f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.25f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.3f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.35f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.4f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.45f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.55f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.6f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.65f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.7f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.75f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.8f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.85f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.9f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.1f, -0.1f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.1f, -0.2f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.1f, -0.3f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.1f, -0.4f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.1f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.1f, -0.6f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.1f, -0.7f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.1f, -0.8f, 0.0f,  0.0f, 1.0f, 0.0f
	};

	// Double triangles indices
	unsigned int indices[] = {
		0, 1, 2,
		3, 4, 5
	};

	// Vertex Buffer Object and Element Buffer Object
	GLuint VBO, EBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	do {
		glfwPollEvents();
		// New OpenGL3 frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// Define ImGUI window elements
		{
			ImGui::Begin("Color Editer");
			ImGui::Text("Change Colors of Triangle ~");
			ImGui::ColorEdit3("Left Angle", (float*)&colors[1], 1);
			ImGui::ColorEdit3("Top Angle", (float*)&colors[2], 1);
			ImGui::ColorEdit3("Right Angle", (float*)&colors[0], 1);

			ImGui::Checkbox("Change The Whole Color of Triangle ~", &drawAll);
			ImGui::ColorEdit3("Color", (float*)&color_all, 1);

			ImGui::Text("Bonus Show Switch ~");
			ImGui::Checkbox("Show Bonus", &showBonus);
			
			// Check whether draw all into one color
			if (drawAll) {
				for (int i = 0; i < 3; i++) {
					vertices[6 * i + 3] = color_all.x;
					vertices[6 * i + 4] = color_all.y;
					vertices[6 * i + 5] = color_all.z;
				}
			}
			else {
				for (int i = 0; i < 3; i++) {
					vertices[6 * i + 3] = colors[i].x;
					vertices[6 * i + 4] = colors[i].y;
					vertices[6 * i + 5] = colors[i].z;
				}
			}

			ImGui::End();
		}

		// Clear the screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(programID);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// Binding EBO buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Position Attributes binding
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// Color Attributes binding
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// Draw lines, points and triangles
		if (showBonus) {
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glDrawArrays(GL_LINES, 6, 8);
			glDrawArrays(GL_POINTS, 10, 53);
		}
		else {
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		}

		glDisableVertexAttribArray(0);
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