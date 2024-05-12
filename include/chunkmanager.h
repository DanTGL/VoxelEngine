#pragma once

#include "chunk.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include "frustum.h"
#include "noise.h"
#include "noiseutils.h"

#include <glm/gtx/hash.hpp>

typedef glm::ivec3 ChunkCoord;

const size_t NUM_CHUNKS_Y = 32;

/*inline bool const operator==(const ChunkCoord_t& l, const ChunkCoord_t& r) {
	return l.x == r.x && l.y == r.y && l.z == r.z;
};

inline bool const operator<(const ChunkCoord_t& l, const ChunkCoord_t& r) {
	if (l.x < r.x)  return true;
	if (l.x > r.x)  return false;

	if (l.y < r.y)	return true;
	if (l.y > r.y)	return false;

	if (l.z < r.z)  return true;
	if (l.z > r.z)  return false;

	return false;
};*/

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
	std::unordered_map<ChunkCoord, Chunk*> m_chunkMap;
	ChunkCoord m_lastCoord;
	
	noise::module::Perlin m_noiseModule;
	noise::module::Perlin m_noiseModule2;
	noise::module::Perlin m_noiseModule3;
	utils::NoiseMap m_heightMap;

	void UpdateVisibility(ChunkCoord coord);

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
	Chunk* LoadChunk(ChunkCoord coord);

	void CreateChunk(ChunkCoord coord);
	Chunk* GetChunk(ChunkCoord coord);
	Chunk* GetChunk(int x, int y, int z) {
		return GetChunk(ChunkCoord(x, y, z));
	}
	Chunk* GetChunkFromPosition(glm::vec3 pos);
	//double GetNoiseValue(int x, int z);
	utils::NoiseMap GetHeightMap();
};