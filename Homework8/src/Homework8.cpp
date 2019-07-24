// Include standard headers
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Include GLEW
#include <GL/glew.h>
#pragma comment(lib, "glew32.lib")

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
using namespace glm;

#include <common/imgui.h>
#include <common/imgui_impl_opengl3.h>
#include <common/imgui_impl_glfw.h>
#include <common/shader.hpp>

#define MAX_POINT_NUM 10000
#define MAX_CONTROL_NUM 10
#define STEP 0.001

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

const unsigned int CLICK_BOX_X = 30;
const unsigned int CLICK_BOX_Y = 30;

struct Point {
	double x;
	double y;
	Point() {}
	Point(double x_, double y_) {
		this->x = x_;
		this->y = y_;
	}
};

vector<Point> LastAddPoints;
vector<Point> ControlPoints;
// Drawa Curve Point Array
float points[MAX_POINT_NUM];
float control_points[MAX_CONTROL_NUM * 6];
float control_lines[2 * (MAX_CONTROL_NUM - 1) * 6];
// For showing the animation of Bezier Curve Generation
float bezier_points[MAX_CONTROL_NUM * (MAX_CONTROL_NUM - 1) / 2 * 6];
float bezier_lines[(MAX_CONTROL_NUM-1) * (MAX_CONTROL_NUM - 2) * 6];
float percent = 0.0f;

glm::vec3 line_color(1.0f, 0.0f, 0.0f);
glm::vec3 ctrl_color(1.0f, 1.0f, 0.0f);

int frac(int source) {
	if (source == 0)
		return 1;

	for (int i = source-1; i >= 1; i--) {
		source *= i;
	}

	return source;
}

// Linear / Quadratic / Cubic polynomial Bezier Curve
Point getBezier(vector<Point> points, float step) {
	int mode = points.size();
	// According to control points size, decide Bezier Curve's mode
	// Calculating factors: B_(i, n)(t)
	int n = mode - 1;
	float* B = new float[mode];

	for (int i = 0; i < mode; i++) {
		B[i] = frac(n) / (frac(i) * frac(n - i)) * pow(step, i) * pow((1 - step), n - i);
	}

	// Resulting the points' location
	float xpos = 0.0f, ypos = 0.0f;
	for (int i = 0; i <= n; i++) {
		xpos += points[i].x * B[i];
		ypos += points[i].y * B[i];
	}

	delete B;
	return Point(xpos, ypos);
}

float Normalize(int raw, bool mode) {
	int fractor = mode ? SCR_HEIGHT / 2 : SCR_WIDTH / 2;
	return float(raw) / fractor;
}

