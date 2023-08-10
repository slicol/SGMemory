#pragma once

#define SG_MEM_ALIGNMENT 16

class ISGMalloc
{
public:
	virtual void* Malloc(size_t Count, uint32 Alignment = SG_MEM_ALIGNMENT) = 0;
	virtual void* Realloc(void* Original, size_t Count, uint32 Alignment = SG_MEM_ALIGNMENT) = 0;
	virtual void Free(void* Original) = 0;
};

//////////////////////////////////////////////////////////////////////////

struct SGBundleNode
{
	SGBundleNode* NextNodeInCurrentBundle;
	union
	{
		SGBundleNode* NextBundle;
		uint32 Count;
	}
};


struct SGBundle
{
	SGBundleNode* Head;
	uint32 Count;

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
		InNode->NextBundle = nullptr;
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

struct SGFreeBlock
{
	uint16 BlockSize;
	uint8 PoolIndex;
	uint32 NumFreeBlocks;
	void* NextFreeBlock;

	SGFreeBlock(uint32 InBubbleSize, uint16 InBlockSize, uint8 InPoolIndex)
	{
		NumFreeBlocks = InBubbleSize / InBlockSize;
		if (NumFreeBlocks * InBlockSize + sizeof(SGFreeBlock) > InBubbleSize)
		{
			//在Bubble的首部留出一个Block用来存储这个Bubble的必要信息。
			NumFreeBlocks--;
		}
	}
};

//////////////////////////////////////////////////////////////////////////
class SGFreeBlockList
{
private:
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
struct SGPoolInfo
{

};


class SGPoolAllocator : public ISGMalloc
{
private:
	void* BaseAddr = nullptr;
	int32 PoolSize = 0;
	int32 BlockSize = 0;
	
public:
	SGPoolAllocator(void* InBaseAddr, int32 InPoolSize, int32 InBlockSize)
	{
		this->BaseAddr = InBaseAddr;
		this->PoolSize = InPoolSize;
		this->BlockSize = InBlockSize;

	}
	
	virtual ~SGPoolAllocator()
	{
	}
};


class SGBucketAllocator : public ISGMalloc
{
	
};

template <typename T>
FORCEINLINE constexpr T AlignDown(T Val, uint64 Alignment)
{
	return (T)(((uint64)Val) & ~(Alignment - 1));
}

template <typename T>
FORCEINLINE constexpr bool IsAligned(T Val, uint64 Alignment)
{
	return !((uint64)Val & (Alignment - 1));
}

#define SG_MEM_POOL_COUNT 26
#define SG_MEM_POOL_SCALE 2048-16
#define SG_MEM_BUCKET_COUNT  (1 + (SG_MEM_POOL_SCALE >> 4));

class SGMemoryManager
{
private:
	SGFreeBlockList FreeLists[SG_MEM_POOL_COUNT];
	static uint8 MemSizeToIndex[SG_MEM_BUCKET_COUNT];
	static uint16 SmallBlockSizesReversed[BINNED2_SMALL_POOL_COUNT];
	int64 AllocatedMemory = 0;

public:
	SGMemoryManager();

public:
	void* Malloc(size_t Size, uint32 Alignment = SG_DEFAULT_ALIGNMENT)
	{
		void* Result = nullptr;
		if (Size <= SG_MEM_POOL_SCALE && Alignment <= SG_MEM_ALIGNMENT)
		{
			uint32 PoolIndex = BoundSizeToPoolIndex(Size);
			Result = FreeLists[PoolIndex].PopFromFront(PoolIndex);
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

	void* MallocExternal(size_t Size, uint32 Alignment)
	{
		uint32 PoolIndex = BoundSizeToPoolIndex(Size);



	}

	void* Realloc(void* Original, size_t NewSize, uint32 Alignment = SG_DEFAULT_ALIGNMENT)
	{
		if (Size <= SG_MEM_POOL_SCALE && Alignment <= SG_MEM_ALIGNMENT)
		{
			uint32 BlockSize = 0;
			uint32 PoolIndex = 0;
			bool bCanFree = true;
		}
	}

	void Free(void* Ptr)
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
	static FORCEINLINE SGFreeBlock* GetPoolHeaderFromPointer(void* Ptr)
	{
		return (SGFreeBlock*)AlignDown(Ptr, 65535);
	}

	FORCEINLINE uint32 BoundSizeToPoolIndex(size_t Size)
	{
		auto BucketIndex = ((Size + SG_MEM_ALIGNMENT - 1) >> 4);
		uint32 PoolIndex = uint32(MemSizeToIndex[BucketIndex]);
		return PoolIndex;
	}
	FORCEINLINE uint32 PoolIndexToBlockSize(uint32 PoolIndex)
	{
		return SmallBlockSizesReversed[SG_MEM_POOL_COUNT - PoolIndex - 1];
	}
};


