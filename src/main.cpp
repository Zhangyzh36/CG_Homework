// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Shader.hpp"
#include "Camera.hpp"
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

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_WIDTH / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	const char* glsl_version = "#version 130";
	const char *TITLE = "Homework6";

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE, NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

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

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	Shader phongLighting("PhongShader.v", "PhongShader.f");
	Shader gouraudLighting("GouraudShader.v", "GouraudShader.f");
	Shader lampShader("LampShader.v", "LampShader.f");

	float vertices[] = {
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
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0fa
	};

	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	bool isRotate = false;
	bool isScale = false;
	bool depthTest = true;
	bool isSpiral = false;
	bool isPerspective = false;
	bool isOrthogonal = false;
	bool isCameraSurround = false;

	float radian = 45;
	float nearValue = 0.1;
	float farValue = 100;

	int display_w = 1024;
	int display_h = 1024;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &display_w, &display_h);

	float left = -5;
	float right = 5;
	float bottom = -5;
	float top = 5;

	int projMode = 0;
	int ctrlMode = 3;
	int shaderMode = 5;

	const int PERSPECTIVE = 0;
	const int ORTHOGONAL = 1;
	const int SELF = 2;
	const int MOUSE = 3;
	const int STATIC = 4;

	const int PHONG = 5;
	const int GOURAUD = 6;

	float ambientStrength = 0.1;
	int specularFactor = 32;
	float specularStrength = 1.0;
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

		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			ImGui::Begin(TITLE);
			ImGui::Text("Projection");
			ImGui::RadioButton("Perspective projection", &projMode, PERSPECTIVE);
			ImGui::RadioButton("Orthogonal projectio", &projMode, ORTHOGONAL);
			
			ImGui::Text("Shading");
			ImGui::RadioButton("Phong Shading", &shaderMode, PHONG);
			ImGui::RadioButton("Gouraud Shading", &shaderMode, GOURAUD);

			ImGui::Checkbox("Camera surround", &isCameraSurround);

			ImGui::SliderFloat("ambientStrength", &ambientStrength, 0.0, 1.0);
			ImGui::SliderFloat("specularStrength", &specularStrength, 0.0, 1.0);
			ImGui::SliderInt("specularFactor", &specularFactor, 1, 256);


			ImGui::Text("Depth");
			ImGui::Checkbox("depth test", &depthTest);
			if (projMode == PERSPECTIVE && ctrlMode == STATIC) {
				ImGui::Text("Projection parameters");
				ImGui::SliderFloat("radian", &radian, 1, 89);
				ImGui::SliderFloat("near", &nearValue, -5, 5);
				ImGui::SliderFloat("far", &farValue, 5, 150);
				isOrthogonal = false;
			}
			else if (projMode == ORTHOGONAL && ctrlMode == STATIC) {
				ImGui::Text("Projection parameters");
				ImGui::SliderFloat("left", &left, -5, 5);
				ImGui::SliderFloat("right", &right, -5, 5);
				ImGui::SliderFloat("bottom", &bottom, -5, 5);
				ImGui::SliderFloat("top", &top, -5, 5);
				ImGui::SliderFloat("near", &nearValue, -5, 5);
				ImGui::SliderFloat("far", &farValue, 5, 150);
				isPerspective = false;
			}

			ImGui::End();
		}

		// Rendering
		ImGui::Render();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (depthTest) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}

		if (isCameraSurround) {
			lightPos.x = 2*sin(glfwGetTime());
			lightPos.y = cos(glfwGetTime());
			lightPos.z = 1;
		}

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		radian = camera.Zoom;	
		
		if (projMode == PERSPECTIVE) {
			proj = glm::perspective(glm::radians(radian), (float)display_w / (float)display_h, nearValue, farValue);
		}
		else if (projMode == ORTHOGONAL) {
			proj = glm::ortho(left, right, bottom, top, nearValue, farValue);
		}
	
		if (shaderMode == PHONG) {
			phongLighting.useProgram();
			phongLighting.setModel(model);
			phongLighting.setView(view);
			phongLighting.setProjection(proj);

			phongLighting.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
			phongLighting.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			phongLighting.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
			phongLighting.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
			phongLighting.setFloat("ambientStrength", ambientStrength);
			phongLighting.setFloat("specularStrength", specularStrength);
			phongLighting.setInteger("specularFactor", specularFactor);
		}
		else {
			gouraudLighting.useProgram();
			gouraudLighting.setModel(model);
			gouraudLighting.setView(view);
			gouraudLighting.setProjection(proj);

			gouraudLighting.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
			gouraudLighting.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			gouraudLighting.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
			gouraudLighting.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
			gouraudLighting.setFloat("ambientStrength", ambientStrength);
			gouraudLighting.setFloat("specularStrength", specularStrength);
			gouraudLighting.setInteger("specularFactor", specularFactor);
		}
		
		

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lampShader.useProgram();

		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.1f));

		lampShader.setModel(model);
		lampShader.setView(view);
		lampShader.setProjection(proj);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
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

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}
