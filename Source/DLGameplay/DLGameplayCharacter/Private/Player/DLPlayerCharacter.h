#pragma once

#include "CoreMinimal.h"
#include "VirtualCam.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "Interface/IDLVirtualCameraController.h"
#include "DLPlayerCharacter.generated.h"


class UDLPlayerAbilitySysComponent;
class UDLPlayerCharacterAsset;
UCLASS()
class DLGAMEPLAYCHARACTER_API ADLPlayerCharacter
	: public ADLCharacterBase
	, public IDLVirtualCameraController
	, public IUnitSceneInfoInterface
{
	GENERATED_BODY()

public:

	explicit ADLPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess))
		class UVCSpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Transient, Category = Camera, meta = (AllowPrivateAccess))
		TMap<FGameplayTag, UVirtualCam*> VirtualCamMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float CurrentMoveSpeedCoefficient = 1.f;

public:

#pragma region IUnitSceneInfoInterface

	virtual FName GetCurrentScene() override;

	virtual void SetEnterNewScene(FName SceneId) override;

#pragma endregion


protected:

	UFUNCTION(BlueprintNativeEvent)
		bool LoadDataTable(FDTPlayerCharacterInfoRow& Out);

protected:

	virtual float CalculateMaxSpeed(const float BaseSpeed, const float Scale) override;

	virtual void OnInitCharacter(const FCharacterInfoBase& InBaseInfo) override;

	virtual void OnUninitCharacter() override;

	virtual void UpdateLockTargetState() override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

	virtual void BeginPlay() override;

	virtual void OnPlayerStateChange(const FGameplayTagContainer& AllState, const FGameplayTag& Tag, bool IsAdd) override;

	virtual void OnUpdateMoveInfo(const FCharacterMovementInfo& Info) override;

	virtual const FDTCharacterInfoBaseRow* GetBaseDataTableInfo(bool EnsureNotData) override;

#pragma region IDLVirtualCameraController

	virtual void ActiveVirtualCamera(const FGameplayTag& VirtualCameraID,
										TArray<UVCDataAssetDef*> DataArray,
										const EVCTimeOutAction InTimeOutAction) override;

	virtual void StopVirtualCamera(const FGameplayTag& VirtualCameraID) override;

	bool BisMouseMoved = false;
	virtual void SetIsMouseMoved(const bool BisMoved) override;
	virtual bool CheckIsMouseMoved() override;
	
	virtual USceneComponent* GetDLSpringArm() override;

#pragma endregion

	UFUNCTION(BlueprintImplementableEvent)
	void OnCamLocked();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnCamUnLocked();

public:

	const FDTPlayerCharacterInfoRow& GetDataTable();

	void SetCurrentMoveSpeedCoefficient(float Coefficient);

	const UDLPlayerCharacterAsset* GetPlayerAsset() const;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UDLPlayerAbilitySysComponent* GetPlayerASC() const;

private:

	UPROPERTY()
		FDTPlayerCharacterInfoRow CharacterDataTable;

	UPROPERTY()
		FGameplayTag VCMoveStateActive;

	bool bIsInitDataTable = false;
};
