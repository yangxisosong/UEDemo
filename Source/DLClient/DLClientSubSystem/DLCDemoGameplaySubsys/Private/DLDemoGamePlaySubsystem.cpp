#include "DLDemoGamePlaySubsystem.h"
#include "DLPlayerAbilityDataTable.h"
#include "DLUIManagerSubsystem.h"
#include "IDemoGamePlayInterface.h"
#include "IDLPrimaryLayout.h"
#include "IDLClientAppInstance.h"
#include "AttributeSet/DLCharacterAttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "DataTable/DTSceneInfoRow.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayFramwork/DLCharacterBase.h"
#include "Interface/IDLLocalPlayerGameplayInterface.h"
#include "Interface/IDLPrimaryPlayerAbilitySystem.h"

DEFINE_LOG_CATEGORY(Log_DLDemoGamePlay);

void UDLDemoGamePlaySubsystem::OnPlayerAbilitySysReady()
{
	const auto ClientAppInstance = IDLClientAppInstance::GetChecked(this);
	PlayerAbilitySystem = ClientAppInstance->GetGameplayInterface()->GetPrimaryPlayerAbilitySystem();

	TArray<FDLAbilityDesc> AbilityDescArray;
	PlayerAbilitySystem->GetAllActiveAbilityDesc(AbilityDescArray);

	if(AbilityDescArray.Num()>0)
	{
		for (int32 i = 0; i < AbilityDescArray.Num(); i++)
		{
			if (AbilityDescArray[i].AbilityUISlot.IsValid())
			{
				FPlayerAbilityInfo AbilityInfo;
				AbilityInfo.AbilityId = AbilityDescArray[i].AbilityId;
				AbilityInfo.CDDuration = AbilityDescArray[i].CDDuration;
				AbilityInfo.AbilityUISlot = AbilityDescArray[i].AbilityUISlot;
				const auto DataTable = PlayerAbilityConfig.LoadSynchronous();
				if(DataTable)
				{
					const auto Row = DataTable->FindRow<FDLPlayerAbilityConfig>
						(AbilityInfo.AbilityId, TEXT("Default"),true);
					if (Row)
					{
						AbilityInfo.AbilityIcon = Row->AbilityIcon;
						AbilityInfo.AbilityKeystrokes = Row->AbilityKey;
					}
					PlayerAbilityInfo.Add(AbilityInfo);
				}
			}
		}
	}

	BindAbilityListener();
}

void UDLDemoGamePlaySubsystem::InitSubsystem(const FClientSubsystemCollection& Collection)
{
	Super::InitSubsystem(Collection);

	CanInstanceExtensionWidget.BindLambda([]() {return true; });

	const auto ClientAppInstance = IDLClientAppInstance::GetChecked(this);

	const auto GameplayInterface = ClientAppInstance->GetGameplayInterface();

	CharacterStateAccessor = GameplayInterface->GetPrimaryPlayerStateAccessor();
	CharacterStateListener = GameplayInterface->GetPrimaryPlayerStateListener();

	check(CharacterStateAccessor);
	check(CharacterStateListener);

	GameplayInterface->OnPlayerAbilitySysReady_CallOrRegister(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, 
			&ThisClass::OnPlayerAbilitySysReady));

	// TODO
	
	InitUIExtensionSystem();

	BindAttributeListener();

	BindUIEventListener();

	// 判断当前是否已经有敌人焦点（消息会提前发）
	InitBossInfo(GameplayInterface);
}

void UDLDemoGamePlaySubsystem::UninitSubsystem()
{
	//移除前端的 UI
	UDLUIManagerSubsystem* UISystem = GetGameInstance()->GetSubsystem<UDLUIManagerSubsystem>();
	if (!UISystem)
	{
		return;
	}

	const auto PrimaryLayout = UISystem->GetPrimaryLayout();
	if (PrimaryLayout)
	{
		const FString TagName = "UI.Layout.GameNormal";
		FGameplayTag Tag;
		Tag.FromExportString(TagName);
		PrimaryLayout->RemoveWidgetToLayout(Tag, Cast<UUserWidget>(MainWidget));
	}

	for (TPair<FGameplayTag, FGameplayMessageListenerHandle> iter : MessageListenerHandleMap)
	{
		iter.Value.Unregister();
	}
	MessageListenerHandleMap.Empty();

	Super::UninitSubsystem();
}

