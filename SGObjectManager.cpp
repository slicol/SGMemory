#include "SGObjectManager.h"


SGObject::SGObject()
{
	SGObjectManager::Get().AddHandleObject(this);
}

SGObject::~SGObject()
{
	SGObjectManager::Get().RemoveHandleObject(this);
}



SGObjectManager& SGObjectManager::Get()
{
	static SGObjectManager Inst;
	return Inst;
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
