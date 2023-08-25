#pragma once
#include "SGDefines.h"
#include "Object/SGObjectManager.h"
#include "Allocator/SGMemoryManager.h"

class SGFixedString : ISGSnapshotable
{
private:
	const char* Buffer = nullptr;
public:
	SGFixedString() {}
	SGFixedString(const char* InStr)
	{
		SetString(InStr);
	}
	~SGFixedString()
	{
		if (Buffer)
		{
			SGFree(Buffer);
			Buffer = nullptr;
		}
		
	}

	void operator=(const char* InStr)
	{
		SetString(InStr);
	}

	const char* operator*() const
	{
		return Buffer;
	}
private:
	FORCEINLINE void SetString(const char* InStr)
	{
		if (Buffer)
		{
			SGFree(Buffer);
		}
		auto size = strlen(InStr) + 1;
		Buffer = (const char*)SGMalloc(size);
		memset((void*)Buffer, 0, size);
		memcpy((void*)Buffer, InStr, size);
	}
public:
	void MakeSnapshot() const override
	{
		MarkPtrAddr((void**)&Buffer);
	}

};


class SGList
{

};