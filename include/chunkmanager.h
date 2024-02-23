#pragma once

#include "chunk.h"
#include <glm/glm.hpp>
#include <map>
#include "frustum.h"
#include "noise.h"
#include "noiseutils.h"

struct ChunkCoord {
	int x, z;
};

inline bool const operator==(const ChunkCoord& l, const ChunkCoord& r) {
	return l.x == r.x && l.z == r.z;
};

inline bool const operator<(const ChunkCoord& l, const ChunkCoord& r) {
	if (l.x < r.x)  return true;
	if (l.x > r.x)  return false;

	if (l.z < r.z)  return true;
	if (l.z > r.z)  return false;

	return false;
};

class ChunkManager {
private:
	std::vector<Chunk*> m_vpChunkAsyncList;
	std::vector<Chunk*> m_vpChunkLoadList;
	std::vector<Chunk*> m_vpChunkSetupList;
	std::vector<Chunk*> m_vpChunkRebuildList;
	std::vector<Chunk*> m_vpChunkUnloadList;
	std::vector<Chunk*> m_vpChunkRenderList;
	std::vector<Chunk*> m_vpChunkVisibilityList;
	std::vector<Chunk*> m_vpChunkUpdateFlagsList;

	glm::vec3 m_cameraPosition;
	glm::vec3 m_cameraView;
	bool m_forceVisibilityUpdate;
	FrustumG* m_Frustum;
	std::map<struct ChunkCoord, Chunk*> m_chunkMap;
	
	noise::module::Perlin m_noiseModule;
	noise::module::Perlin m_noiseModule2;
	noise::module::Perlin m_noiseModule3;
	utils::NoiseMap m_heightMap;

	void UpdateVisibility(int gridX, int gridZ);

	ImprovedCombinedNoise* noise1;
	ImprovedCombinedNoise* noise2;
	ImprovedOctaveNoise* noise3;
public:
	ChunkManager(FrustumG* frustum);
	~ChunkManager();
	void Update(float dt, glm::vec3 cameraPosition, glm::vec3 cameraView);
	void UpdateAsyncChunker();
	void UpdateLoadList();
	void UpdateSetupList();
	void UpdateRebuildList();
	void UpdateFlagsList();
	void UpdateUnloadList();
	void UpdateVisibilityList(glm::vec3 cameraPosition);
	void UpdateRenderList();
	void Render(Shader shader);
	Chunk* LoadChunk(glm::ivec2 pos);

	void CreateChunk(int x, int z);
	Chunk* GetChunk(int x, int z);
	Chunk* GetChunkFromPosition(float posX, float posZ);
	//double GetNoiseValue(int x, int z);
	utils::NoiseMap GetHeightMap();
};