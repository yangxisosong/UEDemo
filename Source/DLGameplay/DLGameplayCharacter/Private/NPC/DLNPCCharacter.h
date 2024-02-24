// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "GameFramework/Character.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "Interface/IDLCharacterParts.h"
#include "DLNPCCharacter.generated.h"

enum class EDLCharacterParts : uint8;

UCLASS()
class DLGAMEPLAYCHARACTER_API ADLNPCCharacter
	: public ADLCharacterBase
	, public IDLCharacterParts
{
	GENERATED_BODY()

public:

	explicit ADLNPCCharacter(const class FObjectInitializer& ObjectInitializer);


protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


#pragma region IDLLockableUnit
	virtual const FLockableUnitConfig& GetLockableUnitConfig() const override;
#pragma endregion

#pragma region IDLCharacterParts

	virtual FGameplayTag GetPartByBodyName(FName BodyName) override;

	virtual FGameplayTag GetPartByBodyIndex(int32 Index)override;

	virtual FName GetBodyNameByIndex(int32 Index) override;

	virtual int32 GetPartAttrIndexByTag(const FGameplayTag& PartTag) override;

	virtual int32 GetDefaultPartAttrIndex()const override;

#pragma endregion

	virtual float CalculateMaxSpeed(const float BaseSpeed, const float Scale) override;

	virtual void OnInitCharacter(const FCharacterInfoBase& InBaseInfo) override;

	virtual void OnUninitCharacter() override;

	virtual void OnUpdateMoveInfo(const FCharacterMovementInfo& Info) override;

	virtual const FDTCharacterInfoBaseRow* GetBaseDataTableInfo(bool EnsureNotData) override;

	// 开始 AI 相关方法
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool GetAISpeed(float& Speed);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float AISpeed = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FGameplayTag CurrentStageTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FGameplayTag CurrentMovementTag;

	UFUNCTION(BlueprintCallable)
		void SwitchBattleStageTag(FGameplayTag NewStageTag);

	UFUNCTION(BlueprintCallable)
	void SwitchMovementTag(FGameplayTag NewMovementTag);
	
	UFUNCTION(NetMulticast, Reliable,BlueprintCallable)
	void NetMulticast_ChangeTag(const FGameplayTagContainer InRemoveGameplayTags,const FGameplayTagContainer AddGameplayTag);

	UFUNCTION(NetMulticast, Reliable,BlueprintCallable)
	void NetMulticast_SwitchMesh(USkeletalMesh* NewMesh,bool bReinitPose);
	
	// 结束 AI 相关方法

private:


	UPROPERTY()
		FDTNPCCharacterInfoRow DTInfo;

	bool bIsInitDT = false;

	TMap<FName, FGameplayTag> BodyNameMapParts;

	TOptional<FLockableUnitConfig> LockableUnitConfig;

public:

	UPROPERTY(EditAnywhere)
		FName NPCCharacterId;
};
