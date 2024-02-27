#pragma once

#include "CoreMinimal.h"
#include "AbilityChainAsset2.h"
#include "EdGraph/EdGraphNode.h"
#include "AbilityChainNodeEdGraphNode.generated.h"

UENUM()
enum class EAbilityChainNodeType
{
	None,
	RootNode,
	Normal
};

DECLARE_DELEGATE(FOnUpdateSubNode);

UCLASS()
class UAbilityChainNodeEdGraphNode : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()

public:
	virtual void AllocateDefaultPins() override;
	virtual void PostCopyNode();
	virtual void NodeConnectionListChanged() override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	void RemoveSubNode(UAbilityChainNodeEdGraphNode* Node);

	void FindNodePath(const FString& Str) const;

	UFUNCTION()
		TArray<FString> GetSelectOptions() const;
public:
	UPROPERTY()
	FString TitleName;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "NodeType != EAbilityChainNodeType::RootNode"))
	EPrimaryInputID2 InputIDInfo;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "NodeType != EAbilityChainNodeType::RootNode", GetOptions = "GetSelectOptions"))
	TArray<FName>AbilityArrayInfo;

	UPROPERTY()
	UEdGraph* Graph;

	UPROPERTY()
		UAbilityChainNodeEdGraphNode* ParentNode;

	UPROPERTY()
		TArray<UAbilityChainNodeEdGraphNode*> SubNodes;

	UPROPERTY()
		UObject* NodeInstance;

	UPROPERTY()
	EAbilityChainNodeType NodeType = EAbilityChainNodeType::Normal;

	FOnUpdateSubNode OnUpdateSubNode;

	//输出引脚
	UEdGraphPin* OutputPin;

	UEdGraphPin* InputPin;

private:

};