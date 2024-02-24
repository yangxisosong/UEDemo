#pragma once

#include "CoreMinimal.h"

/**
 * @brief UE 结构体的 Cast，只有类型一致才能 Cast 成功
 * @tparam T 转换后的类型
 * @tparam S 转换前的类型
 * @param Ptr 转换前的指针
 * @return
 * @note 结构体需要实现一个  GetScriptStruct  的成员函数
 */
template<typename T, typename S>
T* StructCast(S* Ptr)
{
	if (T::StaticStruct() == Ptr->GetScriptStruct())
	{
		// ReSharper disable once CppCStyleCast
		return (T*)(Ptr);
	}
	return nullptr;
}


