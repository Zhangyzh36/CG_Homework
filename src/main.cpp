// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Bezier.hpp"
#include <stdio.h>
#include <math.h>
#include <iostream>

// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually.
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void drawBezier(Bezier &bezier, float t);
void drawLine(const std::vector<glm::vec3> &points);
void drawPoint(const std::vector<glm::vec3> &points);
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_WIDTH / 2.0f;
bool firstMouse = true;

enum STATE{PAUSE, INCREASE, DECREASE};
STATE state = PAUSE;
STATE lastState = PAUSE;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
int display_w = 1024;
int display_h = 1024;

glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

unsigned int VAO;
unsigned int VBO;
Bezier bezier;
float t = 0.25;

int main(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	const char* glsl_version = "#version 130";
	const char *TITLE = "Homework8";

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE, NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	glfwSetCursorPosCallback(window, mouseCallback);
	//glfwSetScrollCallback(window, scrollCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = gladLoadGL() == 0;
#else
	bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	Shader myShader("Shader.v", "Shader.f");
	Shader myShaderBezier("Shader.v", "ShaderBezier.f");
	glEnable(GL_DEPTH_TEST);


	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &display_w, &display_h);
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	


	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin(TITLE);
		//ImGui::SliderFloat("t", &t, 0, 1);
		ImGui::End();
		ImGui::Render();

		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float delta = 0.005;
		if (state == INCREASE) {
			t += delta;
			if (t > 1) {
				t = 1;
				state = PAUSE;
				lastState = DECREASE;
			}
		}
		if (state == DECREASE) {
			t -= delta;
			if (t < 0) {
				t = 0;
				state = PAUSE;
				lastState = INCREASE;
			}
		}

		myShader.useProgram();
		drawBezier(bezier, t);
		myShaderBezier.useProgram();
		drawLine(bezier.bezierCurve(100));
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	lastX = xpos;
	lastY = ypos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		std::cout << "left" << std::endl;
		bezier.push(glm::vec3(lastX, lastY, 0));
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		{
			std::cout << "middle" << std::endl;
			if (state == PAUSE && lastState == PAUSE) {
				state = lastState = INCREASE;
			}
			else if (state == PAUSE && lastState != PAUSE) {
				state = lastState;
			}
			else if (state != PAUSE) {
				state = PAUSE;
			}
		}
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		std::cout << "right" << std::endl;
		bezier.pop();
		break;
	default:
		return;
	}
	return;
}

void drawBezier(Bezier &bezier, float t) {
	std::vector<glm::vec3> vertices = bezier.getVertices();
	int size = vertices.size();
	drawLine(vertices);
	for (int i = 0; i < size - 1; ++i) {
		std::vector<glm::vec3> temp(bezier.linearCombination(vertices, t));
		vertices.swap(temp);
		drawLine(vertices);
	}
}

void drawLine(const std::vector<glm::vec3> &points) {
	if (points.size() == 0) {
		return;
	}

	std::vector<float> vertices;
	int w = display_w / 2;
	int h = display_h / 2;

	for (int i = 0; i < points.size(); ++i) {
		vertices.push_back((points[i].x - w)/ w);
		vertices.push_back((h - points[i].y)/ h);
		vertices.push_back(points[i].z);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindVertexArray(VAO);
	if (points.size() > 1) {
		glDrawArrays(GL_LINE_STRIP, 0, points.size());
	}
	else {
		glDrawArrays(GL_POINT, 0, points.size());
	}
	glBindVertexArray(0);
}
/*
void drawPoint(const std::vector<glm::vec3> &points) {
	if (points.size() == 0) {
		return;
	}

	std::vector<float> vertices;
	int w = display_w / 2;
	int h = display_h / 2;

	for (int i = 0; i < points.size(); ++i) {
		vertices.push_back((points[i].x - w) / w);
		vertices.push_back((h - points[i].y) / h);
		vertices.push_back(points[i].z);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_POINTS, 0, points.size());
	glBindVertexArray(0);
}
*/