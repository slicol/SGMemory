#pragma once
#include "SGDefines.h"
#include <vector>
#include <stack>
#include "SGMemoryManager.h"

struct SGHandle
{
	int32 Value = 0;
	int32 ValidCode = 0;
};

struct VTConstructor;

class SGObject
{
private:
	friend class SGObjectManager;
	SGHandle Handle;
public:
	const SGHandle& GetHandle() const{return Handle;}
	SGObject(VTConstructor*){}
	virtual ~SGObject();
public:
	SGObject();;

};


class SGObjectManager
{
private:
	friend class SGObject;
	std::vector<SGObject*> HandleObjects;
	std::stack<uint32> FreeHandleValues;
	uint32 ValidCodeGenerator = 0;

public:
	static SGObjectManager& Get();

private:
	void AddHandleObject(SGObject* InObject);
	void RemoveHandleObject(SGObject* InObject);
};


template<typename T, typename ... Args>
T* NewObject(Args&& ... InArgs)
{
	void* Ptr = SGMemoryManager::Get().Malloc(sizeof(T));
	T* Result = new (Ptr) T(std::forward<Args>(InArgs)...);
	return Result;
}


FORCEINLINE void FreeObject(void* Ptr)
{
	SGMemoryManager::Get().Free(Ptr);
}