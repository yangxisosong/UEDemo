#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "NativeGameplayTags.h"
#include "GameFramework/Character.h"
#include "DLEnterSceneTrigger.generated.h"


namespace EDLGameplayTriggerMsg
{
	GAMEPLAYSCENE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayTrigger_UnitEnterNewScene);
}

USTRUCT(BlueprintType)
struct FDLUnitEnterNewSceneMsgBody
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TWeakObjectPtr<AActor> UnitActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName PreviousSceneId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName CurrentSceneId;

public:

	bool IsLocalPlayerCharacter() const
	{
		if (UnitActor.IsValid())
		{
			if (Cast<ACharacter>(UnitActor.Get()))
			{
				return UnitActor->GetLocalRole() == ENetRole::ROLE_AutonomousProxy;
			}
		}
		return false;
	}
};


UCLASS()
class GAMEPLAYSCENE_API ADLEnterSceneTrigger
	: public ATriggerBox
{
	GENERATED_BODY()
public:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:

	UPROPERTY(EditInstanceOnly)
		FName SceneId;
};