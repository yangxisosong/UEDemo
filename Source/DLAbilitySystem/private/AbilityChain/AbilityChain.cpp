#include "AbilityChain.h"

#include "AbilityChainAsset.h"
#include "DLKit/Public/ConvertString.h"
#include "DLGameplayAbilityBase.h"
#include "DLAbilityLog.h"

const FAbilityChainInputKey FAbilityChainInputKey::None = FAbilityChainInputKey{};

TSharedPtr<FAbilityChainNodeDefine> UAbilityChain::SearchMatchAbilityChainNode(
	const TArray<FAbilityChainInputKey>& InputIdList) const
{
	TArray<TSharedPtr<FAbilityChainNodeDefine>> Queue;
	TArray<TSharedPtr<FAbilityChainNodeDefine>> TempQueue;

	Queue.Add(AbilityRoot);

	const int32 InputIdListSize = InputIdList.Num();

	int32 CurrentIndex = 0;

	TSharedPtr<FAbilityChainNodeDefine> Ret;

	while (Queue.Num() > 0)
	{
		if (CurrentIndex >= InputIdListSize)
		{
			Ret = Queue[0];
			break;
		}

		const auto& InputKey = InputIdList[CurrentIndex];

		while (Queue.Num() > 0)
		{
			const auto& NodeList = Queue[0]->ChildNodes;

			for (const auto& Node : NodeList)
			{
				// 遍历找到了
				if (Node->InputTags == InputKey)
				{
					TempQueue.Add(Node);
				}
			}

			Queue.RemoveAtSwap(0);
		}

		CurrentIndex++;

		Swap(Queue, TempQueue);
	}

	return Ret;
}

void UAbilityChain::InjectInput(const FAbilityChainInputKey& InputKeys)
{
	if (EnabledAcceptInput)
	{
		CurrentInputKey = InputKeys;
	}
	else
	{
		CurrentInputKey = FAbilityChainInputKey::None;
	}
}



