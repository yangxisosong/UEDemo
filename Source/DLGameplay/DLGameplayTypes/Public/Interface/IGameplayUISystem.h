#pragma once

#include "CoreMinimal.h"

#include "IGameplayUISystem.generated.h"


class ICharacterStateAccessor;
class ICharacterStateListener;


UINTERFACE(MinimalAPI)
class UGameplayUISystem
	: public UInterface
{
	GENERATED_BODY()
};


class DLGAMEPLAYTYPES_API IGameplayUISystem : public IInterface
{
	GENERATED_BODY()


public:

	struct FInitArg
	{
		TScriptInterface<ICharacterStateAccessor> CharacterStateAccessor;
		TScriptInterface<ICharacterStateListener> CharacterStateListener;
	};

	/**
	 * @brief 当进入Gameplay场景时调用，这个时候 角色等数据已经加载完成
	 * @param Arg 
	 * @return 
	 */
	virtual void OnGameplayInit(const FInitArg& Arg) = 0;
};

