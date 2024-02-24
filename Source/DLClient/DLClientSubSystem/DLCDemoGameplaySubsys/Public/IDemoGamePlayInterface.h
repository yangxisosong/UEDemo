#pragma once
#include "CoreMinimal.h"
#include "DLUITriggerEventDef.h"
#include "GameplayTags/Classes/GameplayTagContainer.h"
#include "IDemoGamePlayInterface.generated.h"


USTRUCT(BlueprintType)
struct FPlayerBaseInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CurrentHP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CurrentMP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PS = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CurrentPS = 0;
};

USTRUCT(BlueprintType)
struct FPlayerAbilityInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName AbilityId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText AbilityNameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText AbilityDescText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTag AbilityUISlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CDDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText AbilityKeystrokes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture>  AbilityIcon;
};

USTRUCT(BlueprintType)
struct FBossBaseInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText BossName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 HP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CurrentHP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CurrentMP = 0;
};

class UDLDemoGamePlaySubsystem;
//战斗系统的业务接口
UINTERFACE()
class DLCDEMOGAMEPLAYSUBSYS_API UDLDemoGamePlayInterface : public UInterface
{
	GENERATED_BODY()
};

class DLCDEMOGAMEPLAYSUBSYS_API IDLDemoGamePlayInterface : public IInterface
{
	GENERATED_BODY()
public:

	virtual void InitCurrentSubsystem(UDLDemoGamePlaySubsystem* Subsystem) = 0;

	virtual void InitPlayerBaseInfo(const FPlayerBaseInfo& PlayerBaseInfo) = 0;

	virtual void UpdatePlayerBaseInfo(const FPlayerBaseInfo& PlayerBaseInfo) = 0;

	virtual void InitPlayerAbilityInfo(const TArray<FPlayerAbilityInfo>& PlayerAbilityInfo) = 0;

	virtual void PlayerAbilityCDChange(const FPlayerAbilityInfo PlayerAbilityInfo) = 0;

	virtual void InitBossBaseInfo(const FBossBaseInfo& PlayerAbilityInfo) = 0;

	virtual void UpdateBossBaseInfo(const FBossBaseInfo PlayerAbilityInfo) = 0;

	virtual void SetButtonTipsShow(bool IsShow, const FDLUIShowButtonTipsMsgBody& Msg) = 0;

	virtual void KeyClickEvent(FKey Key) = 0;

	virtual void PlayerBeginBattle() = 0;

	virtual void PlayerQuiteBattle() = 0;

	virtual void PlayerDied() = 0;

	virtual void PlayerRebirth() = 0;

	virtual void PlayerEnterScene(const FText& SceneName) = 0;

	virtual void PlayerBattleVictory() = 0;

	//virtual void SetShowBossInfo(bool IsShow) = 0;
};
