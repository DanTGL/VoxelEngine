#pragma once

#include <glm.hpp>

class Plane {
public:
	float A, B, C, D;
	void set3Points(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
	float Distance(glm::vec3 point);
	void setNormalAndPoint(glm::vec3 normal, glm::vec3 point);
};