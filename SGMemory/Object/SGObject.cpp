#include "SGObject.h"
#include "SGObjectManager.h"

void ISGSnapshotable::MarkPtrAddr(void** InPtrAddr) const
{
	SGObjectManager::Get().SnapshotPtrAddrs.insert(InPtrAddr);
}


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

