#include "SGObjectManager.h"


SGObjectManager& SGObjectManager::Get()
{
	static SGObjectManager Inst;
	return Inst;
}

SGObjectManager::SGObjectManager()
{
	HandleObjects.push_back(nullptr);
	TypeTable.push_back(nullptr);
}

void SGObjectManager::AddHandleObject(SGObject* InObject)
{
	uint32 FreeHandle = 0;
	if (!FreeHandleValues.empty())
	{
		FreeHandle = FreeHandleValues.top();
		FreeHandleValues.pop();
	}

	if (FreeHandle == 0)
	{
		HandleObjects.push_back(InObject);
		FreeHandle = (uint32)HandleObjects.size() - 1;
	}
	else
	{
		if (FreeHandle >= HandleObjects.size())
		{
			//TODO：处理错误
			return;
		}
		HandleObjects[FreeHandle] = InObject;
	}

	ValidCodeGenerator++;
	InObject->Handle.Value = FreeHandle;
	InObject->Handle.ValidCode = ValidCodeGenerator;
}

void SGObjectManager::RemoveHandleObject(SGObject* InObject)
{
	uint32 HandleValue = InObject->Handle.Value;
	InObject->Handle.Value = 0;
	InObject->Handle.ValidCode = 0;
	FreeHandleValues.push(HandleValue);
	HandleObjects[HandleValue] = nullptr;
}


SGObject* SGObjectManager::GetHandleObject(const SGHandle& InHandle) const
{
	if (InHandle.Value < HandleObjects.size())
	{
		SGObject* Result = HandleObjects[InHandle.Value];
		if (Result->Handle.ValidCode == InHandle.ValidCode)
		{
			return Result;
		}
	}
	return nullptr;
}

void SGObjectManager::RegisterType(SGTypeInfo* InTypeInfo)
{
	TypeTable.push_back(InTypeInfo);
	InTypeInfo->Id = (uint32)TypeTable.size() - 1;
}

SGObjectSnapshot SGObjectManager::MakeSnapshot() const
{
	SGObjectSnapshot Result;

	SnapshotPtrAddrs.clear();

	//收集BaseAddr
	void* BasePtr = GDefaultMemoryManager->GetMemoryChunk()->GetBasePtr();
	Result.BaseAddr = (uint64) BasePtr;

	//收集对象列表
	for (auto it = HandleObjects.begin(); it != HandleObjects.end(); ++it)
	{
		SGObject* Obj = *it;
		if (Obj)
		{
			Result.ObjHandleValues.push_back(Obj->Handle.Value);
			uint64 RVA = (uint64)Obj - (uint64)BasePtr;
			Result.ObjRVATable.push_back((uint32)RVA);
			Obj->MakeSnapshot();
		}
	}

	//收集自由指针
	for (auto it = SnapshotPtrAddrs.begin(); it != SnapshotPtrAddrs.end(); ++it)
	{
		void** PtrAddr = *it;
		uint64 RVA = (uint64)PtrAddr - (uint64)BasePtr;
		Result.PtrRVATable.push_back((uint32)RVA);
	}

	SnapshotPtrAddrs.clear();

	//收集自由句柄
	std::stack<uint32> TempStack = FreeHandleValues;
	while (!TempStack.empty())
	{
		Result.FreeHandleValues.push_back(TempStack.top());
		TempStack.pop();
	}

	//收集校验生成码
	Result.ValidCodeGenerator = ValidCodeGenerator;
	return Result;
}

bool SGObjectManager::ResumeSnapshot(const SGObjectSnapshot& InSnapshot)
{
	void* BasePtr = GDefaultMemoryManager->GetMemoryChunk()->GetBasePtr();
	uint64 OffsetAddr = (uint64)BasePtr - InSnapshot.BaseAddr;

	//恢复校验生成码
	this->ValidCodeGenerator = InSnapshot.ValidCodeGenerator;

	//恢复自由句柄
	this->FreeHandleValues = std::stack<uint32>();
	for (auto it = InSnapshot.FreeHandleValues.begin(); it != InSnapshot.FreeHandleValues.end(); ++it)
	{
		this->FreeHandleValues.push(*it);
	}
	
	//恢复自由指针
	for (auto it = InSnapshot.PtrRVATable.begin(); it != InSnapshot.PtrRVATable.end(); ++it)
	{
		uint64 RVA = *it;
		uint64* PtrAddr = (uint64*)((uint64)BasePtr + RVA);
		uint64 NewPtrAddr = (*PtrAddr) + OffsetAddr;
		*PtrAddr = NewPtrAddr;
	}

	//恢复对象列表
	this->HandleObjects.clear();
	for (uint32 i = 0; i < InSnapshot.ObjHandleValues.size(); ++i)
	{
		//恢复对象地址
		uint32 HandleValue = InSnapshot.ObjHandleValues[i];
		uint64 RVA = InSnapshot.ObjRVATable[i];
		uint64 NewAddr = (uint64)BasePtr + RVA;
		
		while(HandleValue > this->HandleObjects.size()) this->HandleObjects.push_back(nullptr);
		SGObject* Obj = (SGObject*)NewAddr;
		this->HandleObjects.push_back(Obj);

		//恢复对象虚表
		const SGTypeInfo* TypeInfo = this->GetTypeInfo(Obj->TypeId);
		uint64 SnapshotVTableAddr = *(uint64*)Obj;
		*(uint64*)Obj = TypeInfo->VTableAddr;
		
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

