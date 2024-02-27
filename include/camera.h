#pragma once

#include <glm/glm.hpp>

class Camera {
private:
	glm::vec3 m_Up;
	glm::vec3 m_Front;
	glm::vec3 m_Pos;
	glm::mat4 m_viewMatrix;
public:
	Camera();
	~Camera();
	void GetViewMatrix(glm::mat4 &viewMatrix);
	void Render();
	void SetPosition(glm::vec3 pos);
	void SetPosition(float x, float y, float z);
	void SetRotation(glm::vec3 rot);
	void SetRotation(float rotX, float rotY, float rotZ);
};