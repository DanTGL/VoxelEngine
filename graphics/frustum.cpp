#include "frustum.h"

#define ANG2RAD 3.14159265358979323846/180.0

void FrustumG::setCamInternals(float angle, float ratio, float nearD, float farD) {
	// Store the information
	this->ratio = ratio;
	this->angle = angle;
	this->nearD = nearD;
	this->farD = farD;

	// Compute width and height of the near and far plane sections
	tang = (float)tan(ANG2RAD * angle * 0.5);
	nh = nearD * tang;
	nw = nh * ratio;
	fh = farD * tang;
	fw = fh * ratio;
}

void FrustumG::setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u) {
	glm::vec3 dir, nc, fc, X, Y, Z;

	// Compute the Z axis of camera,
	// this axis points in the opposite direction from
	// the looking direction
	Z = p - l;
	Z = glm::normalize(Z);

	// X axis of camera with given "up" vector and Z axis
	X = u * Z;
	X = glm::normalize(X);

	// The real "up" vector is the cross product of Z and X
	Y = Z * X;

	// Compute the centers of the near and far planes
	nc = p - Z * nearD;
	fc = p - Z * farD;

	pl[NEARP].setNormalAndPoint(-Z, nc);
	pl[FARP].setNormalAndPoint(Z, fc);

	glm::vec3 aux, normal;

	aux = (nc + Y * nh) - p;
	aux = glm::normalize(aux);
	normal = aux * X;
	pl[TOP].setNormalAndPoint(normal, nc + Y * nh);

	aux = (nc - Y * nh) - p;
	aux = glm::normalize(aux);
	normal = X * aux;
	pl[BOTTOM].setNormalAndPoint(normal, nc - Y * nh);

	aux = (nc - X * nw) - p;
	aux = glm::normalize(aux);
	normal = aux * Y;
	pl[LEFT].setNormalAndPoint(normal, nc - X * nw);

	aux = (nc + X * nw) - p;
	aux = glm::normalize(aux);
	normal = Y * aux;
	pl[RIGHT].setNormalAndPoint(normal, nc + X * nw);
}

int FrustumG::pointInFrustum(glm::vec3 &point) {
	int result = INSIDE;

	for (int i = 0; i < 6; i++) {
		if (pl[i].Distance(point) < 0) {
			return OUTSIDE;
		}
	}

	return (result);
}

int FrustumG::sphereInFrustum(glm::vec3 &point, float radius) {
	int result = INSIDE;
	float distance;

	for (int i = 0; i < 6; i++) {
		distance = pl[i].Distance(point);

		if (distance < -radius) {
			return OUTSIDE;
		} else if (distance < radius) {
			result = INTERSECT;
		}
	}

	return (result);
}

int FrustumG::CubeInFrustum(glm::vec3 &center, float x, float y, float z) {
	int result = INSIDE;

	for (int i = 0; i < 6; i++) {
		// Reset counters for corners in and out
		int out = 0;
		int in = 0;

		if (pl[i].Distance(center + glm::vec3(-x, -y, -z)) < 0) {
			out++;
		} else {
			in++;
		}

		if (pl[i].Distance(center + glm::vec3(x, -y, -z)) < 0) {
			out++;
		} else {
			in++;
		}

		if (pl[i].Distance(center + glm::vec3(-x, -y, z)) < 0)
		{
			out++;
		} else
		{
			in++;
		}

		if (pl[i].Distance(center + glm::vec3(x, -y, z)) < 0)
		{
			out++;
		} else
		{
			in++;
		}

		if (pl[i].Distance(center + glm::vec3(-x, y, -z)) < 0)
		{
			out++;
		} else
		{
			in++;
		}

		if (pl[i].Distance(center + glm::vec3(x, y, -z)) < 0)
		{
			out++;
		} else
		{
			in++;
		}

		if (pl[i].Distance(center + glm::vec3(-x, y, z)) < 0)
		{
			out++;
		} else
		{
			in++;
		}

		if (pl[i].Distance(center + glm::vec3(x, y, z)) < 0)
		{
			out++;
		} else
		{
			in++;
		}

		// If all corners are out
		if (!in)
		{
			return OUTSIDE;
		}
		// If some corners are out and others are in	
		else if (out)
		{
			result = INTERSECT;
		}
	}

	return (result);
}