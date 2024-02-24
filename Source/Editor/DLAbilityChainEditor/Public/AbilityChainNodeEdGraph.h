#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "AbilityChainNodeEdGraph.generated.h"

class UAbilityChainNodeEdGraphNode;

UCLASS()
class UAbilityChainNodeEdGraph : public UEdGraph
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY()
		UAbilityChainNodeEdGraphNode* RootNode;

	TMap<FName, TWeakObjectPtr<UObject>> AbilityNameMap;

	TArray<FDirectoryPath> AbilityDirectoryArray;

	TArray<FString> NodeSelectCache;

public:
	TArray<FString> GetNodeSelectOptions();

	void UpdateAbilityAssetCache();

	void RebuildGraph();

	//自动布局
	void AutoArrange() const;

	//防止出现已被删除资源名称
	void CheckNodeInfo();

	static void RsetNodePin(UAbilityChainNodeEdGraphNode* Node);

	UAbilityChainNodeEdGraphNode* CreateGraphNode();

	UAbilityChainNodeEdGraphNode* CreateGraphRootNode();

private:

};