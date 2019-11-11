#pragma once

#include "glm.hpp"

#include "textures.h"

#define FACE_TOP 0
#define FACE_BOTTOM 1
#define FACE_FRONT 2
#define FACE_BACK 3
#define FACE_LEFT 4
#define FACE_RIGHT 5

struct BlockPos {
	int x, y, z;
};

enum BlockType {
	BlockType_Default = 0,

	BlockType_Grass,
	BlockType_Dirt,
	BlockType_Water,
	BlockType_Stone,
	BlockType_Wood,
	BlockType_Sand,
	BlockType_Gravel,

	BlockType_NumTypes
};

static const float textures[BlockType_NumTypes][6] = {
	{getLayer(256u, 200u)},
	{ getLayer(256u, 240u), getLayer(256u, 242u), getLayer(256u, 243u), getLayer(256u, 243u), getLayer(256u, 243u), getLayer(256u, 243u) },
	{ getLayer(256u, 242u), getLayer(256u, 242u), getLayer(256u, 242u), getLayer(256u, 242u), getLayer(256u, 242u), getLayer(256u, 242u) },
	{ getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u) },
	{ getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u) },
	{ getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u) },
	{ getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u) },
	{ getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u), getLayer(256u, 240u) },
};

static const float BLOCK_RENDER_SIZE = 0.5f;
class Block {
public:
	Block();
	~Block();

	bool IsActive();
	void SetActive(bool active);

	BlockType m_blockType;
	void OnBlockAdded(BlockPos pos, BlockType m_Type);
	virtual void Update(BlockPos pos);
	void OnBlockDestroyed(BlockPos pos);
	void OnNeighborChanged(BlockPos pos, BlockPos neighborPos);
	void NotifyNeighborChange(BlockPos pos);
private:
	bool m_active;
	bool updatable = false;
};

class GrassBlock : public Block {
	void Update(BlockPos pos); 
};

struct BlockInstance {
	BlockPos pos;
	Block* block;
};