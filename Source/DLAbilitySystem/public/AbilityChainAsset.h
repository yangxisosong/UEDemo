#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "AbilityChainAsset.generated.h"


/**
 * @brief 技能连招资产，用来桥接  Editor 与 Runtime 
 */
UCLASS()
class DLABILITYSYSTEM_API UAbilityChainAsset : public UObject
{
	GENERATED_BODY()
public:

	/**
	* @brief 存储技能连招数据，由编辑器生成后设置，并且会被存盘
	*/
	UPROPERTY()
		FString	AbilityChainData;


#if WITH_EDITORONLY_DATA

	/**
	 * @brief 用来过滤编辑器可以支持选择那些目录下的 Ability
	 */
	UPROPERTY(EditAnywhere, meta = (RelativeToGameContentDir, LongPackageName))
		TArray<FDirectoryPath> AbilityDirectoryArray;

	UPROPERTY()
		UEdGraph* EdGraph;
#endif


};
