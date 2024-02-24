#pragma once
#include "CoreMinimal.h"
#include "DLGameplayPredictTaskTypes.generated.h"


USTRUCT()
struct FGameplayPredictTaskId
{
	GENERATED_BODY()

public:

	UPROPERTY()
		FGuid Id;

	static FGameplayPredictTaskId InvalidPredictTaskId;

	bool operator==(const FGameplayPredictTaskId& TaskId) const
	{
		return TaskId.Id == Id;
	}

	FString ToString() const
	{
		return Id.ToString();
	}

	static FGameplayPredictTaskId Create()
	{
		FGameplayPredictTaskId Data;
		Data.Id = FGuid::NewGuid();
		return Data;
	}
};

inline bool IsValidPredictTaskId(const FGameplayPredictTaskId& Id)
{
	return Id.Id.IsValid();
}


using FDLGameplayPredictTaskContextTypeId = uint32;

#define PREDICT_TASK_ID_DEF(Class)			\
				virtual FDLGameplayPredictTaskContextTypeId GetTypeId() const							\
				{							\
					return GetTypeHash(FString(#Class)); \
				}	\
				static Class* GetDefault() { static Class D; return &D; };


class UDLGameplayPredictTask;



USTRUCT()
struct FDLGameplayPredictTaskContextBase
{
	GENERATED_BODY()

public:


	virtual ~FDLGameplayPredictTaskContextBase() = default;



	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << TaskClass;
		bOutSuccess = true;
		return true;
	}

	virtual FString ToString() const
	{
		return FString::Printf(TEXT("TypeId:%d, TaskClass: %s"), GetTypeId(), TaskClass ? *(TaskClass->GetFName().ToString()) : TEXT("None"));
	}


	PREDICT_TASK_ID_DEF(FDLGameplayPredictTaskContextBase)

public:

	UPROPERTY()
		UClass* TaskClass;
};

template<typename T>
T* CastPredictTaskContext(const FDLGameplayPredictTaskContextBase* Data)
{
	if (T::GetDefault()->GetTypeId() == Data->GetTypeId())
	{
		return static_cast<T*>(const_cast<FDLGameplayPredictTaskContextBase*>(Data));
	}
	return nullptr;
}


DLGAMEPLAYPREDICTIONTASK_API FDLGameplayPredictTaskContextBase* CreatePredictTaskContext(FDLGameplayPredictTaskContextTypeId TypeId);


DLGAMEPLAYPREDICTIONTASK_API void RegisterPredictTaskContextFactor(
									FDLGameplayPredictTaskContextTypeId Id,
									TFunction<FDLGameplayPredictTaskContextBase* (FDLGameplayPredictTaskContextTypeId Id)> Factor);