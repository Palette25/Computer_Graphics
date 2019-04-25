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

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 lightPos(5.0f, 1.0f, 2.0f);
glm::vec3 viewPos(0.0f, 1.0f, 5.0f);

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
	window = glfwCreateWindow(1280, 720, "CG_HW6", NULL, NULL);
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
	GLuint object_VAO;
	glGenVertexArrays(1, &object_VAO);
	glBindVertexArray(object_VAO);

	// Create and compile our GLSL program from the shaders
	GLuint lightingID = LoadShaders("lighting.vs", "lighting.fs");
	GLuint sunID = LoadShaders("sun.vs", "sun.fs");

	// OpenGL Drawing Vertex array
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	// Vertex Buffer Object and Element Buffer Object
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint sun_VAO;
    glGenVertexArrays(1, &sun_VAO);
    glBindVertexArray(sun_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);

	float ambientStrength = 0.1f, diffuseStrength = 1.0f, 
			specularStrength = 0.5f, reflectStrength = 1.0f;

	bool mode = false, showBonus = false;

	do {
		glfwPollEvents();
		// New OpenGL3 frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		{
			ImGui::Begin("Sun Light Editer");

			ImGui::Checkbox("Switch to Gouraud Shading", &mode);

			ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 2.0f);
			ImGui::SliderFloat("Diffuse Strength", &diffuseStrength, 0.0f, 2.0f);
			ImGui::SliderFloat("Specular Strength", &specularStrength, 0.0f, 2.0f);
			ImGui::SliderFloat("Reflect Strength", &reflectStrength, 0.0f, 2.0f);

			ImGui::Checkbox("Show Bonus", &showBonus);

			ImGui::End();
		}

		glm::mat4 model(1.0f);
		model = glm::rotate(model, 1.0f, glm::vec3(0.0f, 0.5f, 0.0f));

		glm::mat4 view;
		glm::mat4 projection;
		view = glm::lookAt(
			viewPos,
			glm::vec3(0, 0, 2),
			glm::vec3(0, 1, 0)
		);

		if (showBonus) {
			lightPos.x = sin(glfwGetTime()) * 5.0f;
			lightPos.z = cos(glfwGetTime()) * 5.0f;
			model = glm::rotate(model, (float)glfwGetTime() * 1.5f, glm::vec3(0.0f, 1.0f, 1.0f));
		}

		// First use the lighting vertex shader and fragment shader program
		glUseProgram(lightingID);

		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		// Define Cube Color and Light Color
		glm::vec3 objectColor = glm::vec3(0.5f, 0.7f, 0.2f);
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

		// Bind MVP Matrix
		unsigned int modelLoc = glGetUniformLocation(lightingID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		unsigned int viewLoc = glGetUniformLocation(lightingID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		unsigned int projectionLoc = glGetUniformLocation(lightingID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Bind Color
		unsigned int objectColorLoc = glGetUniformLocation(lightingID, "objectColor");
		glUniform3fv(objectColorLoc, 1, glm::value_ptr(objectColor));
		unsigned int lightColorLoc = glGetUniformLocation(lightingID, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
		// Bind Position
		unsigned int lightPosLoc = glGetUniformLocation(lightingID, "lightPos");
		glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
		unsigned int viewPosLoc = glGetUniformLocation(lightingID, "viewPos");
		glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));
		// Bind Strength
		unsigned int ambientLoc = glGetUniformLocation(lightingID, "ambientStrength");
		glUniform1f(ambientLoc, ambientStrength);
		unsigned int diffuseLoc = glGetUniformLocation(lightingID, "diffuseStrength");
		glUniform1f(diffuseLoc, diffuseStrength);
		unsigned int specularLoc = glGetUniformLocation(lightingID, "specularStrength");
		glUniform1f(specularLoc, specularStrength);
		unsigned int reflectLoc = glGetUniformLocation(lightingID, "reflectStrength");
		glUniform1f(reflectLoc, reflectStrength);
		// Bind Mode
		unsigned int modeLoc = glGetUniformLocation(lightingID, "mode");
		glUniform1i(modeLoc, (mode ? 1 : 0));

		glBindVertexArray(object_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Then use the object vertex shader and fragment shader program
		glUseProgram(sunID);

		viewLoc = glGetUniformLocation(sunID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		projectionLoc = glGetUniformLocation(sunID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Change model loc
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::translate(model, lightPos);
		modelLoc = glGetUniformLocation(sunID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(sun_VAO);
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
	glDeleteBuffers(1, &object_VAO);
	glDeleteBuffers(1, &sun_VAO);
	glDeleteProgram(lightingID);
	glDeleteProgram(sunID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}