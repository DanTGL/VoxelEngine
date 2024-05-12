#pragma once

#include "mesh.h"
#include "block.h"
#include "noiseutils.h"
#include "noise.h"

#include <glm/glm.hpp>

class ChunkManager;

class Chunk {
public:
	Chunk(ChunkManager* manager);
	~Chunk();

	void Update(float dt);
	void Render(Shader shader);

	void CreateMesh();
	void CreateCube(uint8_t x, uint8_t y, uint8_t z, bool xNegative, bool xPositive, bool yNegative, bool yPositive, bool zNegative, bool zPositive);

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

	void SetPosition(glm::vec3 pos);
	void SetGrid(glm::ivec3 coord);

	int GetX();
	int GetY();
	int GetZ();

	glm::vec3 GetPosition();

	static const int CHUNK_SIZE = 16;
	static const int WORLD_HEIGHT = 256;
	Mesh mesh;
private:
	Block*** m_pBlocks;
	bool m_emptyChunk;
	bool m_loaded;
	bool m_setup;
	glm::ivec3 gridPos;
	glm::vec3 worldPos;
	BlockInstance* blocks;
	ChunkManager* m_pManager;
};