FBossBaseInfo UDLDemoGamePlaySubsystem::GetBossInfo()
{
	return BossBaseInfo;
}

void UDLDemoGamePlaySubsystem::Restart()
{
	const auto ClientAppInstance = IDLClientAppInstance::GetChecked(this);
	const auto GameplayInterface = ClientAppInstance->GetGameplayInterface();
	GameplayInterface->PlayerReborn();
}

void UDLDemoGamePlaySubsystem::BackFrontEnd()
{

}

void UDLDemoGamePlaySubsystem::OnPrimaryLayoutReady()
{
	UDLUIManagerSubsystem* UISystem = GetGameInstance()->GetSubsystem<UDLUIManagerSubsystem>();
	if (!UISystem)
	{
		return;
	}

	UClass* MainWidgetClass = WidgetClass.LoadSynchronous();
	if (!ensureAlwaysMsgf(MainWidgetClass, TEXT("一定是忘记配置界面了")))
	{
		return;
	}

	// 创建前端的 UI 到 Layout
	const auto PrimaryLayout = UISystem->GetPrimaryLayout();
	if (PrimaryLayout)
	{
		const FString TagName = "UI.Layout.GameNormal";
		FGameplayTag LayoutTag;
		LayoutTag.FromExportString(TagName);
		const auto Widget = PrimaryLayout->AddWidgetToLayout(LayoutTag, MainWidgetClass);
		if (Cast<IDLDemoGamePlayInterface>(Widget))
		{
			MainWidget = Cast<IDLDemoGamePlayInterface>(Widget);
			MainWidget->InitCurrentSubsystem(this);
		}
	}

	//获取玩家数据
	InitPlayerBaseInfo();

	// 显示鼠标
	//GetPlayerControllerChecked()->SetShowMouseCursor(true);
}

void UDLDemoGamePlaySubsystem::InitPlayerBaseInfo()
{
	UpdatePlayerBaseInfo();

	if (MainWidget)
	{
		MainWidget->InitPlayerBaseInfo(PlayerBaseInfo);
		MainWidget->InitPlayerAbilityInfo(PlayerAbilityInfo);

		//测试
		//FBossBaseInfo BaseInfo;
		//BaseInfo.HP = 10000;
		//BaseInfo.CurrentHP = 10000;
		//BaseInfo.BossName = FText::FromString(UTF8_TO_TCHAR("泰坦巨猿-*"));
		//BossBaseInfo = BaseInfo;
		//MainWidget->InitBossBaseInfo(BaseInfo);
	}
}

void UDLDemoGamePlaySubsystem::UpdatePlayerBaseInfo()
{
	if(CharacterStateAccessor.GetObject())
	{
		//血
		const auto CurrentHp = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(CharacterStateAccessor.GetObject(), UDLPlayerAttributeSet::GetHealthAttribute());
		const auto MaxHp = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(CharacterStateAccessor.GetObject(), UDLPlayerAttributeSet::GetMaxHealthAttribute());

		//魂力
		const auto CurrentMP = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(CharacterStateAccessor.GetObject(), UDLPlayerAttributeSet::GetSoulPowerAttribute());
		const auto MaxMP = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(CharacterStateAccessor.GetObject(), UDLPlayerAttributeSet::GetMaxSoulPowerAttribute());

		//体力（技能和跑步消耗）
		const auto CurrentPS = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(CharacterStateAccessor.GetObject(), UDLPlayerAttributeSet::GetStaminaAttribute());
		const auto MaxPS = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(CharacterStateAccessor.GetObject(), UDLPlayerAttributeSet::GetStaminaAttribute());

		PlayerBaseInfo.HP = MaxHp;
		PlayerBaseInfo.MP = MaxMP;
		PlayerBaseInfo.PS = MaxPS;
		PlayerBaseInfo.CurrentHP = CurrentHp;
		PlayerBaseInfo.CurrentMP = CurrentMP;
		PlayerBaseInfo.CurrentPS = CurrentPS;
	}
}

