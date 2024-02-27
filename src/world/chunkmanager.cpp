#include "chunkmanager.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

using namespace noise;

#define ASYNC_NUM_CHUNKS_PER_FRAME 6

ChunkManager::ChunkManager(FrustumG* frustum) {
	m_Frustum = frustum;

	/*m_noiseModule.SetSeed(4);
	m_noiseModule.SetOctaveCount(2);
	m_noiseModule.SetFrequency(1.0);
	m_noiseModule.SetPersistence(0.75);
	*/
	m_noiseModule.SetPersistence(0.75);
	m_noiseModule.SetFrequency(1.0);

	Random* rnd = new Random(10);

	noise1 = new ImprovedCombinedNoise(new ImprovedOctaveNoise(8, rnd), new ImprovedOctaveNoise(8, rnd));
	noise2 = new ImprovedCombinedNoise(new ImprovedOctaveNoise(8, rnd), new ImprovedOctaveNoise(8, rnd));
	noise3 = new ImprovedOctaveNoise(6, rnd);
}

ChunkManager::~ChunkManager() {
	delete m_Frustum;
	m_Frustum = 0;
}

void ChunkManager::Update(float dt, glm::vec3 cameraPosition, glm::vec3 cameraView) {
	UpdateAsyncChunker();

	UpdateLoadList();

	UpdateSetupList();

	UpdateRebuildList();

	UpdateFlagsList();

	UpdateUnloadList();

	UpdateVisibilityList(cameraPosition);

	if (m_cameraPosition != cameraPosition || m_cameraView != cameraView) {
		UpdateRenderList();
	}

	m_cameraPosition = cameraPosition;
	m_cameraView = cameraView;
}

void ChunkManager::UpdateAsyncChunker() {
	int lNumOfChunksAsynced = 0;

	std::vector<Chunk*>::iterator iterator;
	for (iterator = m_vpChunkAsyncList.begin(); iterator != m_vpChunkAsyncList.end() && (lNumOfChunksAsynced != ASYNC_NUM_CHUNKS_PER_FRAME);) {
		Chunk* pChunk = (*iterator);

		if (lNumOfChunksAsynced != ASYNC_NUM_CHUNKS_PER_FRAME) {
			if (pChunk->IsLoaded() == false) {
				m_vpChunkLoadList.push_back(pChunk);
			}
			lNumOfChunksAsynced++;
			m_vpChunkRebuildList.push_back(pChunk);
			iterator = m_vpChunkAsyncList.erase(iterator);
		}
	}
}

void ChunkManager::UpdateLoadList() {
	int lNumOfChunksLoaded = 0;

	std::vector<Chunk*>::iterator iterator;
	for (iterator = m_vpChunkLoadList.begin(); iterator != m_vpChunkLoadList.end() && (lNumOfChunksLoaded != ASYNC_NUM_CHUNKS_PER_FRAME); ++iterator) {
		Chunk* pChunk = (*iterator);

		if (pChunk->IsLoaded() ==  false) {
			if (lNumOfChunksLoaded != ASYNC_NUM_CHUNKS_PER_FRAME) {
				pChunk->Load();

				// Increase the chunks loaded count
				lNumOfChunksLoaded++;

				m_forceVisibilityUpdate = true;
			}
		}
	}

	// Clear the load list (every frame)
	m_vpChunkLoadList.clear();
}

