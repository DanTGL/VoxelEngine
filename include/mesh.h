#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <tuple>
#include "shader.h"

typedef glm::tvec3<GLbyte> ChunkBlockPos_t;

struct Vertex {
	ChunkBlockPos_t Position;
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

	unsigned int AddVertex(ChunkBlockPos_t pos, glm::vec2 texcoord, float layer);
	void AddTriangle(unsigned int vertex1, unsigned int vertex2, unsigned int vertex3);
	void setupMesh();
	void GetMeshInformation(size_t &numVerts, size_t &numTriangles);
private:
	unsigned int VAO, VBO, EBO;
};