void UDLDemoGamePlaySubsystem::SetPlayerAbilityInfo()
{
	if (MainWidget)
	{
		MainWidget->InitPlayerAbilityInfo(PlayerAbilityInfo);
	}
}

void UDLDemoGamePlaySubsystem::InitUIExtensionSystem()
{
	UDLUIManagerSubsystem* UISystem = GetGameInstance()->GetSubsystem<UDLUIManagerSubsystem>();
	if (!UISystem)
	{
		return;
	}

	//战斗UI根据 战斗消息处理
	const FString TagName = "UI.GamePlay.BossInfo";
	FGameplayTag BossTag;
	BossTag.FromExportString(TagName);
	//将配置注册到扩展系统
	for (const auto& i : TagAndWidgetClass)
	{
		FDLUIExtensionWidgetCondition TempWidgetCondition;
		if(i.ExtPointTag == BossTag)
		{
			FDLUIExtWidgetMsgCondition MsgConditionAdd;
			MsgConditionAdd.MsgType = EDLGameplayMsgType::LocalPlayer_Gameplay_FocusNewTarget;
			MsgConditionAdd.ExecAction = EDLUIExtensionActionType::AddWidgetOnly;
			MsgConditionAdd.MsgPayloadDataType = FDLLocalPlayerFocusNewTargetMsgBody::StaticStruct();
			TempWidgetCondition.InstansMsgConditions.Add(MsgConditionAdd);

			FDLUIExtWidgetMsgCondition MsgConditionDelete;
			MsgConditionDelete.MsgType = EDLGameplayMsgType::LocalPlayer_Gameplay_LoseTarget;
			MsgConditionDelete.ExecAction = EDLUIExtensionActionType::RemoveWidget;
			MsgConditionDelete.MsgPayloadDataType = FDLLocalPlayerFocusNewTargetMsgBody::StaticStruct();
			TempWidgetCondition.InstansMsgConditions.Add(MsgConditionDelete);

			UISystem->GetUIExtensionSystem()->RegisterExtensionWidget(i, TempWidgetCondition);
		}
		else
		{
			UISystem->GetUIExtensionSystem()->RegisterOrInstanceExtensionWidget(i, TempWidgetCondition, CanInstanceExtensionWidget);
		}
	}

	if (!UISystem->GetPrimaryLayout())
	{
		UISystem->OnPrimaryLayoutReady.AddUObject(this, &ThisClass::OnPrimaryLayoutReady);
	}
	else
	{
		this->OnPrimaryLayoutReady();
	}
}

void UDLDemoGamePlaySubsystem::BindAttributeListener()
{
	if (CharacterStateListener)
	{
		auto Call = [this](const FOnAttributeChangeData& Data) {

			this->OnPlayerInfoChange(Data);
		};

		CharacterStateListener->OnGameplayAttributeValueChange(UDLPlayerAttributeSet::GetHealthAttribute()).
			AddWeakLambda(this, Call);

		CharacterStateListener->OnGameplayAttributeValueChange(UDLPlayerAttributeSet::GetMaxHealthAttribute()).
			AddWeakLambda(this, Call);

		CharacterStateListener->OnGameplayAttributeValueChange(UDLPlayerAttributeSet::GetSoulPowerAttribute()).
			AddWeakLambda(this, Call);

		CharacterStateListener->OnGameplayAttributeValueChange(UDLPlayerAttributeSet::GetMaxSoulPowerAttribute()).
			AddWeakLambda(this, Call);

		CharacterStateListener->OnGameplayAttributeValueChange(UDLPlayerAttributeSet::GetStaminaAttribute()).
			AddWeakLambda(this, Call);

		CharacterStateListener->OnGameplayAttributeValueChange(UDLPlayerAttributeSet::GetMaxStaminaAttribute()).
			AddWeakLambda(this, Call);
	}

}

