// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLDrawDebugSetting.generated.h"

enum class EDrawDebugCategory : uint8;
/**
 *
 */
UCLASS(Config = DLDrawDebugSetting)
class DLDRAWDEBUGSYSTEM_API UDLDrawDebugSetting : public UDeveloperSettings
{
public:
	virtual FName GetSectionName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetContainerName() const override;

#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif
private:
	GENERATED_BODY()

public:

	/**
	 * @brief  获取配置文件的实例  只读配置文件
	 * @return
	 */
	static UDLDrawDebugSetting* Get();


	UFUNCTION(BlueprintPure, Category = DLSettings)
		static const UDLDrawDebugSetting* GetDLDrawDebugSetting();

public:

	/*默认是否开启所有目录*/
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
		bool bIsAllowAllCategories = true;

	/*自定义目录的白名单*/
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
		TArray<FString> WhiteList_CustomCategory;

	/*预设目录的白名单*/
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
		TArray<EDrawDebugCategory> WhiteList;

	/*自定义目录的黑名单*/
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
		TArray<FString> BlackList_CustomCategory;

	/*预设目录的黑名单*/
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
		TArray<EDrawDebugCategory> BlackList;
};
