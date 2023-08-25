#pragma once
#include <stddef.h>
#include <stdint.h>

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif



#pragma region Print Private Property Offset in Compile

template<typename T, unsigned int N>
void MTCL_PrintOffsetPublic()
{
#ifdef __clang__
	//#pragma message(__PRETTY_FUNCTION__ ) 
#else
#pragma message(__FUNCSIG__ ) 
#endif
};

#define MTCL_EXPORT_PORPERTY_OFFSET_PUBLIC(Class, Property)	\
	constexpr int32 MTCL_##Class##_##Property##_##Offset## = MTRT_STRUCT_OFFSET(Class, Property);	\
	struct Class##_##Property {};\
	MTCL_PrintOffsetPublic<Class##_##Property, MTCL_##Class##_##Property##_##Offset##>();	\

#define MTCL_BEGIN_PRINT_OFFSET_PUBLIC void _MTCL(){
#define MTCL_END_PRINT_OFFSET_PUBLIC }


template <class Tag, typename Tag::offset x>
struct MTCL_PrintOffset
{
	void _()
	{
	#ifdef __clang__
		//#pragma message(__PRETTY_FUNCTION__ ) 
	#else
	#pragma message(__FUNCSIG__ ) 
	#endif
	}
};



template <class Tag, typename Tag::func x>
struct MTCL_CheckFuncImpl
{
	void _()
	{
		auto check = x;
	#ifdef __clang__
		//#pragma message(__PRETTY_FUNCTION__ ) 
	#else
	#pragma message(__FUNCSIG__ ) 
	#endif
	}
};


#define MTCL_EXPORT_PROPERTY_OFFSET(s,m,t) \
	namespace MTCL{ struct s##_##m { typedef t##(##s##::*offset); };}	\
	template struct MTCL_PrintOffset<MTCL::##s##_##m##, &##s##::##m##>


#define MTCL_CHECK_FUNC_IMPL(s,f,ret,...) \
	namespace MTCL { struct s##_##f{ typedef ret##(##s##::* func)(__VA_ARGS__); }; }	\
	template struct MTCL_CheckFuncImpl<MTCL::##s##_##f##, &##s##::##f##>


//Example:
/*
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
		int64 Foo(int32, float );
	};

	// Put these code in the same namespace:
	MTCL_EXPORT_PROPERTY_OFFSET(TestStruct, A, int32);
	MTCL_EXPORT_PROPERTY_OFFSET(TestStruct, B, float);
	MTCL_EXPORT_PROPERTY_OFFSET(TestStruct, C, int32);
	MTCL_EXPORT_PROPERTY_OFFSET(TestStruct, D, float);

	MTCL_CHECK_FUNC_IMPL(TestStruct, Foo, int64, int32, float );

	// Compiler Output:
	3>void __cdecl MTCL_PrintOffset<struct MTCL::TestStruct_D,12>::_(void)
	3>void __cdecl MTCL_PrintOffset<struct MTCL::TestStruct_C,8>::_(void)
	3>void __cdecl MTCL_PrintOffset<struct MTCL::TestStruct_B,4>::_(void)
	3>void __cdecl MTCL_PrintOffset<struct MTCL::TestStruct_A,0>::_(void)
*/




#pragma endregion
