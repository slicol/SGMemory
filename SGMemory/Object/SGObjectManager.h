#pragma once
#include "SGDefines.h"
#include "Allocator/SGMemoryManager.h"
#include "SGObject.h"
#include <vector>
#include <stack>
#include <set>



struct SGObjectSnapshot
{
	uint64 BaseAddr = 0;
	std::vector<uint32> ObjHandleValues;
	std::vector<uint32> ObjRVATable;
	std::vector<uint32> PtrRVATable;
	std::vector<uint32> FreeHandleValues;
	uint32 ValidCodeGenerator = 0;
};


class SGObjectManager
{
private:
	friend class SGObject;
	friend class ISGSnapshotable;
	std::vector<SGTypeInfo*> TypeTable;
private:
	std::vector<SGObject*> HandleObjects;
	std::stack<uint32> FreeHandleValues;
	mutable std::set<void**> SnapshotPtrAddrs;
	uint32 ValidCodeGenerator = 0;
private:

public:
	static SGObjectManager& Get();
	SGObjectManager();
private:
	void AddHandleObject(SGObject* InObject);
	void RemoveHandleObject(SGObject* InObject);
public:
	SGObject* GetHandleObject(const SGHandle& InHandle) const;
	void RegisterType(SGTypeInfo* InTypeInfo);
	FORCEINLINE const SGTypeInfo* GetTypeInfo(SGTypeId InTypeId)
	{
		return TypeTable[(uint32)InTypeId];
	}
	
	SGObjectSnapshot MakeSnapshot() const;
	bool ResumeSnapshot(const SGObjectSnapshot& InSnapshot);
};


//////////////////////////////////////////////////////////////////////////
template<typename T>
class SGHandlePtr : public SGHandle
{
public:
	SGHandlePtr(const SGHandle& InHandle) 
	{
		Value = InHandle.Value;
		ValidCode = InHandle.ValidCode;
	}

	FORCEINLINE T* operator->() const
	{
		return (T*)SGObjectManager::Get().GetHandleObject(*this);
	}
};

//////////////////////////////////////////////////////////////////////////



