// SGMemory.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SGObjectManager.h"


class SGFixedString : ISGSnapshotable
{
private:
    const char* Buffer = nullptr;
public:
    SGFixedString(){}
    SGFixedString(const char* InStr)
    {
        SetString(InStr);
    }
    ~SGFixedString()
    {
        SGFree(Buffer);
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
		Buffer = (const char*) SGMalloc(size);
		memset((void*)Buffer, 0, size);
		memcpy((void*)Buffer, InStr, size);
    }
public:
    void MakeSnapshot() override
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

};

class SGActor : public SGObject
{
    SG_OBJECT_TYPE_DECL(SGActor, SGObject);
   protected:
    SGFixedString ActorName;
public:
    void Create(const char* InName)
    {
        ActorName = InName;
    }
public:
    virtual void Tick(float InDeltaTime)
    {
        printf("SGActor<%s>::Tick(%f)\n", *ActorName, InDeltaTime);
    }
    
};
SG_OBJECT_TYPE_IMPL(SGActor);


class SGPlayer : public SGActor
{
	SG_OBJECT_TYPE_DECL(SGPlayer, SGActor);

public:
	virtual void Tick(float InDeltaTime)
	{
		printf("%s<%s>::Tick(%f)\n", GetTypeInfo()->Name, *ActorName, InDeltaTime);
        SGActor::Tick(InDeltaTime);
	}
};
SG_OBJECT_TYPE_IMPL(SGPlayer);

class SGEnemy: public SGActor
{
	SG_OBJECT_TYPE_DECL(SGEnemy, SGActor);

public:
	virtual void Tick(float InDeltaTime)
	{
		printf("%s<%s>::Tick(%f)\n", GetTypeInfo()->Name, *ActorName, InDeltaTime);
		SGActor::Tick(InDeltaTime);
	}
};
SG_OBJECT_TYPE_IMPL(SGEnemy);

class SGWorld : public SGObject
{
	SG_OBJECT_TYPE_DECL(SGWorld, SGObject);
private:
    SGPlayer* Actor1;
    SGEnemy* Actor2;

public:
    void Create()
    {
        Actor1 = NewObject<SGPlayer>();
        Actor1->Create("Slight");
		Actor2 = NewObject<SGEnemy>();
		Actor2->Create("Cold");
    }

	virtual void Tick(float InDeltaTime)
	{
		printf("SGWorld::Tick(%f)\n", InDeltaTime);
        Actor1->Tick(InDeltaTime);
        Actor2->Tick(InDeltaTime);
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
    GDefaultMemoryManager = new SGMemoryManager();
    SGHandlePtr<SGWorld> World = CreateWorld();
    World->Tick(0.33);

    

    



    
    std::cout << "Hello World!\n";
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
