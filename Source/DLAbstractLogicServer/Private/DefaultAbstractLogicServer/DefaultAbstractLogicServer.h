#pragma once

#include "CoreMinimal.h"
#include "IDLAbstractLogicServerInterface.h"
#include "DefaultAbstractLogicServer.generated.h"


UCLASS()
class UDefaultAbsLogicServer
	: public UGameInstanceSubsystem
	, public IDLAbstractLogicServerInterface
{
	GENERATED_BODY()
public:

	virtual void Deinitialize() override { }
	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		FString ConfigStr;
		if(FFileHelper::LoadFileToString(ConfigStr, *(FPaths::ProjectSavedDir() / TEXT("DefaultLogicServerConfig.json"))))
		{
			Config.FromJson(ConfigStr);
			UE_LOG(LogTemp, Log, TEXT("Load LogicServer Conifg \n%s"), *ConfigStr);
		}
	}
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		const auto GI = Cast<UGameInstance>(Outer);
		if(GI)
		{
			return  GI->IsDedicatedServerInstance();
		}
		return false;
	}

protected:

	virtual void GameplayServerLoadComplate(const FString& ErrStr) override
	{
		UE_LOG(LogTemp, Log, TEXT("========== GameplayServerLoadComplate =========="));
	}
	                
	virtual void GameplayServerSetupReady() override
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		OnRecvGameplayInfo().ExecuteIfBound(Config);
	}

	virtual FOnRecvGameplayInfo& OnRecvGameplayInfo() override
	{
		return OnRecvGameplayInfoDel;
	}

private:

	FDLGameplayInfo Config;

	FOnRecvGameplayInfo OnRecvGameplayInfoDel;
};



