#pragma once
#include "CoreMinimal.h"
#include "JsonUtilities/Public/JsonUtilities.h"

inline const FString& ToString(const bool Value)
{
	static FString True(TEXT("True"));
	static FString False(TEXT("False"));
	return Value ? True : False;
}

inline FString ToString(const UClass* Class)
{
	return Class->GetPathName();
}

inline FString ToString(const UObject* Value)
{
	return GetNameSafe(Value);
}

inline FString GetNetRoleString(const AActor* Value)
{
	if (Value)
	{
		return StaticEnum<ENetRole>()->GetNameStringByValue(Value->GetLocalRole());
	}
	return TEXT("None");
}

template <typename Enumeration>
FORCEINLINE FString GetEnumerationToString(const Enumeration InValue)
{
	return StaticEnum<Enumeration>()->GetNameStringByValue(static_cast<int64>(InValue));
}

template<typename _StructType>
FORCEINLINE FString ConvertUStructToJson(const _StructType& InStruct)
{
	FString Result;
	if (!FJsonObjectConverter::UStructToJsonObjectString<_StructType>(InStruct, Result))
	{
		UE_LOG(LogTemp, Error, TEXT("UDLAnimationLibrary::ConvertUStructToJson failed!!"));
	}
	return Result;
}

template<typename _StructType>
FORCEINLINE bool ConvertJsonToUStruct(const FString& JsonStr, _StructType& InStruct)
{
	if (FJsonObjectConverter::JsonObjectStringToUStruct<_StructType>(JsonStr, &InStruct))
	{
		UE_LOG(LogTemp, Error, TEXT("UDLAnimationLibrary::ConvertJsonToUStruct failed!!,jsonstr:%s"), *JsonStr);
		return true;
	}
	InStruct = _StructType();
	return false;
}

#define ENUM_TO_STR(Enum, Value) *StaticEnum<Enum>()->GetNameStringByValue((int64)Value)

#define ENUM_TO_STRING(Value) GetEnumerationToString(Value)

#define TO_STR(Value) *ToString(Value)

#define NET_ROLE_STR(Actor) *GetNetRoleString(Actor)

#define USTRUCT_TO_STR(InStruct) ConvertUStructToJson(InStruct)