void UDLDemoGamePlaySubsystem::BindAbilityListener()
{
	//技能监听
	UGameplayMessageSubsystem* MsgSystem = UDLUIManagerSubsystem::Get(this)->
		GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>();

	auto ThunkCallback = [this](FGameplayTag ActualTag, const FDLLocalPlayerAbilityApplyCDMsg& Msg)
	{
		this->OnPlayerAbilityChange(Msg);
	};

	const auto Handle = MsgSystem->RegisterListener<FDLLocalPlayerAbilityApplyCDMsg>(EDLGameplayMsgType::LocalPlayer_Gameplay_AbilityCD,
	                                                                                 ThunkCallback, EGameplayMessageMatch::ExactMatch);

	MessageListenerHandleMap.Add(EDLGameplayMsgType::LocalPlayer_Gameplay_AbilityCD, Handle);
}

void UDLDemoGamePlaySubsystem::BindUIEventListener()
{
	UGameplayMessageSubsystem* MsgSystem = UDLUIManagerSubsystem::Get(this)->
		GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>();

	auto ShowTipsCallback = [this](FGameplayTag ActualTag, const FDLUIShowButtonTipsMsgBody& Msg)
	{
		this->OnButtonTipsStateChange(true, Msg);
	};
	auto HideTipsCallback = [this](FGameplayTag ActualTag, const FDLUIShowButtonTipsMsgBody& Msg)
	{
		this->OnButtonTipsStateChange(false, Msg);
	};
	auto ButtonCliclCallback = [this](FGameplayTag ActualTag, const FDLUITriggerButtonClickMsgBody& Msg)
	{
		this->OnButtonClick(Msg);
	};

	auto PlayerDiedCallback = [this](FGameplayTag ActualTag, const FDLGameplayMsgBody& Msg)
	{
		this->OnPlayerLifeStateChange(true);
	};

	auto PlayerStartCallback = [this](FGameplayTag ActualTag, const FDLGameplayMsgBody& Msg)
	{
		this->OnPlayerLifeStateChange(false);
	};

	auto PlayerInBattleCallback = [this](FGameplayTag ActualTag, const FDLGameplayMsgBody& Msg)
	{
		this->OnPlayerBattleStateChange(true);
	};

	auto PlayerQuitBattleCallback = [this](FGameplayTag ActualTag, const FDLGameplayMsgBody& Msg)
	{
		this->OnPlayerBattleStateChange(false);
	};

	auto PlayerEnterSceneCallback = [this](FGameplayTag ActualTag, const FDLLocalPlayerEnterNewSceneMsgBody& Msg)
	{
		this->OnPlayerEnterScene(Msg);
	};

	auto PlayerLoseTargetCallback = [this](FGameplayTag ActualTag, const FDLLocalPlayerFocusNewTargetMsgBody& Msg)
	{
		this->OnPlayerFocusChange(true,Msg);
	};
	auto PlayerFocusNewTargetCallback = [this](FGameplayTag ActualTag, const FDLLocalPlayerFocusNewTargetMsgBody& Msg)
	{
		this->OnPlayerFocusChange(false,Msg);
	};

	const auto ShowHandle = MsgSystem->RegisterListener<FDLUIShowButtonTipsMsgBody>(EDLGameplayTriggerMsg::GameplayTrigger_ShowButtonTips,
																					 ShowTipsCallback, EGameplayMessageMatch::ExactMatch);

	const auto HideHandle = MsgSystem->RegisterListener<FDLUIShowButtonTipsMsgBody>(EDLGameplayTriggerMsg::GameplayTrigger_HideButtonTips,
																				 HideTipsCallback, EGameplayMessageMatch::ExactMatch);

	const auto ButtonHandle = MsgSystem->RegisterListener<FDLUITriggerButtonClickMsgBody>(EDLGameplayTriggerMsg::GameplayTrigger_ButtonClick,
																				 ButtonCliclCallback, EGameplayMessageMatch::ExactMatch);

	const auto PlayerDiedHandle = MsgSystem->RegisterListener<FDLGameplayMsgBody>(EDLGameplayMsgType::LocalPlayer_Gameplay_PlayerDied,
																			 PlayerDiedCallback, EGameplayMessageMatch::ExactMatch);
	
	const auto PlayerStartHandle = MsgSystem->RegisterListener<FDLGameplayMsgBody>(EDLGameplayMsgType::LocalPlayer_Gameplay_PlayerStart,
																			 PlayerStartCallback, EGameplayMessageMatch::ExactMatch);

	const auto PlayerInBattleHandle = MsgSystem->RegisterListener<FDLGameplayMsgBody>(EDLGameplayMsgType::LocalPlayer_Gameplay_InBattleState,
																			 PlayerInBattleCallback, EGameplayMessageMatch::ExactMatch);

	const auto PlayerQuitBattleHandle = MsgSystem->RegisterListener<FDLGameplayMsgBody>(EDLGameplayMsgType::LocalPlayer_Gameplay_OutBattleState ,
																			 PlayerQuitBattleCallback, EGameplayMessageMatch::ExactMatch);

	const auto PlayerEnterSceneHandle = MsgSystem->RegisterListener<FDLLocalPlayerEnterNewSceneMsgBody>(EDLGameplayMsgType::LocalPlayer_Gameplay_EnterNewScene,
																			 PlayerEnterSceneCallback, EGameplayMessageMatch::ExactMatch);

	const auto PlayerLoseTargetHandle = MsgSystem->RegisterListener<FDLLocalPlayerFocusNewTargetMsgBody>(EDLGameplayMsgType::LocalPlayer_Gameplay_LoseTarget,
																			 PlayerLoseTargetCallback, EGameplayMessageMatch::ExactMatch);

	const auto PlayerFocusNewTargetHandle = MsgSystem->RegisterListener<FDLLocalPlayerFocusNewTargetMsgBody>(EDLGameplayMsgType::LocalPlayer_Gameplay_FocusNewTarget,
																			 PlayerFocusNewTargetCallback, EGameplayMessageMatch::ExactMatch);
	//锁定目标
	MessageListenerHandleMap.Add(EDLGameplayMsgType::LocalPlayer_Gameplay_FocusNewTarget, PlayerFocusNewTargetHandle);
	MessageListenerHandleMap.Add(EDLGameplayMsgType::LocalPlayer_Gameplay_LoseTarget, PlayerLoseTargetHandle);
	//死亡重生
	MessageListenerHandleMap.Add(EDLGameplayMsgType::LocalPlayer_Gameplay_PlayerDied, PlayerDiedHandle);
	MessageListenerHandleMap.Add(EDLGameplayMsgType::LocalPlayer_Gameplay_PlayerStart, PlayerStartHandle);
	//进出战斗
	MessageListenerHandleMap.Add(EDLGameplayMsgType::LocalPlayer_Gameplay_InBattleState, PlayerInBattleHandle);
	MessageListenerHandleMap.Add(EDLGameplayMsgType::LocalPlayer_Gameplay_OutBattleState, PlayerQuitBattleHandle);
	//进入场景
	MessageListenerHandleMap.Add(EDLGameplayMsgType::LocalPlayer_Gameplay_EnterNewScene, PlayerEnterSceneHandle);
	//按键提示
	MessageListenerHandleMap.Add(EDLGameplayTriggerMsg::GameplayTrigger_ShowButtonTips, ShowHandle);
	MessageListenerHandleMap.Add(EDLGameplayTriggerMsg::GameplayTrigger_HideButtonTips, HideHandle);
	//按钮按下
	MessageListenerHandleMap.Add(EDLGameplayTriggerMsg::GameplayTrigger_ButtonClick, ButtonHandle);
}

