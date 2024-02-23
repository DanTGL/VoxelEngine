#include "plane.h"

void Plane::set3Points(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
	glm::vec3 v = p1 - p0;
	glm::vec3 u = p2 - p0;

	glm::vec3 n = glm::cross(v, u);
	n = glm::normalize(n);
	A = n.x;
	B = n.y;
	C = n.z;
	D = -glm::dot(n, p0);
}

void Plane::setNormalAndPoint(glm::vec3 normal, glm::vec3 point) {
	normal = glm::normalize(normal);
	A = normal.x;
	B = normal.y;
	C = normal.z;
	D = -glm::dot(normal, point);
}

float Plane::Distance(glm::vec3 point) {
	return A * point.x + B * point.y + C * point.z + D;
}