/******************************************************************************
 * Core.h
 *
 * 게임 엔진의 핵심 공통 헤더 파일
 *
 * 역할:
 * 1. DLL export/import 정의
 *    - Engine_API 매크로를 통해 DLL 내보내기/가져오기 지원
 * 2. 메모리 디버깅 지원
 *    - _CRTDBG_MAP_ALLOC, crtdbg.h 포함
 *    - new 매크로 재정의로 할당 위치 추적 가능
 * 3. 메모리 안전 삭제 유틸리티
 *    - SafeDelete / SafeDeleteArray: nullptr로 초기화하며 delete 수행
 *
 ******************************************************************************/
#pragma once

// dll 외부로 템플릿 내보낼 때 발생하는 경고 비활성화.
#pragma warning(disable: 4251)
// 지역 변수의 주소를 반환할 때 발생하는 경고 비활성화.
#pragma warning(disable: 4172)

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#if BuildEngineDLL
#define Engine_API __declspec(dllexport)
#else
#define Engine_API __declspec(dllimport)
#endif

#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define new new
#endif

// 메모리 정리 함수.
template<typename T>
void SafeDelete(T*& target)
{
	if (target)
	{
		delete target;
		target = nullptr;
	}
}

template<typename T>
void SafeDeleteArray(T*& target)
{
	if (target)
	{
		delete[] target;
		target = nullptr;
	}
}