void UDLDemoGamePlaySubsystem::InitBossInfo(IDLLocalPlayerGameplayInterface* GameplayInterface)
{
	if (GameplayInterface->HasEnemyUnitFocus())
	{
		const auto StateAccessor = GameplayInterface->GetEnemyUnitStateAccessor();
		const auto StateListener = GameplayInterface->GetEnemyUnitStateListener();
		if (StateAccessor)
		{
			const auto FocusHP = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(StateAccessor.GetObject(), UDLNPCAttributeSet::GetHealthAttribute());
			const auto FocusMaxHP = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(StateAccessor.GetObject(), UDLNPCAttributeSet::GetMaxHealthAttribute());

			BossBaseInfo.HP = FocusMaxHP;
			BossBaseInfo.CurrentHP = FocusHP;
			BossBaseInfo.BossName = FText::FromString(UTF8_TO_TCHAR("泰坦"));

			auto Call = [this](const FOnAttributeChangeData& Data) {

				if(Data.Attribute.AttributeName=="Health")
				{
					this->BossBaseInfo.CurrentHP= Data.NewValue;
					this->OnBossInfoChange(true, BossBaseInfo);
				}
			};

			StateListener->OnGameplayAttributeValueChange(UDLPlayerAttributeSet::GetHealthAttribute()).
				AddWeakLambda(this, Call);

			//StateListener->OnGameplayAttributeValueChange(UDLPlayerAttributeSet::GetMaxHealthAttribute()).
			//	AddWeakLambda(this, Call);

			UDLUIManagerSubsystem* UISystem = GetGameInstance()->GetSubsystem<UDLUIManagerSubsystem>();
			if (UISystem)
			{
				const FString TagName = "UI.GamePlay.BossInfo";
				FGameplayTag BossTag;
				BossTag.FromExportString(TagName);
				UISystem->GetUIExtensionSystem()->SetWidgetVisibleWithTag(BossTag, true);
			}
		}
	}
}

