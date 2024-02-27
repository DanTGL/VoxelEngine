#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <tuple>
#include "shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	float Layer;
};

struct Texture {
	unsigned int id;
	std::string type;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh() {}
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Render(Shader shader);

	unsigned int AddVertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texcoord, float layer);
	void AddTriangle(unsigned int vertex1, unsigned int vertex2, unsigned int vertex3);
	void setupMesh();
	void GetMeshInformation(size_t &numVerts, size_t &numTriangles);
private:
	unsigned int VAO, VBO, EBO;
};