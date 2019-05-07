#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	unsigned int id;
	Shader(const char* vertexPath, const char* fragmentPath);
	void useProgram();
	void setColor(const std::string &name, float r, float g, float b, float a) const;

	void setTransform(const glm::mat4 trans) const;
private:
};

#endif