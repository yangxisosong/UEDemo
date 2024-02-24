// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICharacterStateListener.h"
#include "NativeGameplayTags.h"
#include "UObject/Interface.h"
#include "IDLLocalPlayerGameplayInterface.generated.h"


namespace EDLGameplayMsgType
{
	// 进入战斗状态
	DLGAMEPLAYTYPES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LocalPlayer_Gameplay_InBattleState);

	// 离开战斗状态
	DLGAMEPLAYTYPES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LocalPlayer_Gameplay_OutBattleState);

	// 玩家死亡
	DLGAMEPLAYTYPES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LocalPlayer_Gameplay_PlayerDied);

	// 玩家出生
	DLGAMEPLAYTYPES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LocalPlayer_Gameplay_PlayerStart);

	// 进入新场景  FDLLocalPlayerEnterNewSceneMsgBody
	DLGAMEPLAYTYPES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LocalPlayer_Gameplay_EnterNewScene);
	 
	// 玩家有新的目标  FDLLocalPlayerFocusNewTargetMsgBody
	DLGAMEPLAYTYPES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LocalPlayer_Gameplay_FocusNewTarget);

	// 玩家丢失目标  FDLGameplayMsgBody
	DLGAMEPLAYTYPES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LocalPlayer_Gameplay_LoseTarget);
};


// Type  LocalPlayer_Gameplay_EnterNewScene
USTRUCT(BlueprintType)
struct FDLLocalPlayerEnterNewSceneMsgBody
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName SceneId;
};


// Type  LocalPlayer_Gameplay_FocusNewTarget
USTRUCT(BlueprintType)
struct FDLLocalPlayerFocusNewTargetMsgBody
{
	GENERATED_BODY()
public:

	UPROPERTY()
		TWeakObjectPtr<AActor> FocusTarget;

	UPROPERTY()
		TScriptInterface<ICharacterStateListener> StateListener;

	UPROPERTY()
		TScriptInterface<ICharacterStateAccessor> StateAccessor;
};


USTRUCT(BlueprintType)
struct FDLGameplayMsgBody
{
	GENERATED_BODY()
};



class IDLPrimaryPlayerAbilitySystem;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UDLLocalPlayerGameplayInterface : public UInterface
{
	GENERATED_BODY()
};


class DLGAMEPLAYTYPES_API IDLLocalPlayerGameplayInterface
{
	GENERATED_BODY()

public:

	virtual void OnLocalPlayerGameplayReady_CallOrRegister(const FSimpleMulticastDelegate::FDelegate& Delegate) = 0;

	virtual void OnLocalPlayerGameplayReady_Unregister(UObject* BindObject) = 0;

	virtual void OnPlayerAbilitySysReady_CallOrRegister(const FSimpleMulticastDelegate::FDelegate& Delegate) = 0;

	virtual void OnPlayerAbilitySysReady_Unregister(UObject* BindObject) = 0;

	virtual bool IsGameplayReady() = 0;

	virtual bool IsInBattleState() = 0;

	virtual bool HasEnemyUnitFocus() = 0;

	virtual TScriptInterface<ICharacterStateListener> GetPrimaryPlayerStateListener() = 0;

	virtual TScriptInterface<ICharacterStateAccessor> GetPrimaryPlayerStateAccessor() = 0;

	virtual TScriptInterface<IDLPrimaryPlayerAbilitySystem> GetPrimaryPlayerAbilitySystem() = 0;

	virtual TScriptInterface<ICharacterStateAccessor> GetEnemyUnitStateAccessor() = 0;

	virtual TScriptInterface<ICharacterStateListener> GetEnemyUnitStateListener() = 0;

	// 复活玩家
	virtual void PlayerReborn() = 0;
};
