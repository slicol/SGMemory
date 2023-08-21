// SGMemory.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SGObjectManager.h"
#include "SGContainers.h"


//////////////////////////////////////////////////////////////////////////

class SGActor : public SGObject
{
    SG_OBJECT_TYPE_DECL(SGActor, SGObject);
protected:
    SGFixedString ActorName;

protected:
	void MakeSnapshot() const override
	{
        ActorName.MakeSnapshot();
	}


public:
    void Create(const char* InName)
    {
        ActorName = InName;
    }
public:
    virtual void Tick(float InDeltaTime)
    {
        printf("[%p] SGActor<%s>::Tick(%f)\n", this, *ActorName, InDeltaTime);
    }
    
};
SG_OBJECT_TYPE_IMPL(SGActor);

//////////////////////////////////////////////////////////////////////////
class SGPlayer : public SGActor
{
	SG_OBJECT_TYPE_DECL(SGPlayer, SGActor);

public:
	virtual void Tick(float InDeltaTime)
	{
		printf("[%p] %s<%s>::Tick(%f)\n", this, GetTypeInfo()->Name, *ActorName, InDeltaTime);
        SGActor::Tick(InDeltaTime);
	}
};
SG_OBJECT_TYPE_IMPL(SGPlayer);

//////////////////////////////////////////////////////////////////////////
class SGEnemy: public SGActor
{
	SG_OBJECT_TYPE_DECL(SGEnemy, SGActor);

public:
	virtual void Tick(float InDeltaTime)
	{
		printf("[%p] %s<%s>::Tick(%f)\n", this, GetTypeInfo()->Name, *ActorName, InDeltaTime);
		SGActor::Tick(InDeltaTime);
	}
};
SG_OBJECT_TYPE_IMPL(SGEnemy);

//////////////////////////////////////////////////////////////////////////
class SGWorld : public SGObject
{
	SG_OBJECT_TYPE_DECL(SGWorld, SGObject);
private:
    SGPlayer* Actor1 = nullptr;
    SGEnemy* Actor2 = nullptr;

protected:
    void MakeSnapshot() const override
    {
        MarkPtrAddr((void**)&Actor1);
        MarkPtrAddr((void**)&Actor2);
    }

public:
    void Create()
    {
        Actor1 = NewObject<SGPlayer>();
        Actor1->Create("Slight");
		Actor2 = NewObject<SGEnemy>();
		Actor2->Create("Cold");
    }

    void KillEnemy()
    {
        printf("[%p] SGWorld::KillEnemy()\n", this);
        FreeObject(Actor2);
        Actor2 = nullptr;
    }

	void NewEnemy()
	{
		printf("[%p] SGWorld::NewEnemy()\n", this);
		Actor2 = NewObject<SGEnemy>();
		Actor2->Create("NewEnemy");
		Actor2 = NewObject<SGEnemy>();
		Actor2->Create("NewEnemy");
	}

	virtual void Tick(float InDeltaTime)
	{
		printf("[%p] SGWorld::Tick(%f)\n", this, InDeltaTime);
        Actor1->Tick(InDeltaTime);
        if (Actor2)
        {
            Actor2->Tick(InDeltaTime);
        }
	}
};
SG_OBJECT_TYPE_IMPL(SGWorld);

//////////////////////////////////////////////////////////////////////////


SGHandle CreateWorld()
{
	SGWorld* World = NewObject<SGWorld>();
	World->Create();
	return World->GetHandle();
}


int main()
{
    SGHandle WorldHandle;

    
    {
        //创建一个Chunk
		GDefaultMemoryManager = new SGMemoryManager();
        printf("MemoryChunk = %p\n", GDefaultMemoryManager->GetMemoryChunk()->GetBasePtr());

        //创建游戏世界
        WorldHandle = CreateWorld();

        //执行逻辑
		SGHandlePtr<SGWorld> World = WorldHandle;
		World->Tick(0.33f);
        //World->KillEnemy();
        //World->Tick(0.33f);
        //World->NewEnemy();
        //World->Tick(0.33f);
    }
    
    //创建快照
	SGMemorySnapshot MemSnapshot = GDefaultMemoryManager->MakeSnapshot();
	SGObjectSnapshot ObjSnapshot = SGObjectManager::Get().MakeSnapshot();


	//打乱内存
	{
		delete GDefaultMemoryManager;
		new SGMemoryManager();
	}

    {
        //创建新的Chunk
        GDefaultMemoryManager = new SGMemoryManager();
        printf("MemoryChunk = %p\n", GDefaultMemoryManager->GetMemoryChunk()->GetBasePtr());

        //恢复快照
        GDefaultMemoryManager->ResumeSnapshot(MemSnapshot);
        SGObjectManager::Get().ResumeSnapshot(ObjSnapshot);

        //直接执行逻辑
        SGHandlePtr<SGWorld> World = WorldHandle;
        World->Tick(0.33f);
		World->KillEnemy();
		World->Tick(0.33f);
		World->NewEnemy();
		World->Tick(0.33f);
    }

    



    
    std::cout << "Hello World!\n";
}


class TestStruct
{
public:
	int32 A = 1;
	float B = 2;
private:
	int32 C = 3;
protected:
	float D = 4;
private:
    int64 Foo(int32, float ){return 0;};
};

// Put these code in the same namespace:
MTCL_EXPORT_PROPERTY_OFFSET(TestStruct, A, int32);
MTCL_EXPORT_PROPERTY_OFFSET(TestStruct, B, float);
MTCL_EXPORT_PROPERTY_OFFSET(TestStruct, C, int32);
MTCL_EXPORT_PROPERTY_OFFSET(TestStruct, D, float);

template <class Tag, typename Tag::offset x>
struct MTCL_PrintRVA
{
	void _()
	{
        auto a = x;
	#ifdef __clang__
		//#pragma message(__PRETTY_FUNCTION__ ) 
	#else
	#pragma message(__FUNCSIG__ ) 
	#endif
	}
};

//namespace MTCL { struct TestStruct_Foo{ typedef void(TestStruct::* offset)() ; }; }	
//template struct MTCL_CheckFuncImpl<MTCL::TestStruct_Foo, &TestStruct::Foo>;


MTCL_CHECK_FUNC_IMPL(TestStruct, Foo, int64, int32, float );

void MetaCheck()
{
    auto a = &TestStruct::A;
    
    
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
