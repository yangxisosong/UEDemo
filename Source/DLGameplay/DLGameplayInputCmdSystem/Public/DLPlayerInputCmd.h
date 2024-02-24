#pragma once


#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DLPlayerInputCmd.generated.h"

USTRUCT(BlueprintType)
struct FDLPlayerInputCmdExecContext
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
		APawn* LocalCharacter = nullptr;

	UPROPERTY(BlueprintReadWrite)
		APlayerController* LocalController = nullptr;

	UPROPERTY(BlueprintReadWrite)
		FString ContextDebugStr;
};


USTRUCT(BlueprintType)
struct FDLPlayerInputCmdConstructBase
{
	GENERATED_BODY()
public:

	virtual ~FDLPlayerInputCmdConstructBase() = default;

	virtual UScriptStruct* GetScriptStruct() const
	{
		return FDLPlayerInputCmdConstructBase::StaticStruct();
	}
};


USTRUCT(BlueprintType)
struct FDLPlayerInputCmdConstructAbility : public FDLPlayerInputCmdConstructBase
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 AbstractKeyID = INDEX_NONE;


protected:

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FDLPlayerInputCmdConstructAbility::StaticStruct();
	}
};



USTRUCT(BlueprintType)
struct FDLPlayerInputCmdConstructRoll : public FDLPlayerInputCmdConstructBase
{
	GENERATED_BODY()
public:

	// 输入的朝向
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator InputRollRotation = FRotator::ZeroRotator;

	// 输入的朝向  原始的
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator InputOrientationRotation = FRotator::ZeroRotator;

protected:

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FDLPlayerInputCmdConstructRoll::StaticStruct();
	}
};


USTRUCT(BlueprintType)
struct FDLPlayerInputCmdConstructJump : public FDLPlayerInputCmdConstructBase
{
	GENERATED_BODY()

protected:

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FDLPlayerInputCmdConstructJump::StaticStruct();
	}
};


USTRUCT(BlueprintType)
struct FDLPlayerInputCmdConstructMovement : public FDLPlayerInputCmdConstructBase
{
	GENERATED_BODY()

public:

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FDLPlayerInputCmdConstructMovement::StaticStruct();
	}

	UPROPERTY(BlueprintReadWrite)
		FVector InputVector = FVector::ZeroVector;
};


UCLASS(Config = Game)
class DLGAMEPLAYINPUTCMDSYSTEM_API UDLPlayerInputCmdBase : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
		FGameplayTagContainer CmdTags;

public:

	FString ToString()const
	{
		return this->GetFName().ToString();
	}

public:

	virtual bool Exec(const FDLPlayerInputCmdExecContext& Context) { return false; };

	virtual void InitFinish() {};

	UFUNCTION(BlueprintCallable)
		void SetContextObject(UObject* Obj)
	{
		ContextObj = Obj;
	}

	UFUNCTION(BlueprintCallable)
		UObject* GetContextObject() const
	{
		return ContextObj.Get();
	}

	bool IsIdle()const
	{
		return bIsIdle;
	}

	void SetActive(const bool IsActive)
	{
		bIsIdle = !IsActive;
		if (bIsIdle)
		{
			ContextObj = nullptr;
		}
	}

private:

	bool bIsIdle = true;

	TWeakObjectPtr<UObject> ContextObj;
};