void UDLDemoGamePlaySubsystem::OnPlayerInfoChange(const FOnAttributeChangeData& Data)
{
	UpdatePlayerBaseInfo();
	if (MainWidget)
	{
		MainWidget->UpdatePlayerBaseInfo(PlayerBaseInfo);
	}
	//UE_LOG(Log_DLDemoGamePlay, Log, TEXT("OnGameplayAttributeValueChange %s"), *Data.Attribute.AttributeName);
}

void UDLDemoGamePlaySubsystem::OnBossInfoChange(bool IsFocus,const FBossBaseInfo& Data)
{
	if(MainWidget)
	{
		MainWidget->UpdateBossBaseInfo(BossBaseInfo);
	}
}

void UDLDemoGamePlaySubsystem::OnPlayerAbilityChange(const FDLLocalPlayerAbilityApplyCDMsg& Data)
{
	if (MainWidget)
	{
		FPlayerAbilityInfo AbilityInfo;
		AbilityInfo.AbilityId= Data.OwnerAbilityId;
		AbilityInfo.CDDuration = Data.Duration;
		AbilityInfo.AbilityKeystrokes = FText::FromString(FString::FromInt(00));
		MainWidget->PlayerAbilityCDChange(AbilityInfo);
	}
	UE_LOG(Log_DLDemoGamePlay, Log, TEXT("OnPlayerAbilityChange %F"), Data.Duration);
}

void UDLDemoGamePlaySubsystem::OnButtonTipsStateChange(bool IsShow, const FDLUIShowButtonTipsMsgBody& Msg)
{
	if (MainWidget)
	{
		IsShowButtonTips = IsShow;
		MainWidget->SetButtonTipsShow(IsShow, Msg);
	}
}

