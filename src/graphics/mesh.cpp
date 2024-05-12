#include "mesh.h"

#include <GL\glew.h>

#include "textures.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;

	setupMesh();
}

void Mesh::Render(Shader shader) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	shader.setInt("textureArray", textures[0].id);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textures[0].id);

	glBindVertexArray(VAO);

	if (!indices.empty()) {
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glBindVertexArray(0);
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
				 &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, sizeof(ChunkBlockPos_t), GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Layer));

	glBindVertexArray(0);
}

void Mesh::GetMeshInformation(size_t &numVerts, size_t &numTriangles) {
	numVerts = vertices.size();
	numTriangles = indices.size() / 3;
}

unsigned int Mesh::AddVertex(ChunkBlockPos_t pos, glm::vec2 texcoord, float layer) {
	Vertex vertex;
	vertex.Position = pos;
	vertex.TexCoords = texcoord;
	vertex.Layer = layer;
	vertices.push_back(vertex);
	return vertices.size() - 1;
}

void Mesh::AddTriangle(unsigned int vertex1, unsigned int vertex2, unsigned int vertex3) {
	indices.push_back(vertex1);
	indices.push_back(vertex2);
	indices.push_back(vertex3);
}

void AddTexture(std::string imagepath) {
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

}