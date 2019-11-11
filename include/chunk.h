#pragma once

#include "mesh.h"
#include "block.h"
#include "noiseutils.h"
#include "noise.h"

class ChunkManager;

struct block {
	BlockType type;
	bool active = false;
};

typedef struct {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
} MeshInfo;

class Chunk {
public:
	Chunk(ChunkManager* manager);
	~Chunk();

	void Update(float dt);
	void Render(Shader shader);

	void CreateMesh();
	void CreateCube(float x, float y, float z, bool xNegative, bool xPositive, bool yNegative, bool yPositive, bool zNegative, bool zPositive);

	void SetupSphere();
	void SetupLandscape(ImprovedCombinedNoise* noise1, ImprovedCombinedNoise* noise2, ImprovedOctaveNoise* noise3);

	void UpdateShouldRenderFlags();
	bool ShouldRender();

	void Load();
	void Unload();
	void Setup();
	
	void Update();

	bool IsLoaded();
	bool IsSetup();

	void SetPosition(glm::vec2 pos);
	void SetGrid(int gridX, int gridZ);

	int GetX();
	int GetZ();

	glm::vec2 GetPosition();

	static const int CHUNK_SIZE = 16;
	static const int WORLD_HEIGHT = 256;
	Mesh mesh;
private:
	Block*** m_pBlocks;
	bool m_emptyChunk;
	bool m_loaded;
	bool m_setup;
	glm::vec2 gridPos, worldPos;
	BlockInstance* blocks;
	ChunkManager* m_pManager;
};

