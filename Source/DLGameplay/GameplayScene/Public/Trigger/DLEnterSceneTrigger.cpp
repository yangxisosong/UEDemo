#include "DLEnterSceneTrigger.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Interface/IUnitSceneInfoInterface.h"


namespace EDLGameplayTriggerMsg
{
	UE_DEFINE_GAMEPLAY_TAG(GameplayTrigger_UnitEnterNewScene, "DLMsgType.GameplayTrigger.UnitEnterNewScene");
}

void ADLEnterSceneTrigger::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (IUnitSceneInfoInterface* Interface = Cast<IUnitSceneInfoInterface>(OtherActor))
	{
		if (Interface->GetCurrentScene() != SceneId)
		{
			const FName PreSceneId = Interface->GetCurrentScene();
			Interface->SetEnterNewScene(SceneId);

			FDLUnitEnterNewSceneMsgBody Body;
			Body.CurrentSceneId = SceneId;
			Body.PreviousSceneId = PreSceneId;
			Body.UnitActor = OtherActor;

			auto& MsgSys = UGameplayMessageSubsystem::Get(this);
			MsgSys.BroadcastMessage(EDLGameplayTriggerMsg::GameplayTrigger_UnitEnterNewScene, Body);
		}
	}

}
