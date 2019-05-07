// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Shader.hpp"
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

int main(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	const char* glsl_version = "#version 130";
	const char *TITLE = "Homework4";
	const int SCREEN_WIDTH = 1024;
	const int SCREEN_HEIGHT = 1024;
	

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE, NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

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

	Shader myShader("Shader.v", "Shader.f");

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	float vertices[] = {
		-0.2f, -0.2f, -0.2f, 1.0f, 1.0f, 1.0f,
		0.2f, -0.2f, -0.2f,0.0f, 1.0f, 1.0f,
		0.2f, 0.2f, -0.2f,0.0f, 0.0f, 1.0f,
		0.2f, 0.2f, -0.2f,0.0f, 0.0f, 1.0f,
		-0.2f, 0.2f, -0.2f,1.0f, 0.0f, 1.0f,
		-0.2f, -0.2f, -0.2f,1.0f, 1.0f, 1.0f,

		-0.2f, -0.2f, 0.2f,1.0f, 1.0f, 0.0f,
		0.2f, -0.2f, 0.2f,0.0f, 1.0f, 0.0f,
		0.2f, 0.2f, 0.2f,0.0f, 0.0f, 0.0f,
		0.2f, 0.2f, 0.2f,0.0f, 0.0f, 0.0f,
		-0.2f, 0.2f, 0.2f,1.0f, 0.0f, 0.0f,
		-0.2f, -0.2f, 0.2f,1.0f, 1.0f, 0.0f,

		-0.2f, 0.2f, 0.2f,1.0f, 0.0f, 0.0f,
		-0.2f, 0.2f, -0.2f,1.0f, 0.0f, 1.0f,
		-0.2f, -0.2f, -0.2f,1.0f, 1.0f, 1.0f,
		-0.2f, -0.2f, -0.2f,1.0f, 1.0f, 1.0f,
		-0.2f, -0.2f, 0.2f,1.0f, 1.0f, 0.0f,
		-0.2f, 0.2f, 0.2f,1.0f, 0.0f, 0.0f,

		0.2f, 0.2f, 0.2f,0.0f, 0.0f, 0.0f,
		0.2f, 0.2f, -0.2f,0.0f, 0.0f, 1.0f,
		0.2f, -0.2f, -0.2f,0.0f, 1.0f, 1.0f,
		0.2f, -0.2f, -0.2f,0.0f, 1.0f, 1.0f,
		0.2f, -0.2f, 0.2f,0.0f, 1.0f, 0.0f,
		0.2f, 0.2f, 0.2f,0.0f, 0.0f, 0.0f,

		-0.2f, -0.2f, -0.2f,1.0f, 1.0f, 1.0f,
		0.2f, -0.2f, -0.2f,0.0f, 1.0f, 1.0f,
		0.2f, -0.2f, 0.2f,0.0f, 1.0f, 0.0f,
		0.2f, -0.2f, 0.2f,0.0f, 1.0f, 0.0f,
		-0.2f, -0.2f, 0.2f,1.0f, 1.0f, 0.0f,
		-0.2f, -0.2f, -0.2f,1.0f, 1.0f, 1.0f,

		-0.2f, 0.2f, -0.2f,1.0f, 0.0f, 1.0f,
		0.2f, 0.2f, -0.2f,0.0f, 0.0f, 1.0f,
		0.2f, 0.2f, 0.2f,0.0f, 0.0f, 0.0f,
		0.2f, 0.2f, 0.2f,0.0f, 0.0f, 0.0f,
		-0.2f, 0.2f, 0.2f,1.0f, 0.0f, 0.0f,
		-0.2f, 0.2f, -0.2f,1.0f, 0.0f, 1.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);     
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));     
	glEnableVertexAttribArray(1);


	bool isTranslate = false;
	bool isRotate = false;
	bool isScale = false;
	bool depthTest = false;
	bool isSpiral = false;

	int r = 0;
	int display_w, display_h;
	glfwMakeContextCurrent(window);
	while (!glfwWindowShouldClose(window))
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			ImGui::Begin(TITLE);
			ImGui::Checkbox("depth test", &depthTest);
			ImGui::Checkbox("translate", &isTranslate);
			ImGui::Checkbox("spiral", &isSpiral);
			ImGui::Checkbox("rotate", &isRotate);
			ImGui::Checkbox("scale", &isScale);
			ImGui::End();
		}

		// Rendering
		ImGui::Render();

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (depthTest) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}

		glm::mat4 trans = glm::mat4(1.0f);
		//trans = glm::translate(trans, glm::vec3(translateX, 0.0f, 0.0f));
		
		if (isTranslate) {
			trans = glm::translate(trans, glm::vec3(sin(glfwGetTime()), 0.0f, 0.0f));
		} 
		if (isSpiral) {
			float alpha = 0.5;
			float beta = 0.5;
			float theta = glfwGetTime();
			std::cout << theta << std::endl;
			float x = 0.1 * (alpha + beta * theta) * cos(3 * theta);
			float y = 0.1 * (alpha + beta * theta) * sin(3 * theta);
			trans = glm::translate(trans, glm::vec3(x, y, 0.0f));
		}
		if (isRotate) {
			float radian = 100.0f * glfwGetTime();
			trans = glm::rotate(trans, glm::radians(radian), glm::vec3(1.0, 0.0, 1.0));
		}
		if (isScale) {
			float scale = 0.5f * sin(2 * glfwGetTime()) + 1.0f;
			trans = glm::scale(trans, glm::vec3(scale, scale, scale));
		}

		myShader.useProgram();
		myShader.setTransform(trans);

		glBindVertexArray(VAO);
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