TSubclassOf<UDLGameplayAbilityBase> UAbilityChain::CheckoutAlternateAbility(const FCheckOutAbilityArg& Arg)
{
	if (!ensureAlwaysMsgf(Arg.ASC.IsValid(), TEXT("传入一个无效的ASC ?")))
	{
		return nullptr;
	}

	if (CurrentInputKey == FAbilityChainInputKey::None)
	{
		return nullptr;
	}

	UE_LOG(LogDLAbilityChain, Log, TEXT("CheckoutAlternateAbility"));

	if (!EvaluateContext)
	{
		EvaluateContext = MakeUnique<FAbilityChainEvaluateContext>();
	}


	static TArray<FAbilityChainInputKey> EvaluateInputList;
	EvaluateInputList.Empty();

	EvaluateInputList.Append(EvaluateContext->InputIdList);
	EvaluateInputList.Add(CurrentInputKey);

	FString DebugStrInputList;
	for (const auto& InputID : EvaluateInputList)
	{
		DebugStrInputList += (FString(TEXT("->")) + InputID.ToString());
	}

	UE_LOG(LogDLAbilityChain, Log, TEXT("InputList : %s"), *DebugStrInputList);

	const auto Node = this->SearchMatchAbilityChainNode(EvaluateInputList);

	// 如果搜索到了，进一步考虑应该放那个技能
	if (Node)
	{
		//评估这个技能可不可以释放
		const FGameplayAbilityActorInfo* ActorInfo = Arg.ASC->AbilityActorInfo.Get();

		TSubclassOf<UDLGameplayAbilityBase> RetClass;

		for (const FAlternateAbilityDefine& AlternateAbility : Node->AlternateAbilityList)
		{
			const auto AbilityClass = AlternateAbility.AbilityClass.LoadSynchronous();

			if (!AbilityClass)
			{
				UE_LOG(LogDLAbilityChain, Warning, TEXT("加载备选技能资产失败，是否是忘记配置了 ? "));
				continue;
			}

			const auto Spec = Arg.ASC->FindAbilitySpecFromClass(AbilityClass);
			if (Spec)
			{
				const auto AbilityIns = Spec->GetPrimaryInstance();
				if (ensureAlwaysMsgf(AbilityIns, TEXT("一定能找到一个实例 否则就是配错了 技能")))
				{
					// 加载 CD 、 Block 、 Cost
					if (Spec->GetPrimaryInstance()->CanActivateAbility(Spec->Handle, ActorInfo, Arg.SourceTags, Arg.TargetTags))
					{
						RetClass = AbilityClass;
						break;
					}
					else
					{
						UE_LOG(LogDLAbilityChain, Warning, TEXT("这个技能不能被释放  %s,  没有满足释放条件，正在尝试其他备选技能"), TO_STR(AbilityClass));
						continue;
					}
				}
			}
			else
			{
				UE_LOG(LogDLAbilityChain, Warning, TEXT("玩家还没有拥有连招表中配置的技能  %s"), TO_STR(AbilityClass));
			}
		}

		if (RetClass)
		{
			UE_LOG(LogDLAbilityChain, Log, TEXT("Find a Ability %s"), *RetClass->GetPathName());

			//记录输入ID 
			EvaluateContext->InputIdList.Add(CurrentInputKey);
			LastInputKey = CurrentInputKey;
			CurrentInputKey = FAbilityChainInputKey::None;
			return RetClass;
		}
	}

	// 如果之前存在有效的输入，那么可能是 连招断了 的从头来
	if (EvaluateContext->InputIdList.Num() > 0)
	{
		EvaluateContext = nullptr;

		UE_LOG(LogDLAbilityChain, Log, TEXT("连招断了 的从头来"));

		return this->CheckoutAlternateAbility(Arg);
	}

	UE_LOG(LogDLAbilityChain, Log, TEXT("没有符合的 GA"));

	// 如果有效的输入完全没有，那么就确实没有符合的 GA
	return nullptr;
}

void UAbilityChain::CommitCheckOutAbility(TSubclassOf<UDLGameplayAbilityBase> AbilityClass)
{
	UE_LOG(LogDLAbilityChain, Log, TEXT("CommitCheckOutAbility %s"), TO_STR(AbilityClass));
	CheckOutAbilityList.Add(AbilityClass);
}

void UAbilityChain::AckCheckOutAbility(TSubclassOf<UDLGameplayAbilityBase> AbilityClass)
{
	// 如果检出的技能 ACK 与当前的技能一致  说明中途没有继续检出技能
	if (CheckOutAbilityList.Num() == 1 && CheckOutAbilityList[0] == AbilityClass)
	{
		UE_LOG(LogDLAbilityChain, Log, TEXT("AckCheckOutAbility 技能没有继续进行衔接  %s"), *AbilityClass->GetPathName());
		EvaluateContext = nullptr;
	}

	CheckOutAbilityList.RemoveSingleSwap(AbilityClass);
}


static void BuildAlternateAbilityDef(const TSharedPtr<FJsonObject> JsonObject, FAlternateAbilityDefine* Define)
{
	if (JsonObject && Define)
	{
		FString AbilityClass;
		JsonObject->TryGetStringField(TEXT("AbilityClass"), AbilityClass);

		int32 Priority = 0;
		JsonObject->TryGetNumberField(TEXT("Priority"), Priority);

		Define->Priority = Priority;
		Define->AbilityClass = AbilityClass;
	}
}