void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods);
int GetBezierCurveAtTime();
int GetCurrentCurvePoints();
void SortControlPoints();

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
	window = glfwCreateWindow(1280, 720, "CG_HW8", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, MouseButtonCallBack);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW.\n");
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
	GLuint programID = LoadShaders("shader.vs", "shader.fs");

	// Vertex Buffer Object and Element Buffer Object
	GLuint VBO, VCO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VCO);

	bool showBonus = false;

	do {
		glfwPollEvents();
		// New OpenGL3 frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// Define ImGUI window elements
		{
			ImGui::Begin("Bezier Curve Drawer");

			ImGui::ColorEdit3("Curve Color", (float*)&line_color);
			ImGui::ColorEdit3("Control-Point Color", (float*)&ctrl_color);

			ImGui::Checkbox("Show Bonus", &showBonus);
			ImGui::Text("\n");

			// Dynamic show control points location
			string cp_number = "Current Control Point Number: " + to_string(ControlPoints.size()) 
									+ " (MAX_NUM: " + to_string(MAX_CONTROL_NUM) + ")";
			ImGui::Text(cp_number.c_str());
			for (int index = 0; index < ControlPoints.size(); index++) {
				string ctrl_text = "Control Point " + to_string(index) + " : ";
				ctrl_text += "(";
				ctrl_text += to_string(Normalize(ControlPoints[index].x, false));
				ctrl_text += ", ";
				ctrl_text += to_string(Normalize(ControlPoints[index].y, true));
				ctrl_text += ")";
				ImGui::Text(ctrl_text.c_str());
			}
			ImGui::Text("\n");

			ImGui::End();
		}

		// Clear the screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(programID);
		glPointSize(6.0f);

		int curr_offset = 0.0f;

		if (showBonus) {
			if (percent >= 1.0f) {
				curr_offset = GetCurrentCurvePoints();
			}
			else {
				// Dynamic showing the generation of Bezier Curve
				curr_offset = GetBezierCurveAtTime();
			}
		}
		else {
			percent = 0.0f;
			// Directly and Staticly Draw Bezier Curve
			curr_offset = GetCurrentCurvePoints();
		}

		// Render main Bezier Curve Points
		if (curr_offset > 0) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glDrawArrays(GL_POINTS, 0, curr_offset / 6);
		}

		// Render Control Points
		if (ControlPoints.size() > 0) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(control_points), control_points, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glDrawArrays(GL_POINTS, 0, ControlPoints.size());
			// Draw Lines
			glBufferData(GL_ARRAY_BUFFER, sizeof(control_lines), control_lines, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glDrawArrays(GL_LINES, 0, 2 * (ControlPoints.size() - 1));
		}

		// Render Bonus -- Animation Of Bezier Curve Generation Drawing Points
		if (showBonus && percent > 0.0f && percent <= 1.0f) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(bezier_points), bezier_points, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			// Calculating Animation Points Size
			int animation_size = 0;
			for (int i = ControlPoints.size() - 1; i >= 1; i--) {
				animation_size += i;
			}

			glDrawArrays(GL_POINTS, 0, animation_size);

			animation_size = 0;
			for (int i = ControlPoints.size() - 2; i >= 1; i--) {
				animation_size += i;
			}

			// Render Animation Lines
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(bezier_lines), bezier_lines, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glDrawArrays(GL_LINES, 0, animation_size * 2);
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
	glDeleteBuffers(1, &VCO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VAO);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{
	if(action == GLFW_PRESS) {
		if(button == GLFW_MOUSE_BUTTON_LEFT) {
			// Reset Persent
			percent = 0.0f;

			double xCur, yCur;
			int point_num = ControlPoints.size();
			if(point_num < MAX_CONTROL_NUM) {
				// Add a new Control Point
				glfwGetCursorPos(window, &xCur, &yCur);
				// Normalize cursor position
				xCur =(xCur - SCR_WIDTH / 2);
				yCur = (-yCur + SCR_HEIGHT / 2);
				// New the control point
				ControlPoints.push_back(Point(xCur, yCur));
				LastAddPoints.push_back(Point(xCur, yCur));
			}
		}
		else if(button == GLFW_MOUSE_BUTTON_RIGHT && LastAddPoints.size() > 0) {
			// Reset Persent
			percent = 0.0f;

			// Delete Last Adding Control Point
			for(int i = 0; i < ControlPoints.size(); i++) {
				if(ControlPoints[i].x == LastAddPoints.back().x 
					&& ControlPoints[i].y == LastAddPoints.back().y) 
				{
					ControlPoints.erase(ControlPoints.begin() + i);
					LastAddPoints.pop_back();
					break;
				}
			}
		}
	}
}

void SortControlPoints()
{
	for(int i = 0; i < ControlPoints.size(); i++) {
		for(int j = i+1; j < ControlPoints.size(); j++) {
			if(ControlPoints[i].x > ControlPoints[j].x) {
				Point temp = ControlPoints[i];
				ControlPoints[i] = ControlPoints[j];
				ControlPoints[j] = temp;
			}
		}
	}
}

void SetControlPoints(int point_num) 
{
	int _offset = 0;
	// Set control points
	for (int i = 0; i < point_num; i++) {
		control_points[_offset++] = Normalize(ControlPoints[i].x, false);
		control_points[_offset++] = Normalize(ControlPoints[i].y, true);
		control_points[_offset++] = 0.0f;
		// Set Color
		control_points[_offset++] = ctrl_color.x;
		control_points[_offset++] = ctrl_color.y;
		control_points[_offset++] = ctrl_color.z;
	}
	// Set control lines
	_offset = 0;
	for (int i = 0; i < point_num; i++) {
		int iter_time = 1;
		if (i > 0 && i < MAX_CONTROL_NUM-1) {
			iter_time = 2;
		}
		for (int k = 0; k < iter_time; k++) {
			control_lines[_offset++] = Normalize(ControlPoints[i].x, false);
			control_lines[_offset++] = Normalize(ControlPoints[i].y, true);
			control_lines[_offset++] = 0.0f;
			// Set Color
			control_lines[_offset++] = ctrl_color.x;
			control_lines[_offset++] = ctrl_color.y;
			control_lines[_offset++] = ctrl_color.z;
		}
	}
}

