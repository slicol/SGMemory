#include "SGObjectManager.h"


SGObject::SGObject(SGTypeInfo* InTypeInfo)
{
	SGObjectManager::Get().AddHandleObject(this);
	TypeId = InTypeInfo->Id;
}

SGObject::~SGObject()
{
	SGObjectManager::Get().RemoveHandleObject(this);
}

const SGTypeInfo* SGObject::GetTypeInfo() const
{
	return SGObjectManager::Get().GetTypeInfo(TypeId);
}


SGObjectManager& SGObjectManager::Get()
{
	static SGObjectManager Inst;
	return Inst;
}

SGObjectManager::SGObjectManager()
{
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
			//´¦Àí´íÎó
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
	InTypeInfo->Id = TypeTable.size() - 1;
}

//////////////////////////////////////////////////////////////////////////

