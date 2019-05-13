#include "Shader.hpp"

const char *FILE_READ_FAILURE = "Read file failed.";
const char *VERTEXSHADER_COMPILE_FAILURE = "Vertex shader compile failed.";
const char *FRAGMENTSHADER_COMPILE_FAILURE = "Fragment shader compile failed.";
const char *SHADER_PROGRAM_LINKING_FAILURE = "Shader program linking failed.";
const unsigned int LOG_LENGTH = 512;

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	std::string vertexSource;
	std::string fragmentSource;
	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;

	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {	
		vertexShaderFile.open(vertexPath);
		fragmentShaderFile.open(fragmentPath);

		std::stringstream vertexStream, fragmentStream;
		
		vertexStream << vertexShaderFile.rdbuf();
		fragmentStream << fragmentShaderFile.rdbuf();

		vertexShaderFile.close();
		fragmentShaderFile.close();

		vertexSource = vertexStream.str();
		fragmentSource = fragmentStream.str();
	} catch (std::ifstream::failure e) {
		std::cout << FILE_READ_FAILURE << std::endl;
	}

	const char* vertexCode = vertexSource.c_str();
	const char* fragmentCode = fragmentSource.c_str();
	int success;

	char log[LOG_LENGTH];

	//编译顶点着色器并检查是否成功
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexCode, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, LOG_LENGTH, NULL, log);
		std::cout << VERTEXSHADER_COMPILE_FAILURE << log << std::endl;
	}

	//编译片段着色器并检查是否成功
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, LOG_LENGTH, NULL, log);
		std::cout << FRAGMENTSHADER_COMPILE_FAILURE << log << std::endl;
	}

	//将着色器链接至主程序并检查是否成功
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, LOG_LENGTH, NULL, log);
		std::cout << SHADER_PROGRAM_LINKING_FAILURE << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::useProgram() {
	glUseProgram(id);
}

void Shader::setModel(const glm::mat4 model) const {
	setMatrix("model", model);
}

void Shader::setView(const glm::mat4 view) const {
	setMatrix("view", view);
}

void Shader::setProjection(const glm::mat4 projection) const {
	setMatrix("projection", projection);
}

void Shader::setMatrix(const std::string &name, const glm::mat4 mat) const {
	unsigned int loc = glGetUniformLocation(id, name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec3(const std::string &name, float x1, float x2, float x3) const {
	glUniform3f(glGetUniformLocation(id, name.c_str()), x1, x2, x3);
}

void Shader::setVec4(const std::string &name, float x1, float x2, float x3, float x4) const {
	glUniform4f(glGetUniformLocation(id, name.c_str()), x1, x2, x3, x4);
}

void Shader::setFloat(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setInteger(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

