#pragma once
#include "SGDefines.h"
#include "Allocator/SGMemoryManager.h"


struct SGHandle
{
	uint32 Value = 0;
	uint32 ValidCode = 0;
};

struct VTConstructor;
typedef uint32 SGTypeId;
class SGTypeInfo;

class ISGSnapshotable
{
public:
	virtual ~ISGSnapshotable(){}
	virtual void MakeSnapshot() const{};
	void MarkPtrAddr(void** InPtrAddr) const;
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
	
public:
	SGObject(SGTypeInfo* InTypeInfo);
};



class SGTypeInfo
{
private:
	friend class SGObjectManager;
	friend class SGObject;
public:
	uint32 Id;
	const char* Name;
	uint64 VTableAddr;
public:
	SGTypeInfo(const char* InName): Id(0), Name(InName), VTableAddr(0){};
};


template<typename T>
struct SGTypeRegister : public SGTypeInfo
{
	SGTypeRegister(const char* InName): SGTypeInfo(InName)
	{
		SGObjectManager::Get().RegisterType(this);

		T Temp((VTConstructor*)0);
		VTableAddr = *(uint64*)&Temp;
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
	public: T(VTConstructor* v):S(v){}\
	public: T(SGTypeInfo* v): S(v){}\
	public: T(): S(&TypeRegister){}\
	private: static SGTypeRegister<T> TypeRegister \

	
#define SG_OBJECT_TYPE_IMPL(T) SGTypeRegister<T> T::TypeRegister(#T)


//////////////////////////////////////////////////////////////////////////