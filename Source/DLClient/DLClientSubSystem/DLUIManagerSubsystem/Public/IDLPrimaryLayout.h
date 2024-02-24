#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NativeGameplayTags.h"
#include "Components/Widget.h"
#include "IDLPrimaryLayout.generated.h"

//DLUIMANAGERSUBSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layout_GameNormal);
//DLUIMANAGERSUBSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layout_GameMenu);
//DLUIMANAGERSUBSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layout_Loading);
//DLUIMANAGERSUBSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layout_Modal);

/**
 * @brief 游戏的主布局接口，其他的 UI 都是通过这个接口挂载在布局上的
 */
UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class DLUIMANAGERSUBSYSTEM_API UDLPrimaryLayout : public UInterface
{
	GENERATED_BODY()
};

class DLUIMANAGERSUBSYSTEM_API IDLPrimaryLayout
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
		virtual UUserWidget* AddWidgetToLayout(FGameplayTag LayoutTag, TSubclassOf<UUserWidget> WidgetClass) = 0;

	UFUNCTION(BlueprintCallable)
		virtual void RemoveWidgetToLayout(FGameplayTag LayoutTag, UUserWidget* WidgetIns) = 0;
};
