#include "SGMemoryManager.h"



uint16 SGPoolAllocator::MemSizeToIndex[SG_MEM_BUCKET_COUNT] = { 0 };

uint16 SGPoolAllocator::SmallBlockSizes[SG_MEM_POOL_COUNT] = {
	16, 32, 48, 64, 80, 96, 128,160,
	192, 224, 256, 288, 320, 384, 448, 512,
	576, 640, 704, 768, 896, 1024 - 16, 1168, 1488,
	1632, 2048 - 16
};


SGPoolAllocator::SGPoolAllocator(SGMemoryChunk* InChunk)
{
	AllocatedMemory = 0;
	MemoryChunk = InChunk;
	uint32 PoolIndex = 0;
	for (uint32 i = 0; i != SG_MEM_BUCKET_COUNT; ++i)
	{
		uint32 BlockSize = i << 4;
		while (SmallBlockSizes[PoolIndex] < BlockSize) ++PoolIndex;
		MemSizeToIndex[i] = uint8(PoolIndex);
	}

}

SGPoolAllocator::~SGPoolAllocator()
{
	MemoryChunk = nullptr;
}


void* SGPoolAllocator::MallocExternal(size_t Size, uint32 Alignment)
{
	uint32 PoolIndex = BoundSizeToPoolIndex(Size);
	uint32 BlockSize = PoolIndexToBlockSize(PoolIndex);
	void* Result = nullptr;

	//关键：分配一个新的Pool
	SGFreeBlock* FirstFreeBlock = AllocateNewPoolWithFirstFreeBlock(BlockSize, PoolIndex);
	if (FirstFreeBlock)
	{
		Result = FirstFreeBlock->AllocateRegularBlock();

		//将一个新的Pool预切割为Block，且加入到FreeLists中去
		for (int32 i = 0; FirstFreeBlock->GetNumFreeRegularBlocks() != 0; i++)
		{
			if (!FreeLists[PoolIndex].PushToFront(Result))  break;
			Result = FirstFreeBlock->AllocateRegularBlock();
		}

		//然后顺便分配一个出去
		AllocatedMemory += BlockSize;
	}
	else
	{
		//错误处理
	}

	return Result;
}

SGFreeBlock* SGPoolAllocator::AllocateNewPoolWithFirstFreeBlock(uint32 InBlockSize, uint32 InPoolIndex)
{
	void* Ptr = MemoryChunk->AllocatePage();
	if (Ptr)
	{
		SGFreeBlock* FirstFreeBlock = new (Ptr) SGFreeBlock(SG_MEM_POOL_SIZE, InBlockSize, InPoolIndex);
		return FirstFreeBlock;
	}
	return nullptr;
}


SGMemorySnapshot SGMemoryManager::MakeSnapshot()
{
	SGMemorySnapshot Result;
	Result.Size = MemoryChunk->GetUsedSize();
	Result.Buffer = ::_aligned_malloc(Result.Size, SG_MEM_ALIGNMENT);
	memset(Result.Buffer, 0, Result.Size);
	memcpy(Result.Buffer, MemoryChunk->GetBasePtr(), Result.Size);
	Result.BaseAddr = (uint64)MemoryChunk->GetBasePtr();
	return Result;
}
