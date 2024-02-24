#pragma once

#include "CoreMinimal.h"
#include "DLGameplayAbilityBase.h"
#include "DLGameplayAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "DLGameplayAbilityLib.generated.h"


UCLASS()
class DLABILITYSYSTEM_API UDLGameplayAbilityLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:


	/**
	 * @brief 发送技能中的事件给指定的Actor
	 *
	 * Actor 必须实现 获取 ASC 的接口，否则会发送失败.  没有创建任何预测Window
	 *
	 * @param Actor	  目标Actor
	 * @param EventTag 触发事件的Tag
	 * @param Payload  事件携带的参数包
	 */
	UFUNCTION(BlueprintCallable, Category = DLAbility)
		static void SendAbilityEventToActor(AActor* Actor, FGameplayTag EventTag, FDLAbilityEventData Payload);


	/**
	 * 获取Ability的主实例
	 * 
	 * @param AbilitySystemComponent 
	 * @param Handle 
	 * @return  Ability 必须设置 UDLGameplayAbilityBase::InstancingPolicy 为 InstancedPerActor, 否则返回 nullptr 
	 */
	UFUNCTION(BlueprintCallable, Category = DLAbility)
	static UDLGameplayAbilityBase* GetPrimaryAbilityInstanceFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle);


	UFUNCTION(BlueprintCallable, Category= DLAbility )
	static FGameplayAbilityTargetDataHandle MakeEmptyGameplayAbilityTargetData();

	//获取技能的AI配置
	UFUNCTION(BlueprintCallable, BlueprintPure,Category= DLAbility )
	static void GetDLAbilityAISetting(const TSubclassOf<UDLGameplayAbilityBase> AbilityClass, FDLAbilityAISetting& AISetting);
};
