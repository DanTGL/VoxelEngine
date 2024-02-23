#include "block.h"

Block::Block() {
	this->m_active = false;
}

Block::~Block() {

}

void Block::SetActive(bool active) {
	this->m_active = active;
}

void Block::OnBlockAdded(BlockPos pos, BlockType m_Type) {
}

void Block::Update(BlockPos pos) {
}

void Block::OnBlockDestroyed(BlockPos pos) {
}

void Block::OnNeighborChanged(BlockPos pos, BlockPos neighborPos) {
}

void Block::NotifyNeighborChange(BlockPos pos) {
}

bool Block::IsActive() {
	return this->m_active;
}

void GrassBlock::Update(BlockPos pos) {
}