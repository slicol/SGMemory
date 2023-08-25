#include "SGMemoryManager.h"


SGMemorySnapshot SGMemoryManager::MakeSnapshot() const
{
	SGMemorySnapshot Result;
	
	//收集内存数据
	Result.Size = MemoryChunk->GetUsedSize();
	Result.Buffer = ::_aligned_malloc(Result.Size, SG_MEM_ALIGNMENT);
	memset(Result.Buffer, 0, Result.Size);
	memcpy(Result.Buffer, MemoryChunk->BasePtr, Result.Size);
	Result.BaseAddr = (uint64)MemoryChunk->BasePtr;

	//收集内存结构
	Result.AllocatedMemory = Allocator->AllocatedMemory;
	for (uint16 i = 0; i < SG_MEM_POOL_COUNT; ++i)
	{
		SGFreeBlockList& FreeList = Allocator->FreeLists[i];
		SGBundleNode* Head = FreeList.Bundle.Head;
		uint32 Count = FreeList.Bundle.Count;

		uint64 RVA = (uint64)Head - (uint64)MemoryChunk->BasePtr;
		uint64 Temp = (RVA << 32 ) | Count;
		Result.FreeBundles[i] = Temp;
	}
	
	return Result;
}


bool SGMemoryManager::ResumeSnapshot(const SGMemorySnapshot& InSnapshot)
{
	if (MemoryChunk->ChunkSize < InSnapshot.Size)
	{
		//处理错误
		return false;
	}

	if (!IsPowerOfTwo(InSnapshot.Size) || InSnapshot.Size % MemoryChunk->PageSize != 0)
	{
		//处理错误
		return false;
	}
	
	//恢复内存数据
	memcpy(MemoryChunk->BasePtr, InSnapshot.Buffer, InSnapshot.Size);
	MemoryChunk->NumPageUsed = InSnapshot.Size / MemoryChunk->PageSize;

	//恢复内存结构
	uint64 OffsetAddr = (uint64)MemoryChunk->BasePtr - InSnapshot.BaseAddr;
	Allocator->AllocatedMemory = InSnapshot.AllocatedMemory;
	for (uint16 i = 0; i < SG_MEM_POOL_COUNT; ++i)
	{
		uint64 Temp = InSnapshot.FreeBundles[i];
		uint64 RVA = Temp >> 32;
		uint32 Count = (uint32)(Temp & 0xFFFFFFFF);

		SGFreeBlockList& FreeList = Allocator->FreeLists[i];
		FreeList.Bundle.Head = (SGBundleNode*)(RVA + (uint64)MemoryChunk->BasePtr);
		FreeList.Bundle.Count = Count;

		SGBundleNode* Node = FreeList.Bundle.Head;
		uint32 ValidCount = 0;
		while (Node && ValidCount < Count)
		{
			uint64 OldAddr = (uint64)Node->NextNodeInCurrentBundle;
			uint64 NewAddr = OldAddr + OffsetAddr;
			Node->NextNodeInCurrentBundle = (SGBundleNode*)NewAddr;
			ValidCount++;
			Node = Node->NextNodeInCurrentBundle;
		}
	}
	
	return true;
}

SGMemoryManager * GDefaultMemoryManager = nullptr;