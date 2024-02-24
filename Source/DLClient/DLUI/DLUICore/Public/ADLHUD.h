#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/HUD.h"
#include "Interface/IGameplayUISystem.h"
#include "ADLHUD.generated.h"



/**
 * @brief Gameplay UI 的基类，这个类是一个容器，来管理 Gameplay 中的一些子部件
 *
 * 设计思路，这个类会由具体的 业务逻辑来实现子类，基类只负责一些逻辑相关的实现，不进行任何表现操作
 */
UCLASS()
class DLUICORE_API UDLGameplayUIMainBase : public UUserWidget
{
	GENERATED_BODY()
public:

	void Init(TScriptInterface<ICharacterStateAccessor> InCharacterStateAccessor, TScriptInterface<ICharacterStateListener> InCharacterStateListener)
	{
		// 初始化数据组件
		CharacterStateAccessor = InCharacterStateAccessor;
		CharacterStateListener = InCharacterStateListener;

		// Ntf 初始化逻辑
		this->OnInit();
	}

protected:

	virtual void OnInit()
	{

	}

protected:

	UPROPERTY(BlueprintReadOnly)
		TScriptInterface<ICharacterStateAccessor> CharacterStateAccessor;

	UPROPERTY(BlueprintReadOnly)
		TScriptInterface<ICharacterStateListener> CharacterStateListener;
};



/**
 * @brief HUD 是游戏中全部UI的容器，更多的是做逻辑上的管理，将来可能会做一些 Debug 的绘制
 */
UCLASS()
class DLUICORE_API ADLHUD
	: public AHUD
	, public IGameplayUISystem
{
	GENERATED_BODY()
public:

	virtual void OnGameplayInit(const FInitArg& Arg) override
	{
		UClass* GameplayUIClass = DLGameplayUIClass.LoadSynchronous();

		if (GameplayUIClass)
		{
			GameplayUI = CreateWidget<UDLGameplayUIMainBase>(GetOwningPlayerController(), GameplayUIClass, TEXT("GameplayWidget"));

			GameplayUI->Init(Arg.CharacterStateAccessor, Arg.CharacterStateListener);

			GameplayUI->AddToViewport();

			this->OnCreateGameplayUI(GameplayUI);

			// 
			if (LoginUI)
			{
				LoginUI->RemoveFromViewport();
			}

			LoginUI = nullptr;
		}

	}



protected:

	virtual void BeginPlay() override
	{
		//UClass* LoginWidgetClass = DLLoginUIClass.LoadSynchronous();
		//
		//if (LoginWidgetClass)
		//{
		//	LoginUI = CreateWidget<UUserWidget>(GetOwningPlayerController(), LoginWidgetClass, TEXT("LoginWidget"));
		//	LoginUI->AddToViewport();
		//	this->OnCreateLoginUI(LoginUI);
		//}

		Super::BeginPlay();
	}


	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override
	{
		Super::EndPlay(EndPlayReason);
	}


protected:

	UFUNCTION(BlueprintImplementableEvent)
		void OnCreateGameplayUI(UDLGameplayUIMainBase* Widget);

	UFUNCTION(BlueprintImplementableEvent)
		void OnCreateLoginUI(UUserWidget* Widget);

protected:

	UPROPERTY()
		UUserWidget* LoginUI;

	UPROPERTY()
		UDLGameplayUIMainBase* GameplayUI;

	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<UUserWidget> DLLoginUIClass;

	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<UDLGameplayUIMainBase> DLGameplayUIClass;
};