int GetCurrentCurvePoints() 
{
	int point_num = ControlPoints.size(), offset = 0;
	// Clear Curve Points
	memset(points, 0, sizeof(points));
	memset(control_points, 0, sizeof(control_points));
	memset(control_lines, 0, sizeof(control_lines));

	if(point_num <= MAX_CONTROL_NUM && point_num >= 2) {
		// Sort Control Points by X-axis
		SortControlPoints();
		for(float step = 0.0; step <= 1.0; step += STEP) {
			Point temp = getBezier(ControlPoints, step);
			points[offset++] = Normalize(temp.x, false);
			points[offset++] = Normalize(temp.y, true);
			points[offset++] = 0.0f;
			// Set Color
			points[offset++] = line_color.x;
			points[offset++] = line_color.y;
			points[offset++] = line_color.z;
		}
	}

	SetControlPoints(point_num);

	return offset;
}

int GetBezierCurveAtTime() 
{
	percent += 0.001;
	int point_num = ControlPoints.size(), offset = 0;
	// Clear Curve Points
	memset(points, 0, sizeof(points));
	memset(control_points, 0, sizeof(control_points));
	memset(control_lines, 0, sizeof(control_lines));
	memset(bezier_points, 0, sizeof(bezier_points));
	memset(bezier_lines, 0, sizeof(bezier_lines));

	if (point_num <= MAX_CONTROL_NUM && point_num >= 2) {
		// Sort Control Points by X-axis
		SortControlPoints();
		for (float step = 0.0; step <= percent; step += 0.001) {
			Point temp = getBezier(ControlPoints, step);
			points[offset++] = Normalize(temp.x, false);
			points[offset++] = Normalize(temp.y, true);
			points[offset++] = 0.0f;
			// Set Color
			points[offset++] = line_color.x;
			points[offset++] = line_color.y;
			points[offset++] = line_color.z;
		}
		// Generate bezier curve animation points -- For Bonus
		int iter_time = point_num - 1;
		vector<Point> animation_points;
		vector<Point> edge_points;

		// Init the Edge Points as the Control Points
		for (int i = 0; i < point_num; i++) {
			edge_points.push_back(ControlPoints[i]);
		}

		while (iter_time--) {
			// Adding animation points
			int pre_size = animation_points.size(), after_size = -1;
			for (int i = 0; i <= iter_time; i++) {
				double xpos = percent * edge_points[i + 1].x + (1 - percent) * edge_points[i].x,
						ypos = percent * edge_points[i + 1].y + (1 - percent) * edge_points[i].y;
				animation_points.push_back(Point(xpos, ypos));
			}
			after_size = animation_points.size();

			// Reset Edge Points
			edge_points.clear();
			vector<Point>().swap(edge_points);
			for (int i = pre_size; i < after_size; i++) {
				edge_points.push_back(animation_points[i]);
			}
		}

		// Set the animation points location array
		int _offset = 0;
		for (int i = 0; i < animation_points.size(); i++) {
			bezier_points[_offset++] = Normalize(animation_points[i].x, false);
			bezier_points[_offset++] = Normalize(animation_points[i].y, true);
			bezier_points[_offset++] = 0.0f;
			// Set Color
			bezier_points[_offset++] = ctrl_color.x;
			bezier_points[_offset++] = ctrl_color.y;
			bezier_points[_offset++] = ctrl_color.z;
		}

		// Set the animation lines location array
		_offset = 0;
		int cal_num = point_num - 1, temp = cal_num - 1;
		bool ignore_next = false;

		for (int i = 0; i < animation_points.size(); i++) {
			int iter_time = 1;
			if (i > 0 && i < animation_points.size() - 1) {
				if (i == cal_num - 1) {
					cal_num += temp;
					temp--;
					ignore_next = true;
				}
				else {
					if (!ignore_next) {
						iter_time = 2;
					}
					
					ignore_next = false;
				}
			}
			for (int k = 0; k < iter_time; k++) {
				bezier_lines[_offset++] = Normalize(animation_points[i].x, false);
				bezier_lines[_offset++] = Normalize(animation_points[i].y, true);
				bezier_lines[_offset++] = 0.0f;
				// Set Color
				bezier_lines[_offset++] = ctrl_color.x;
				bezier_lines[_offset++] = ctrl_color.y;
				bezier_lines[_offset++] = ctrl_color.z;
			}
		}
	}

	SetControlPoints(point_num);

	return offset;
}