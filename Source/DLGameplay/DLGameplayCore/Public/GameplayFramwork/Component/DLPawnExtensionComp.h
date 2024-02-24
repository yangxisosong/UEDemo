#pragma once

#include "CoreMinimal.h"
#include "DLPawnExtensionComp.generated.h"


class UDLUnitAbilitySystemComponent;


UCLASS()
class UDLPawnExtensionComp : public UActorComponent
{
	GENERATED_BODY()
public:

	void OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
	{
		if (!OnPawnReadyToInitialize.IsBoundToObject(Delegate.GetUObject()))
		{
			OnPawnReadyToInitialize.Add(Delegate);
		}

		if (bPawnReadyToInitialize)
		{
			Delegate.Execute();
		}
	}


	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
	{
		if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
		{
			OnAbilitySystemInitialized.Add(Delegate);
		}

		if (AbilitySystemComponent)
		{
			Delegate.Execute();
		}
	}
	
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
	{
		if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
		{
			OnAbilitySystemUninitialized.Add(Delegate);
		}
	}


	void OnLoadAssetComplate()
	{
		
	}

	void OnPawnControllerChanged()
	{
		
	}

	void OnPlayerStateReplicated()
	{
		
	}

	bool IsPawnReadyToInit() const
	{
		return bPawnReadyToInitialize;
	}


private:


	bool CheckPawnReadyToInitialize();

private:


	FSimpleMulticastDelegate OnPawnReadyToInitialize;
	
	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	bool bPawnReadyToInitialize = false;


	UPROPERTY()
	UDLUnitAbilitySystemComponent* AbilitySystemComponent = nullptr;
};
