#pragma once
#include "SGMemoryDefines.h"
#include <new>
#include <iostream>


//////////////////////////////////////////////////////////////////////////

class ISGMalloc
{
public:
	virtual void* Malloc(size_t Size, uint32 Alignment = SG_MEM_ALIGNMENT) = 0;
	virtual void* Realloc(void* Original, size_t Size, uint32 Alignment = SG_MEM_ALIGNMENT) = 0;
	virtual void Free(void* Ptr) = 0;
};

//////////////////////////////////////////////////////////////////////////

//16 byte
struct SGBundleNode
{
	SGBundleNode* NextNodeInCurrentBundle;
	uint32 Count;
};


struct SGBundle
{
	SGBundleNode* Head;
	uint32 Count;

	~SGBundle()
	{
		Head = nullptr;
		Count = 0;
	}

	FORCEINLINE SGBundle()
	{
		Reset();
	}

	FORCEINLINE void Reset()
	{
		Head = nullptr;
		Count = 0;
	}

	FORCEINLINE void PushToFront(SGBundleNode* InNode)
	{
		InNode->NextNodeInCurrentBundle = Head;
		InNode->Count = 0;
		Head = InNode;
		Count++;
	}

	FORCEINLINE SGBundleNode* PopFromFront()
	{
		SGBundleNode* Result = Head;
		Count --;
		Head = Result->NextNodeInCurrentBundle;
		return Result;
	}
};

//16 byte
struct SGFreeBlock
{
	uint16 BlockSize;
	uint8 PoolIndex;
	uint32 NumFreeBlocks;
	void* NextFreeBlock;

	SGFreeBlock(uint32 InPoolSize, uint16 InBlockSize, uint8 InPoolIndex)
	{
		BlockSize = InBlockSize;
		PoolIndex = InPoolIndex;
		NextFreeBlock = nullptr;
		NumFreeBlocks = InPoolSize / InBlockSize;

		//对于不能整除的，这个条件一定是不成立的，那么正好可以用余数空间来作为FirstFreeBlock
		if (NumFreeBlocks * InBlockSize + sizeof(SGFreeBlock) > InPoolSize)
		{
			//对于能够整除的，需要占用一个Block来用作FirstFreeBlock
			NumFreeBlocks--;
		}
		
	}

	//分配一个Block出去
	FORCEINLINE void* AllocateRegularBlock()
	{
		--NumFreeBlocks;
		if (IsAligned(this, SG_MEM_POOL_SIZE))
		{
			return (uint8*)this + SG_MEM_POOL_SIZE - (NumFreeBlocks + 1) * BlockSize;
		}
		return (uint8*)this + (NumFreeBlocks)*BlockSize;
	}

	FORCEINLINE uint32 GetNumFreeRegularBlocks() const
	{
		return NumFreeBlocks;
	}
};

//////////////////////////////////////////////////////////////////////////
class SGFreeBlockList
{
private:
	friend class SGMemoryManager;
	SGBundle Bundle;

public:
	FORCEINLINE bool PushToFront(void* InPtr)
	{
		Bundle.PushToFront((SGBundleNode*)InPtr);
		return true;
	}

	FORCEINLINE void* PopFromFront()
	{
		return Bundle.Head ? Bundle.PopFromFront() : nullptr;
	}

};



//////////////////////////////////////////////////////////////////////////


class SGMemoryChunk
{
private:
	friend class SGMemoryManager;

	void* BasePtr = nullptr;
	uint32 ChunkSize = 0;
	uint32 PageSize = 0;
	uint16 PageCount = 0;
	uint16 NumPageUsed = 0;

public:
	SGMemoryChunk(uint32 InChunkSize, uint32 InPageSize)
	{
		if (!IsPowerOfTwo(InPageSize))
		{
			//处理错误
			return;
		}
		
		PageSize = InPageSize;
		PageCount = ((InChunkSize + InPageSize - 1) / InPageSize);
		ChunkSize = PageCount * InPageSize;
		BasePtr = ::_aligned_malloc(ChunkSize, PageSize);

		if (BasePtr == nullptr)
		{
			//处理错误
		}
	}