void ChunkManager::UpdateSetupList() {
	// Setup any chunks that have not already been setup
	std::vector<Chunk*>::iterator iterator;

	for (iterator = m_vpChunkSetupList.begin(); iterator != m_vpChunkSetupList.end(); ++iterator) {
		Chunk* pChunk = (*iterator);

		if (pChunk->IsLoaded() && pChunk->IsSetup() == false) {
			/*utils::NoiseMapBuilderPlane heightMapBuilder;
			heightMapBuilder.SetSourceModule(m_noiseModule);
			heightMapBuilder.SetDestNoiseMap(m_heightMap);
			heightMapBuilder.SetDestSize(256, 256);
			heightMapBuilder.SetBounds(pChunk->GetX() * Chunk::CHUNK_SIZE, (pChunk->GetX() * Chunk::CHUNK_SIZE) + Chunk::CHUNK_SIZE - 1, pChunk->GetZ() * Chunk::CHUNK_SIZE, (pChunk->GetZ() * Chunk::CHUNK_SIZE) + Chunk::CHUNK_SIZE - 1);
			heightMapBuilder.Build();*/

			pChunk->Setup();
			//pChunk->SetupLandscape(noise1, noise2, noise3);


			if (pChunk->IsSetup()) {
				// Only force the visibility update if we actually setup the chunk, some chunks wait in the pre-setup stage...
				m_forceVisibilityUpdate = true;
			}
		}
	}

	// Clear the setup list (every frame)
	m_vpChunkSetupList.clear();
}

void ChunkManager::UpdateRebuildList() {
	// Rebuild any chunks that are in the rebuild chunk list
	std::vector<Chunk*>::iterator iterator;
	int lNumRebuiltChunkThisFrame = 0;

	for (iterator = m_vpChunkRebuildList.begin(); iterator != m_vpChunkRebuildList.end() && (lNumRebuiltChunkThisFrame != ASYNC_NUM_CHUNKS_PER_FRAME); ++iterator) {
		Chunk* pChunk = (*iterator);

		if (pChunk->IsLoaded() && pChunk->IsSetup()) {
			if (lNumRebuiltChunkThisFrame != ASYNC_NUM_CHUNKS_PER_FRAME) {
				pChunk->CreateMesh();

				// If we rebuild a chunk, add it to the list of chunks that need their render flags updated
				// since we might now be empty or surrounded.
				m_vpChunkUpdateFlagsList.push_back(pChunk);

				// Also add our neighbours since they might now be surrounded too (If we have neighbours)
				Chunk* pChunkXMinus = GetChunk(pChunk->GetX() - 1, pChunk->GetZ());
				Chunk* pChunkXPlus = GetChunk(pChunk->GetX() + 1, pChunk->GetZ());
				//Chunk* pChunkYMinus = GetChunk(pChunk->GetX(), pChunk->GetZ());
				//Chunk* pChunkYPlus = GetChunk(pChunk->GetX(), pChunk->GetZ());
				Chunk* pChunkZMinus = GetChunk(pChunk->GetX(), pChunk->GetZ() - 1);
				Chunk* pChunkZPlus = GetChunk(pChunk->GetX(), pChunk->GetZ() + 1);

				if (pChunkXMinus != NULL)
					m_vpChunkUpdateFlagsList.push_back(pChunkXMinus);

				if (pChunkXPlus != NULL)
					m_vpChunkUpdateFlagsList.push_back(pChunkXPlus);
				/*
				if (pChunkYMinus != NULL)
					m_vpChunkUpdateFlagsList.push_back(pChunkYMinus);

				if (pChunkYPlus != NULL)
					m_vpChunkUpdateFlagsList.push_back(pChunkYPlus);*/

				if (pChunkZMinus != NULL)
					m_vpChunkUpdateFlagsList.push_back(pChunkZMinus);

				if (pChunkZPlus != NULL)
					m_vpChunkUpdateFlagsList.push_back(pChunkZPlus);

				// Only rebuild a certain number of chunks per frame
				lNumRebuiltChunkThisFrame++;

				m_forceVisibilityUpdate = true;
			}
		}
	}

	// Clear the rebuild list
	m_vpChunkRebuildList.clear();
}

void ChunkManager::UpdateFlagsList() {
	std::vector<Chunk*>::iterator iterator;

	for (iterator = m_vpChunkUpdateFlagsList.begin(); iterator != m_vpChunkUpdateFlagsList.end(); ++iterator) {
		Chunk* pChunk = (*iterator);

		pChunk->UpdateShouldRenderFlags();

	}

	m_vpChunkUpdateFlagsList.clear();
}

