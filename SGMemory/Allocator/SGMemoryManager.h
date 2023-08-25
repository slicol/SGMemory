#pragma once
#include "SGMemoryDefines.h"
#include "SGMemoryAllocator.h"



struct SGMemorySnapshot
{
	void* Buffer = nullptr;
	uint32 Size = 0;
	uint64 BaseAddr = 0;
	uint64 FreeBundles[SG_MEM_POOL_COUNT] = {0};
	int64 AllocatedMemory = 0;
};

class SGMemoryManager
{
private:
	SGMemoryChunk* MemoryChunk = nullptr;
	SGPoolAllocator* Allocator = nullptr;

public:
	SGMemoryManager()
	{
		MemoryChunk = new SGMemoryChunk(SG_MEM_CHUNK_SIZE, SG_MEM_POOL_SIZE);
		Allocator = new SGPoolAllocator(MemoryChunk);
	}
	~SGMemoryManager()
	{
		delete Allocator;
		delete MemoryChunk;
	}

public:
	void* Malloc(size_t Size, uint32 Alignment = SG_MEM_ALIGNMENT)
	{
		return Allocator->Malloc(Size, Alignment);
	}

	void* Realloc(void* Original, size_t Size, uint32 Alignment = SG_MEM_ALIGNMENT)
	{
		return Allocator->Realloc(Original, Size, Alignment);
	}

	void Free(void* Ptr)
	{
		return Allocator->Free(Ptr);
	}

public:
	FORCEINLINE const SGMemoryChunk* GetMemoryChunk() const{return MemoryChunk;}
	SGMemorySnapshot MakeSnapshot() const;
	bool ResumeSnapshot(const SGMemorySnapshot& InSnapshot);

private:

};


extern SGMemoryManager * GDefaultMemoryManager;

#define SGMalloc(Size) GDefaultMemoryManager->Malloc((size_t)Size)
#define SGFree(Ptr) GDefaultMemoryManager->Free((void*)Ptr)