	~SGMemoryChunk()
	{
		if (BasePtr)
		{
			::_aligned_free(BasePtr);
		}
	}

	bool ApplySnapshot(void* InBuffer, uint32 InSize)
	{
		if (InSize > ChunkSize || InSize % PageSize != 0)
		{
			//处理错误
			return false;
		}

		NumPageUsed = InSize / PageSize;
		memcpy(BasePtr, InBuffer, InSize);
		return true;
	}

	FORCEINLINE void* GetBasePtr() const
	{
		return BasePtr;
	}

	FORCEINLINE uint32 GetUsedSize() const
	{
		return NumPageUsed * PageSize;
	}

	FORCEINLINE uint32 GetChunkSize() const
	{
		return ChunkSize;
	}

	FORCEINLINE void* AllocatePage()
	{
		void* Result = nullptr;
		if (NumPageUsed < PageCount)
		{
			Result = ((uint8*)BasePtr) + NumPageUsed * PageSize;
			++NumPageUsed;
		}
		else
		{
			//处理错误
		}
		return Result;
	}

	
};



class SGPoolAllocator : public ISGMalloc
{
private:
	friend class SGMemoryManager;
	static uint16 MemSizeToIndex[SG_MEM_BUCKET_COUNT];
	static uint16 SmallBlockSizes[SG_MEM_POOL_COUNT];
	SGFreeBlockList FreeLists[SG_MEM_POOL_COUNT];
	int64 AllocatedMemory = 0;
	SGMemoryChunk* MemoryChunk = nullptr;

public:
	SGPoolAllocator(SGMemoryChunk* InChunk);
	virtual ~SGPoolAllocator();

	FORCEINLINE void* Malloc(size_t Size, uint32 Alignment = SG_MEM_ALIGNMENT) override
	{
		void* Result = nullptr;
		if (Size <= SG_MEM_POOL_SCALE && Alignment <= SG_MEM_ALIGNMENT)
		{
			uint32 PoolIndex = BoundSizeToPoolIndex(Size);
			Result = FreeLists[PoolIndex].PopFromFront();
			if (Result)
			{
				uint32 BlockSize = PoolIndexToBlockSize(PoolIndex);
				AllocatedMemory += BlockSize;
			}

			if (Result == nullptr)
			{
				Result = MallocExternal(Size, Alignment);
			}
		}
		else
		{
			//分配失败处理
		}

		return Result;
	}


	FORCEINLINE void* Realloc(void* Original, size_t NewSize, uint32 Alignment = SG_MEM_ALIGNMENT) override
	{
		if (NewSize <= SG_MEM_POOL_SCALE && Alignment <= SG_MEM_ALIGNMENT)
		{
			uint32 BlockSize = 0;
			uint32 PoolIndex = 0;
			bool bCanFree = true;
		}
		return nullptr;
	}

	FORCEINLINE void Free(void* Ptr) override
	{
		SGFreeBlock* BasePtr = GetPoolHeaderFromPointer(Ptr);
		uint32 PoolIndex = BasePtr->PoolIndex;
		if (FreeLists[PoolIndex].PushToFront(Ptr))
		{
			AllocatedMemory -= BasePtr->BlockSize;
			return;
		}
		//错误处理
	}

private:
	void* MallocExternal(size_t Size, uint32 Alignment);
	SGFreeBlock* AllocateNewPoolWithFirstFreeBlock(uint32 InBlockSize, uint32 InPoolIndex);

	static FORCEINLINE SGFreeBlock* GetPoolHeaderFromPointer(void* Ptr)
	{
		return (SGFreeBlock*)AlignDown(Ptr, SG_MEM_POOL_SIZE);
	}

	FORCEINLINE uint32 BoundSizeToPoolIndex(size_t Size)
	{
		auto BucketIndex = ((Size + SG_MEM_ALIGNMENT - 1) >> 4);
		uint32 PoolIndex = uint32(MemSizeToIndex[BucketIndex]);
		return PoolIndex;
	}
	FORCEINLINE uint32 PoolIndexToBlockSize(uint32 PoolIndex)
	{
		return SmallBlockSizes[PoolIndex];
	}

};