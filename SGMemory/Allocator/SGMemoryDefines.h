#pragma once
#include "SGDefines.h"

#define SG_MEM_ALIGNMENT 16
#define SG_MEM_POOL_COUNT 26
#define SG_MEM_POOL_SCALE (2048-16)
#define SG_MEM_POOL_SIZE  65536
#define SG_MEM_CHUNK_SIZE (1<<24)
#define SG_MEM_BUCKET_COUNT  (1 + (SG_MEM_POOL_SCALE >> 4))


//关键：基于对齐规则，找到指针所属的内存块
template <typename T>
FORCEINLINE constexpr T AlignDown(T Val, uint64 Alignment)
{
	return (T)(((uint64)Val) & ~(Alignment - 1));
}

//关键：判断一个指针是否刚好是对齐内存块的起点
template <typename T>
FORCEINLINE constexpr bool IsAligned(T Val, uint64 Alignment)
{
	return !((uint64)Val & (Alignment - 1));
}

template <typename T>
static FORCEINLINE bool IsPowerOfTwo(T Value)
{
	return ((Value & (Value - 1)) == (T)0);
}
