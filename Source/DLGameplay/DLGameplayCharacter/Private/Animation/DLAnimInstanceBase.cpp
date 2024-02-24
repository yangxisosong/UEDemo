#include "Animation/DLAnimInstanceBase.h"

#include "AnimationTestCharacter.h"
#include "DLAnimCommonLibrary.h"
#include "Component/DLCharacterMovementComponentBase.h"
#include "Components/DLAnimComponentBase.h"
#include "GameFramework/Character.h"
#include "DLAssetManager/Public/DLAssetManager.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "DLGameplayTypes/Public/Interface/ICharacterStateAccessor.h"

TScriptInterface<ICharacterStateAccessor> UDLAnimInstanceBase::GetCharacterStateAccessor() const
{
	return CharacterStateAccessor;
}

UDLCharacterMovementComponentBase* UDLAnimInstanceBase::GetMovementComponentBase() const
{
	return MovementComponentBase;
}

void UDLAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (!GetOwnerCharacter())
	{
		//没有Owner说明是在实例中
#if WITH_EDITOR
		if (!DebugAnimConfigName.IsEmpty())
		{
			LoadDebugAnimConfig();
		}
#endif
		return;
	}
}

void UDLAnimInstanceBase::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	OnInitAnimIns();
}

void UDLAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!GetOwnerCharacter() || !GetAnimCompFromOwner())
	{
		return;
	}

	CopyCharacterInfoFromAnimComp();
	PrevAnimCharacterInfo = NextPrevAnimCharacterInfo;

	if (IS_ANIM_LOG_ENABLED())
	{
		const FString AnimCharacterInfoStr = UDLAnimCommonLibrary::ConvertUStructToJson(AnimCharacterInfo);
		const FString PrevAnimCharacterInfoStr = UDLAnimCommonLibrary::ConvertUStructToJson(PrevAnimCharacterInfo);

		// DL_ANIM_LOG(Warning
		// 	, TEXT("<UDLAnimInstanceBase>NativeUpdateAnimation,anim ins name is %s,\nnew value is %s,\nprevious value is %s")
		// 	, *(this->GetName()), *AnimCharacterInfoStr, *PrevAnimCharacterInfoStr);
	}

	OnUpdateAnimation(DeltaSeconds);

	//每帧标记一次旧数据,Tick完成之后我们就能够确定下次PrevAnimCharacterInfo了
	NextPrevAnimCharacterInfo = AnimCharacterInfo;
}

// UBlendSpace1D* UDLAnimInstanceBase::GetBlendSpace1DByTag(const FGameplayTag Name) const
// {
// 	return UDLAnimCommonLibrary::AnimAssetToBlendSpace1D(GetAnimAssetByTag(Name));
// }
//
// UBlendSpace* UDLAnimInstanceBase::GetBlendSpaceByTag(const FGameplayTag Name) const
// {
// 	return UDLAnimCommonLibrary::AnimAssetToBlendSpace(GetAnimAssetByTag(Name));
// }
//
// UAnimSequence* UDLAnimInstanceBase::GetAnimSeqByTag(const FGameplayTag Name) const
// {
// 	return UDLAnimCommonLibrary::AnimAssetToAnimSeq(GetAnimAssetByTag(Name));
// }
//
// UAnimMontage* UDLAnimInstanceBase::GetAnimMontageByTag(const FGameplayTag Name) const
// {
// 	return UDLAnimCommonLibrary::AnimAssetToAnimMontage(GetAnimAssetByTag(Name));
// }

float UDLAnimInstanceBase::GetCurveValueByEnum(const EDLAnimCurveName Curve) const
{
	return GetCurveValue(UDLAnimCommonLibrary::GetEnumerationToName<EDLAnimCurveName>(Curve));
}

void UDLAnimInstanceBase::ApplyDebugAnimCharacterInfo()
{
	UDLAnimCommonLibrary::ConvertJsonToUStruct<FAnimCharacterInfo>(DebugAnimCharacterInfo, AnimCharacterInfo);
}

void UDLAnimInstanceBase::LoadDebugAnimConfig()
{
	static FSoftObjectPath AnimConfigPath("DataTable'/Game/DL/ConfigData/Animation/DL_AnimConfig.DL_AnimConfig'");
	UDataTable* AnimConfigTable = Cast<UDataTable>(AnimConfigPath.TryLoad());
	if (!AnimConfigTable)
	{
		UE_LOG(LogTemp, Error, TEXT("<UDLAnimInstanceBase::LoadAnimConfig> load anim config data table fail!!!"));
		return;
	}

	FAnimConfig* Data = AnimConfigTable->FindRow<FAnimConfig>(FName(DebugAnimConfigName)
			, TEXT("<UDLAnimInstanceBase::LoadAnimConfig> load anim config data table"));
	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("<UDLAnimInstanceBase::LoadAnimConfig> Not find target row:%s "), *DebugAnimConfigName);
		return;
	}
	AnimConfig = *Data;
}

UDLAnimComponentBase* UDLAnimInstanceBase::GetAnimCompFromOwner() const
{
	if (!GetOwnerCharacter())
	{
		return nullptr;
	}
	return Cast<UDLAnimComponentBase>(GetOwnerCharacter()->GetComponentByClass(UDLAnimComponentBase::StaticClass()));
}

AAnimationTestCharacter* UDLAnimInstanceBase::GetOwnerCharacter() const
{
	return Cast<AAnimationTestCharacter>(TryGetPawnOwner());
}

void UDLAnimInstanceBase::OnInitAnimIns()
{
	AnimConfig = GetOwnerCharacter()->GetAnimConfig();

	// CharacterStateAccessor = TryGetPawnOwner();
	// MovementComponentBase = Cast<UDLCharacterMovementComponentBase>(TryGetPawnOwner()->GetMovementComponent());
	//
	// ensureAlwaysMsgf(CharacterStateAccessor, TEXT("应该可以获取到 读取状态的 接口"));
	// ensureAlwaysMsgf(MovementComponentBase, TEXT("应该可以获取到  移动组件的 接口"));
}

void UDLAnimInstanceBase::CopyCharacterInfoFromAnimComp()
{
	AnimCharacterInfo = GetAnimCompFromOwner()->GetAnimCharacterInfo();
}
