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

#define relative_width 640
#define relative_height 360

// Drawing functions predefinitions
void drawTriangleWithBresenham(int x0, int y0, int x1, int y1, int x2, int y2, vector<int>& points);
void drawCircleWithBresenham(int x0, int y0, int radius, vector<int>& points);
void circlePlot(int x0, int y0, int x, int y, vector<int>& points);

// Extract float points-array from Bresenham produced results
void produceVertices(vector<int>& points, float* vertices);

// Max and Min tool functions
int max(int a, int b, int c);
int min(int a, int b, int c);

// Rasterization
vector<int> getLineEquation(int x0, int y0, int x1, int y1);
vector<int> rasterizeTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

// Bresenham algorithm
vector<int> Bresenham(int x0, int y0, int x1, int y1){
	vector<int> points;
	// Push origin begin points
	points.push_back(x0);
	points.push_back(y0);
	// Direction checking
	int dx = x1 - x0, dy = y1 - y0;
	int direct_x = dx > 0 ? 1 : -1, direct_y = dy > 0 ? 1 : -1;
	if(dx < 0) dx = -dx;
	if(dy < 0) dy = -dy;

	if(abs(dx)> abs(dy)){
		int p = 2 * dy - dx, x = x0, y = y0;
		// Repeating loop
		while(x != x1){
			points.push_back(x);
			points.push_back(y);
			if(p> 0){
				y += direct_y;
				p = p + 2 * dy - 2 * dx;
			}else {
				p = p + 2 * dy;
			}
			x += direct_x;
		}
	}else {
		int p = 2 * dx - dy, x = x0, y = y0;
		while(y != y1){
			points.push_back(x);
			points.push_back(y);
			if(p > 0){
				x += direct_x;
				p = p + 2 * dx - 2 * dy;
			}else {
				p = p + 2 * dx;
			}
			y += direct_y;
		}
	}
	// Push origin End points
	points.push_back(x1);
	points.push_back(y1);
	return points;
}

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
	window = glfwCreateWindow(1280, 720, "CG_HW3", NULL, NULL);
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
	bool drawTriangle = true, drawCircle = false, showBonus = false, alreadyRas = false;
	float radius = 100.0f;
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

	// Vertex Buffer Object and Element Buffer Object
	GLuint VBO, EBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	do {
		glfwPollEvents();
		// New OpenGL3 frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// Define ImGUI window elements
		{
			ImGui::Begin("Triangle and Circle Drawer");
			ImGui::Checkbox("Draw The Triangle ~", &drawTriangle);
			ImGui::Checkbox("Draw The Circle ~", &drawCircle);
			if (drawCircle) {
				ImGui::Text("Change Radius of the drawing circle ~");
				ImGui::SliderFloat("Radius", &radius, 0.0f, 200.0f);
			}

			ImGui::Text("\nRasterize The Triangle ~");
			ImGui::Checkbox("Rasterize", &showBonus);

			ImGui::End();
		}
		// Get produces points from Bresenham
		vector<int> points;

		// Draw Triangle
		if(drawTriangle){
			if(showBonus){
				points = rasterizeTriangle(-200, 180, 100, -200, -500, -200);
			}else {
				drawTriangleWithBresenham(-200, 180, 100, -200, -500, -200, points);
			}
		}
		// Draw Circle
		if(drawCircle){
			drawCircleWithBresenham(300, 0, (int)radius, points);
		}

		float* vertices = new float[points.size() / 2 * 6];
		produceVertices(points, vertices);

		// Clear the screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(programID);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, points.size() / 2 * 6 * sizeof(float), vertices, GL_STREAM_DRAW);

		// Position Attributes binding
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// Color Attributes binding
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		
		glDrawArrays(GL_POINTS, 0, points.size() / 2);

		glDisableVertexAttribArray(0);

		// Render ImGUI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		delete[] vertices;

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


void drawTriangleWithBresenham(int x0, int y0, int x1, int y1, int x2, int y2, vector<int>& points){
	vector<vector<int>> edges;
	// Build triangle with three pairs of points, using Bresenham algorithm
	edges.push_back(Bresenham(x0, y0, x1, y1));
	edges.push_back(Bresenham(x1, y1, x2, y2));
	edges.push_back(Bresenham(x2, y2, x0, y0));
	// Make points
	for(int i=0; i<edges.size(); i++){
		for(int j=0; j<edges[i].size(); j++){
			points.push_back(edges[i][j]);
		}
	}
}

void drawCircleWithBresenham(int x0, int y0, int radius, vector<int>& points){
	int x = 0, y = radius, dis = 3 - 2 * radius;
	const int multiper = 4, pos_len = 10, neg_len = 6;
	circlePlot(x0, y0, x, y, points);
	// Drawing loop
	while(x < y){
		if(dis < 0){
			dis += multiper * x + neg_len;
		}else {
			dis += multiper * (x - y) + pos_len;
			--y;
		}
		++x;
		circlePlot(x0, y0, x, y, points);
	}
}

void circlePlot(int src_x, int src_y, int x, int y, vector<int>& points){
	// Divide into eight parts to search points to build a circle
	points.push_back(src_x + x); points.push_back(src_y + y);
	points.push_back(src_x + x); points.push_back(src_y - y);
	points.push_back(src_x - x); points.push_back(src_y + y);
	points.push_back(src_x - x); points.push_back(src_y - y);
	points.push_back(src_x + y); points.push_back(src_y + x);
	points.push_back(src_x + y); points.push_back(src_y - x);
	points.push_back(src_x - y); points.push_back(src_y + x);
	points.push_back(src_x - y); points.push_back(src_y - x);
}

void produceVertices(vector<int>& points, float* vertices){
	for(int i=0; i<points.size() / 2; i++){
		// Points position
		vertices[i * 6 + 0] = (float)points[i * 2 + 0] / (float)relative_width;
		vertices[i * 6 + 1] = (float)points[i * 2 + 1] / (float)relative_height;
		vertices[i * 6 + 2] = 0.0f;
		// Points color -- Yellow
		vertices[i * 6 + 3] = 1.0f;
		vertices[i * 6 + 4] = 1.0f;
		vertices[i * 6 + 5] = 0.0f;
	}
}

int max(int a, int b, int c){
	return max(a, max(b, c));
}

int min(int a, int b, int c){
	return min(a, min(b, c));
}

// Line Eqation: Ax + By + C = 0
vector<int> getLineEquation(int x0, int y0, int x1, int y1){
	vector<int> result;
	result.push_back(y1 - y0);
	result.push_back(x0 - x1);
	result.push_back(x1 * y0 - x0 * y1);
	return result;
}

vector<int> rasterizeTriangle(int x0, int y0, int x1, int y1, int x2, int y2){
	int max_x = max(x0, x1, x2),
		min_x = min(x0, x1, x2),
		max_y = max(y0, y1, y2),
		min_y = min(y0, y1, y2);
	int cen_x = (max_x - min_x) / 2, cen_y = (max_y - min_y) / 2;
	// Build lines
	vector<vector<int>> lines;
	lines.push_back(getLineEquation(x0, y0, x1, y1));
	lines.push_back(getLineEquation(x1, y1, x2, y2));
	lines.push_back(getLineEquation(x2, y2, x0, y0));
	// Centerlize lines
	int temp_arr[3][2] = {
		{x2, y2}, {x1, y1}, {x0, y0}
	};
	for(int i=0; i<3; i++){
		int temp = lines[i][0] * temp_arr[i][0] + 
				   lines[i][1] * temp_arr[i][1] +
				   lines[i][2];
		if(temp < 0){
			for(int j=0; j<lines[i].size(); j++){
				lines[i][j] *= -1;
			}
		}
	}
	// Generate Pixel results
	vector<int> result;
	for (int x = min_x; x <= max_x; x++) {
		for (int y = min_y; y <= max_y; y++) {
			bool inside = true;
			for (int i = 0; i< 3; i++) {
				int temp = lines[i][0] * x + lines[i][1] * y + lines[i][2];
				if (temp < 0) {
					inside = false;
					break;
				}
			}
			if (inside) {
				result.push_back(x);
				result.push_back(y);
			}
		}
	}
	return result;
}