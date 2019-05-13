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
	void setModel(const glm::mat4 model) const;
	void setView(const glm::mat4 view) const;
	void setProjection(const glm::mat4 projection) const;
	void setVec3(const std::string &name, float x1, float x2, float x3) const;
	void setVec4(const std::string &name, float x1, float x2, float x3, float x4) const;
	void setFloat(const std::string &name, float value) const;
	void setInteger(const std::string &name, int value) const;
	void setMatrix(const std::string &name, const glm::mat4 mat) const;
private:
};

#endif