#include "Bezier.hpp"

Bezier::Bezier()
{
}

bool Bezier::push(glm::vec3 point)
{
	int size = vertices.size();
	if (size == 0) {
		vertices.push_back(point);
		return true;
	}

	if (vertices[size - 1] == point) {
		return false;
	}
	vertices.push_back(point);
	return true;
}

bool Bezier::pop()
{
	if (vertices.size() == 0) {
		return false;
	}

	vertices.pop_back();
	return true;
}

std::vector<glm::vec3> Bezier::linearCombination(std::vector<glm::vec3> points, float t)
{
	std::vector<glm::vec3> res;
	for (int i = 0; i < points.size() - 1; ++i) {
		res.push_back(points[i] * t + points[i + 1] * (1 - t));
	}
	return res;
}

std::vector<glm::vec3> Bezier::bezierCurve(int num)
{
	int n = vertices.size() - 1;
	std::vector<glm::vec3> res;
	if (n < 2) {
		return res;
	}
	
	for (int i = 0; i <= num; ++i) {
		float t = (float)i / num;
		glm::vec3 p(0.0, 0.0, 0.0);
		for (int j = 0; j <= n; ++j) {
			p += vertices[j] * B(n, j, t);
	
		}
		res.push_back(p);
	}

	return res;
}

std::vector<glm::vec3> Bezier::getVertices()
{
	return vertices;
}

int Bezier::C(int n, int i)
{
	if (i > n)
		return 0;
	else if (i == n || i == 0)
		return 1;
	else
		return C(n - 1, i) + C(n - 1, i - 1);
}

float Bezier::B(int n, int i, float t) {
	return C(n, i) * pow(t, i) * pow(1 - t, n - i);
}
