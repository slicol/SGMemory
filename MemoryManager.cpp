#include "MemoryManager.h"

uint8 SGMemoryManager::MemSizeToIndex[SG_MEM_BUCKET_COUNT] = { 0 };
uint16 SGMemoryManager::SmallBlockSizesReversed[SG_MEM_POOL_COUNT]  = {0};

static constexpr uint8 SmallBlockSizes[SG_MEM_BUCKET_COUNT] = {
	16, 32, 48, 64, 80, 96, 128,160,
	192, 224, 256, 288, 320, 384, 448, 512,
	576, 640, 704, 768, 896, 1024 - 16, 1168, 1488,
	1632, 2048 - 16
};

SGMemoryManager::SGMemoryManager()
{
	uint32 PoolIndex = 0;
	for (uint32 i = 0; i != SG_MEM_BUCKET_COUNT; ++i)
	{
		uint32 BlockSize = i << 4;
		while (SmallBlockSize[PoolIndex] < BlockSize) ++PoolIndex;
		MemSizeToIndex[i] = uint8(PoolIndex);
	}

	for (uint32 i = 0; i != SG_MEM_POOL_COUNT; ++i)
	{
		SmallBlockSizesReversed[i] = SmallBlockSizes[SG_MEM_POOL_COUNT - i - 1];
	}
}