void UDLDemoGamePlaySubsystem::OnButtonClick(const FDLUITriggerButtonClickMsgBody& Msg)
{
	if (MainWidget)
	{
		MainWidget->KeyClickEvent(Msg.Key);
	}
}

void UDLDemoGamePlaySubsystem::OnPlayerLifeStateChange(bool IsDied)
{
	if (MainWidget)
	{
		if(IsDied)
		{
			MainWidget->PlayerDied();
			// 显示鼠标
			GetPlayerControllerChecked()->SetShowMouseCursor(true);
		}
		else
		{
			InitPlayerBaseInfo();
			MainWidget->PlayerRebirth();

			// 隐藏鼠标
			GetPlayerControllerChecked()->SetShowMouseCursor(false);
		}
	}
}

void UDLDemoGamePlaySubsystem::OnPlayerBattleStateChange(bool IsInBattle)
{
	if (MainWidget)
	{
		if (IsInBattle)
		{
			MainWidget->PlayerBeginBattle();
		}
		else
		{
			MainWidget->PlayerQuiteBattle();
		}
	}
}

void UDLDemoGamePlaySubsystem::OnPlayerEnterScene(const FDLLocalPlayerEnterNewSceneMsgBody& Msg)
{
	if (MainWidget)
	{
		const auto DataTable = SceneInfo.LoadSynchronous();
   		if (DataTable)
		{
			const auto Row = DataTable->FindRow<FDTSceneInfoRow>
				(Msg.SceneId, TEXT("Default"), true);
			if (Row)
			{
				MainWidget->PlayerEnterScene(Row->WelcomeSceneText);
			}
		}
	}
}

void UDLDemoGamePlaySubsystem::OnPlayerFocusChange(bool IsLose, const FDLLocalPlayerFocusNewTargetMsgBody& Msg)
{
	if(!IsLose)
	{
		if (Msg.StateAccessor == nullptr || Msg.StateListener == nullptr)
		{
			return;
		}

		const auto FocusHP = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(Msg.StateAccessor.GetObject(), UDLNPCAttributeSet::GetHealthAttribute());
		const auto FocusMaxHP = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(Msg.StateAccessor.GetObject(), UDLNPCAttributeSet::GetMaxHealthAttribute());

		BossBaseInfo.HP = FocusMaxHP;
		BossBaseInfo.CurrentHP = FocusHP;
		BossBaseInfo.BossName = FText::FromString(UTF8_TO_TCHAR("泰坦"));

		auto Call = [this, Msg](const FOnAttributeChangeData& Data) {

			FBossBaseInfo BaseInfo;
			const auto FocusHP = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(Msg.StateAccessor.GetObject(), UDLNPCAttributeSet::GetHealthAttribute());
			const auto FocusMaxHP = ICharacterStateAccessor::Execute_GetCurrentAttributeValue(Msg.StateAccessor.GetObject(), UDLNPCAttributeSet::GetMaxHealthAttribute());
			BaseInfo.HP = FocusMaxHP;
			BaseInfo.CurrentHP = FocusHP;
			BaseInfo.BossName = FText::FromString(UTF8_TO_TCHAR("泰坦"));
			this->BossBaseInfo = BaseInfo;
			this->OnBossInfoChange(true,BaseInfo);
		};

		Msg.StateListener->OnGameplayAttributeValueChange(UDLPlayerAttributeSet::GetHealthAttribute()).
			AddWeakLambda(this, Call);

		Msg.StateListener->OnGameplayAttributeValueChange(UDLPlayerAttributeSet::GetMaxHealthAttribute()).
			AddWeakLambda(this, Call);

		if (MainWidget)
		{
			MainWidget->InitBossBaseInfo(BossBaseInfo);
		}
	}
	else
	{
		this->OnBossInfoChange(false, BossBaseInfo);
	}

}

void UDLDemoGamePlaySubsystem::OnPlayerBattleVictory()
{
	if(MainWidget)
	{
		MainWidget->PlayerBattleVictory();
	}
}
