#include "camera.h"
#include <gtc\matrix_transform.hpp>

Camera::Camera() {

}

Camera::~Camera() {

}

void Camera::Render() {
	m_viewMatrix = glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);
}

void Camera::GetViewMatrix(glm::mat4 &viewMatrix) {
	viewMatrix = m_viewMatrix;
}

void Camera::SetPosition(glm::vec3 pos) {
	this->m_Pos = pos;
}

void Camera::SetPosition(float x, float y, float z) {
	this->m_Pos.x = x;
	this->m_Pos.y = y;
	this->m_Pos.z = z;
}

void Camera::SetRotation(glm::vec3 rot) {
	this->m_Up = rot;
}

void Camera::SetRotation(float rotX, float rotY, float rotZ) {
	this->m_Up.x = rotX;
	this->m_Up.y = rotY;
	this->m_Up.z = rotZ;
}
