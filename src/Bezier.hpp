#ifndef BEZIER_HPP
#define BEZIER_HPP

#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>

class Bezier {
public:
	Bezier();
	bool push(glm::vec3 point);
	bool pop();
	std::vector<glm::vec3> linearCombination(std::vector<glm::vec3> vertices, float t);
	std::vector<glm::vec3> bezierCurve(int n);
	std::vector<glm::vec3> getVertices();
private:
	std::vector<glm::vec3> vertices;
	int C(int n, int i);
	float B(int n, int i, float t);
	
};

#endif
