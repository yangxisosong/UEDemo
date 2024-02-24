#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DLPlayerInputCmd.h"
#include "ASC/DLPlayerAbilitySysComponent.h"
#include "LocalPlayerAbilityInputCmd.generated.h"



UCLASS(Config = Game)
class DLABILITYSYSTEM_API UDLInputCmdAbility : public UDLPlayerInputCmdBase
{
	GENERATED_BODY()
public:

	virtual bool Exec(const FDLPlayerInputCmdExecContext& Context) override
	{
		const IAbilitySystemInterface* Interface =  Cast<IAbilitySystemInterface>(Context.LocalCharacter);
		if (Interface)
		{
			const auto ASC = Cast<UDLPlayerAbilitySysComponent>(Interface->GetAbilitySystemComponent());
			if (ensureAlwaysMsgf(ASC, TEXT("这里一定能够获取到正确的  ASC  DebugSts(%s)"), *Context.ContextDebugStr))
			{
				return  ASC->InjectInputActiveAbility(AbstractKeyID, GetContextObject());
			}
		}

		return false;
	}

private:

	// 技能的抽象按键
	UPROPERTY()
	int32 AbstractKeyID = INDEX_NONE;
};
