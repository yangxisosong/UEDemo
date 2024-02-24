#pragma once
#include "CoreMinimal.h"
#include "IUnitSceneInfoInterface.generated.h"

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint), MinimalAPI)
class UUnitSceneInfoInterface : public UInterface
{
	GENERATED_BODY()
};

class DLGAMEPLAYTYPES_API IUnitSceneInfoInterface : public IInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
		virtual FName GetCurrentScene() { return FName{}; }


	UFUNCTION(BlueprintCallable)
		virtual void SetEnterNewScene(FName SceneId) { };

};


