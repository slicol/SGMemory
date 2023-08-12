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
typedef uint32 SGTypeId;
class SGTypeInfo;

class ISGSnapshotable
{
public:
	virtual void MakeSnapshot() = 0;
};

class SGObject : public ISGSnapshotable
{
private:
	friend class SGObjectManager;
	SGHandle Handle;
	SGTypeId TypeId = 0;
public:
	const SGHandle& GetHandle() const{return Handle;}
	const SGTypeInfo* GetTypeInfo() const;
	SGObject(VTConstructor*){}
	virtual ~SGObject();
	void MakeSnapshot() override{}
public:
	SGObject(SGTypeInfo* InTypeInfo);
};


class SGObjectManager
{
private:
	friend class SGObject;
	std::vector<SGObject*> HandleObjects;
	std::stack<uint32> FreeHandleValues;
	std::vector<SGTypeInfo*> TypeTable;
	uint32 ValidCodeGenerator = 0;

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

class SGTypeInfo
{
private:
	friend class SGObjectManager;
	friend class SGObject;
public:
	uint32 Id;
	const char* Name;
public:
	SGTypeInfo(const char* InName): Id(0), Name(InName){};
	virtual void ResumeVT(void* InObject) = 0;
};


template<typename T>
struct SGTypeRegister : public SGTypeInfo
{
	SGTypeRegister(const char* InName): SGTypeInfo(InName)
	{
		SGObjectManager::Get().RegisterType(this);
	}

	void ResumeVT(void* InObject) override
	{
		new (InObject) T((VTConstructor*)0);
	}
};


//////////////////////////////////////////////////////////////////////////

template<typename T, typename ... Args>
T* NewObject(Args&& ... InArgs)
{
	void* Ptr = SGMalloc(sizeof(T));
	T* Result = new (Ptr) T(std::forward<Args>(InArgs)...);
	return Result;
}


FORCEINLINE void FreeObject(void* Ptr)
{
	SGFree(Ptr);
}


//////////////////////////////////////////////////////////////////////////

#define SG_OBJECT_TYPE_DECL(T, S) \
	public: T(VTConstructor* v) :S(v) {} \
	public: T(SGTypeInfo* v): S(v){}\
	public: T(): S(&TypeRegister){}\
	private: static SGTypeRegister<T> TypeRegister \

	
#define SG_OBJECT_TYPE_IMPL(T) SGTypeRegister<T> T::TypeRegister(#T)


//////////////////////////////////////////////////////////////////////////