void ChunkManager::UpdateUnloadList() {

	// Unload any chunks
	std::vector<Chunk*>::iterator iterator;

	for (iterator = m_vpChunkUnloadList.begin(); iterator != m_vpChunkUnloadList.end(); ++iterator) {
		Chunk* pChunk = (*iterator);

		if (pChunk->IsLoaded()) {
			pChunk->Unload();

			m_forceVisibilityUpdate = true;
		}
	}

	// Clear the unload list (every frame)
	m_vpChunkUnloadList.clear();
}

//Chunk* lastChunk;
Chunk* lastChunk;

void ChunkManager::UpdateVisibilityList(glm::vec3 cameraPosition) {
	Chunk* chunk;
	
	int gridX = (int)((cameraPosition.x + BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridY = (int)((cameraPosition.y + BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridZ = (int)((cameraPosition.z + BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);

	//printf("cx: %d, cy: %d\n", gridX, gridZ);

	chunk = GetChunk(gridX, gridZ);

	if (m_forceVisibilityUpdate || m_chunkMap.empty() || chunk != lastChunk) {
		

		lastChunk = chunk;

		//std::copy(m_vpChunkVisibilityList.begin(), m_vpChunkVisibilityList.end(), std::back_inserter(m_vpChunkUnloadList));

		m_vpChunkVisibilityList.clear();

		if (cameraPosition.x <= -0.5f)
			gridX -= 1;
		if (cameraPosition.y <= -0.5f)
			gridY -= 1;
		if (cameraPosition.z <= -0.5f)
			gridZ -= 1;

		UpdateVisibility(gridX, gridZ);
		UpdateVisibility(gridX - 1, gridZ);
		UpdateVisibility(gridX + 1, gridZ);
		UpdateVisibility(gridX, gridZ - 1);
		UpdateVisibility(gridX, gridZ + 1);

		Chunk* curChunk = GetChunk(gridX, gridZ);
		Chunk* pChunkXMinus = GetChunk(gridX - 1, gridZ);
		Chunk* pChunkXPlus = GetChunk(gridX + 1, gridZ);
		//Chunk* pChunkYMinus = GetChunk(gridX, gridY - 1, gridZ);
		//Chunk* pChunkYPlus = GetChunk(gridX, gridY + 1, gridZ);
		Chunk* pChunkZMinus = GetChunk(gridX, gridZ - 1);
		Chunk* pChunkZPlus = GetChunk(gridX, gridZ + 1);

		/*std::vector<Chunk*>::iterator iterator;
		for (iterator = m_vpChunkUnloadList.begin(); iterator != m_vpChunkUnloadList.end();) {
			Chunk* pChunk = (*iterator);

			if (pChunk == curChunk || pChunk == pChunkXMinus || pChunk == pChunkXPlus || pChunk == pChunkZMinus || pChunk == pChunkZPlus) {
				iterator = m_vpChunkUnloadList.erase(iterator);
				continue;
			} 

			iterator++;
		}*/

		m_forceVisibilityUpdate = false;
	}
}

void ChunkManager::UpdateVisibility(int gridX, int gridZ) {
	if (GetChunk(gridX, gridZ) == NULL) {
		CreateChunk(gridX, gridZ);
	}

	Chunk* pChunk = GetChunk(gridX, gridZ);

	if (!pChunk->IsLoaded()) {
		m_vpChunkAsyncList.push_back(pChunk);
	}

	if (!pChunk->IsSetup()) {
		m_vpChunkSetupList.push_back(pChunk);
	}

	m_vpChunkVisibilityList.push_back(pChunk);


}

void ChunkManager::UpdateRenderList() {
	// Clear the render list each frame BEFORE we do our tests to see what chunks should be rendered
	m_vpChunkRenderList.clear();

	std::vector<Chunk*>::iterator iterator;

	for (iterator = m_vpChunkVisibilityList.begin(); iterator != m_vpChunkVisibilityList.end(); ++iterator) {
		Chunk* pChunk = (*iterator);
		if (pChunk != NULL) {
			if (pChunk->IsLoaded() && pChunk->IsSetup()) {
				if (pChunk->ShouldRender()) {	// Early flags check so we don't always have to do the frustum check...
					// Check if this chunk is inside the camera frustum
				//printf("n");
					float c_offset = (Chunk::CHUNK_SIZE * BLOCK_RENDER_SIZE) - BLOCK_RENDER_SIZE;
					glm::vec2 chunkCenter = pChunk->GetPosition() + glm::vec2(c_offset, c_offset);
					float c_heightoffset = (Chunk::WORLD_HEIGHT * BLOCK_RENDER_SIZE) - BLOCK_RENDER_SIZE;
					
					float c_size = Chunk::CHUNK_SIZE * BLOCK_RENDER_SIZE;
					float c_height = Chunk::WORLD_HEIGHT * BLOCK_RENDER_SIZE;
					glm::vec3 center(chunkCenter.x, c_heightoffset, chunkCenter.y);
					//if (m_Frustum->CubeInFrustum(center, c_size, c_height, c_size)) {
						m_vpChunkRenderList.push_back(pChunk);
					//}
				}
			}
		}
	}
}


void ChunkManager::Render(Shader shader) {
	// Render any chunks in the render list
	std::vector<Chunk*>::iterator iterator;

	for (iterator = m_vpChunkRenderList.begin(); iterator != m_vpChunkRenderList.end(); ++iterator) {
		Chunk* pChunk = (*iterator);
		if (pChunk->IsLoaded() && pChunk->IsSetup()) {
			if (pChunk->ShouldRender()) { // Early flags check so we don't always have to do the frustum check...
				pChunk->Render(shader);
			}
		}
	}
}

void ChunkManager::CreateChunk(int x, int z) {
	ChunkCoord coord;
	coord.x = x;
	coord.z = z;

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(m_noiseModule);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(Chunk::CHUNK_SIZE, Chunk::CHUNK_SIZE);
	heightMapBuilder.SetBounds(x * Chunk::CHUNK_SIZE, (x * Chunk::CHUNK_SIZE) + Chunk::CHUNK_SIZE - 1, z * Chunk::CHUNK_SIZE, (z * Chunk::CHUNK_SIZE) + Chunk::CHUNK_SIZE - 1);
	heightMapBuilder.Build();

	Chunk* pNewChunk = new Chunk(this);

	float xPos = x * (Chunk::CHUNK_SIZE * BLOCK_RENDER_SIZE*2.0f);
	float zPos = z * (Chunk::CHUNK_SIZE * BLOCK_RENDER_SIZE*2.0f);

	pNewChunk->SetPosition(glm::vec2(xPos, zPos));
	pNewChunk->SetGrid(coord.x, coord.z);
	pNewChunk->SetupLandscape(noise1, noise2, noise3);

	m_chunkMap[coord] = pNewChunk;


}

Chunk* ChunkManager::GetChunk(int x, int z) {
	ChunkCoord coord;
	coord.x = x;
	coord.z = z;

	std::map<ChunkCoord, Chunk*>::iterator iterator = m_chunkMap.find(coord);
	if (iterator != m_chunkMap.end()) {
		Chunk* lpReturn = m_chunkMap[coord];
		return lpReturn;
	}

	return NULL;
}

Chunk* ChunkManager::GetChunkFromPosition(float posX, float posZ) {
	int gridX = (int)((posX + BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridZ = (int)((posZ + BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);

	if (posX <= -0.5f)
		gridX -= 1;
	if (posZ <= -0.5f)
		gridZ -= 1;

	return GetChunk(gridX, gridZ);
}

utils::NoiseMap ChunkManager::GetHeightMap() {
	return m_heightMap;
}

Chunk* ChunkManager::LoadChunk(glm::ivec2 pos) {
	glm::ivec2 chunkPos = pos;
	pos *= Chunk::CHUNK_SIZE;

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(m_noiseModule);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(Chunk::CHUNK_SIZE, Chunk::CHUNK_SIZE);
	heightMapBuilder.SetBounds(pos.x, pos.x + Chunk::CHUNK_SIZE - 1, pos.y, pos.y + Chunk::CHUNK_SIZE - 1);
	heightMapBuilder.Build();

	Chunk* chunk = new Chunk(this);


	return chunk;
}