static void BuildAbilityChainNodeDefine(const TSharedPtr<FJsonObject> JsonObject, FAbilityChainNodeDefine* Define)
{
	if (JsonObject && Define)
	{
		const TArray<TSharedPtr<FJsonValue>>* ChildNodes = nullptr;
		const TArray<TSharedPtr<FJsonValue>>* AlternateAbilityList = nullptr;

		FString InputKey;
		JsonObject->TryGetStringField(TEXT("InputID"), InputKey);

		JsonObject->TryGetArrayField(TEXT("AlternateAbilityList"), AlternateAbilityList);

		Define->InputTags = FAbilityChainInputKey(InputKey);

		for (const TSharedPtr<FJsonValue>& Node : *AlternateAbilityList)
		{
			const TSharedPtr<FJsonObject> Object = Node->AsObject();
			if (Object)
			{
				FAlternateAbilityDefine& AlternateAbility = Define->AlternateAbilityList.AddDefaulted_GetRef();
				BuildAlternateAbilityDef(Object, &AlternateAbility);
			}
		}
		Define->AlternateAbilityList.Sort([](const FAlternateAbilityDefine& A, const FAlternateAbilityDefine& B)
		{
			return A.Priority > B.Priority;
		});

		JsonObject->TryGetArrayField(TEXT("ChildNodes"), ChildNodes);

		for (const TSharedPtr<FJsonValue>& Node : *ChildNodes)
		{
			const TSharedPtr<FJsonObject> Object = Node->AsObject();
			if (Object)
			{
				TSharedPtr<FAbilityChainNodeDefine> NodeDef = MakeShared<FAbilityChainNodeDefine>();
				BuildAbilityChainNodeDefine(Object, NodeDef.Get());
				Define->ChildNodes.Add(MoveTemp(NodeDef));
			}
		}
	}
}

static void BuildAbilityTreeStr(const TSharedPtr<FAbilityChainNodeDefine> Root, const int32 Ident, FString* Out)
{
	static uint8 VecLeft[100];

	if (Ident > 0)
	{
		for (int i = 0; i < Ident - 1; ++i)
		{
			*Out += VecLeft[i] ? TEXT("│   ") : TEXT("    ");
		}
		*Out += VecLeft[Ident - 1] ? TEXT("├── ") : TEXT("└── ");
	}

	if (!Root)
	{
		return;
	}

	const int32 MaxSize = Root->ChildNodes.Num();

	*Out += Root->InputTags.ToString();
	*Out += TEXT("\n");

	if (MaxSize == 0)
	{
		return;
	}

	for (int i = 0; i < MaxSize; ++i)
	{
		if (i < (MaxSize - 1))
		{
			VecLeft[Ident] = 1;
		}
		else
		{
			VecLeft[Ident] = 0;
		}

		BuildAbilityTreeStr(Root->ChildNodes[i], Ident + 1, Out);
	}
}


bool UAbilityChain::BuildAbilityChainTree(const FString& InAbilityJson)
{
	AbilityRoot = MakeShared<FAbilityChainNodeDefine>();

	const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(InAbilityJson);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		BuildAbilityChainNodeDefine(JsonObject, AbilityRoot.Get());
	}

#if !UE_BUILD_SHIPPING

	TArray<FString> AbilityList;
	if (AbilityRoot)
	{
		BuildAbilityTreeStr(AbilityRoot, 0, &AbilityChainListDebugString);
	}

#endif


	return true;
}

FString UAbilityChain::GetDebugString() const
{
	FString DebugStrInputList;

	DebugStrInputList += AbilityChainListDebugString;
	DebugStrInputList += TEXT("\n\n");

	if (this->EvaluateContext && this->EvaluateContext->InputIdList.Num() > 0)
	{
		for (const auto& InputID : this->EvaluateContext->InputIdList)
		{
			DebugStrInputList += (FString(TEXT("->")) + InputID.ToString());
		}
	}
	else
	{
		DebugStrInputList += TEXT("None Input");
	}

	return DebugStrInputList;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool UAbilityChain::Init(UAbilityChainAsset* Asset)
{
	if (!Asset)
	{
		return false;
	}

	return BuildAbilityChainTree(Asset->AbilityChainData);
}
