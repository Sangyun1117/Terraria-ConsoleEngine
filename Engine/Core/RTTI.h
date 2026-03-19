/******************************************************************************
 * RTTI.h
 *
 * 상속 관계에 있는 클래스 간의 런타임 타입 확인 및 안전한 형변환을 지원
 * C++의 dynamic_cast를 대체하거나, 일부 환경에서 빠른 타입 체크를 위해 사용
 *
 ******************************************************************************/

#pragma once
#include "Core.h"

// RunTime-Type-Information.
class Engine_API RTTI
{
public:
	// 순수 가상 함수: 객체의 타입 ID 반환
	virtual const size_t& TypeIdInstance() const = 0;

	//타입id가 일치하는지 확인
	virtual bool Is(const size_t id) const
	{
		return false;
	}

	//지정한 T 타입으로 안전하게 캐스팅
	template<typename T>
	T* As()
	{
		if (Is(T::TypeIdClass()))
		{
			return (T*)this;
		}

		//타입이 다르면 nullptr 반환
		return nullptr;
	}

	template<typename T>
	const T* As() const
	{
		if (Is(T::TypeIdClass()))
		{
			return (T*)this;
		}

		return nullptr;
	}
};

// 매크로: 클래스 내부에 선언하여 RTTI 기능 활성화
#define RTTI_DECLARATIONS(Type, ParentType)												\
public:																					\
	using super = ParentType;															\
    /* 객체 타입 ID 반환 */																\
	virtual const size_t& TypeIdInstance() const { return Type::TypeIdClass(); }		\
	static const size_t TypeIdClass()													\
	{																					\
		static int runTimeTypeId = 0;													\
		return reinterpret_cast<size_t>(&runTimeTypeId);								\
	}																					\
	/* 특정 타입 ID와 일치하는지 확인, 상속 계층 따라 검사 */							\
	virtual bool Is(const size_t id) const												\
	{																					\
		if (id == TypeIdClass())														\
		{																				\
			return true;																\
		}																				\
		else																			\
		{																				\
            /* 상위 클래스의 Is 호출하여 상속 계층 확인 */								\
			return ParentType::Is(id);													\
		}																				\
	}