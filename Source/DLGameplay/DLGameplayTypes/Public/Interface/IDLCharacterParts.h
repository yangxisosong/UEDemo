#pragma once

#include "CoreMinimal.h"
#include "CharacterTypesDef.h"
#include "IDLCharacterParts.generated.h"



UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class UDLCharacterParts : public UInterface
{
	GENERATED_BODY()
};

class DLGAMEPLAYTYPES_API IDLCharacterParts : public IInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
		virtual FGameplayTag GetPartByBodyName(FName BoneName) = 0;

	UFUNCTION(BlueprintCallable)
		virtual FGameplayTag GetPartByBodyIndex(int32 Index) = 0;

	UFUNCTION(BlueprintCallable)
		virtual FName GetBodyNameByIndex(int32 Index) = 0;

	/**
	 * @brief 是否拆分了部位，如果没有拆分，其他 API 取出来的始终是 默认部位
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
		virtual bool IsSplitParts() const { return true; }

public:

	virtual int32 GetDefaultPartAttrIndex()const = 0;

	virtual int32 GetPartAttrIndexByTag(const FGameplayTag& PartTag) = 0;
};