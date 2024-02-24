// Fill out your copyright notice in the Description page of Project Settings.


#include "DLNPCStartPoint.h"

#include "DLAssetManager.h"
#include "Components/CapsuleComponent.h"
#include "DataTable/DLGameplayDataTableHelper.h"
#include "DataTable/DTPlayerCharacterInfoRow.h"
#include "GameplayFramwork/DLGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Types/DLCharacterAssetDef.h"


static bool IsPreview()
{
#if WITH_EDITOR
	if (IsRunningCommandlet() || IsRunningGame())
	{
		return false;
	}
	return true;
#else
	return false;
#endif
}


ADLNPCStartPoint::ADLNPCStartPoint(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	if (IsPreview())
	{
#if WITH_EDITOR
		PreviewMesh = CreateEditorOnlyDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
		PreviewMesh->SetupAttachment(GetCapsuleComponent());
#endif 
	}
}

void ADLNPCStartPoint::BeginPlay()
{
	Super::BeginPlay();

	if (IsPreview())
	{
#if WITH_EDITOR
		PreviewMesh->SetVisibility(false);
#endif
	}

	this->StartLoadCharacterAsset();
}

void ADLNPCStartPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason != EEndPlayReason::Quit)
	{
		this->UnLoadAsset();

		if (IsPreview())
		{
#if WITH_EDITOR
			PreviewMesh->SetVisibility(true);
#endif
		}
	}
}

void ADLNPCStartPoint::StartLoadCharacterAsset()
{
	const auto NcpInfo = FDLGameplayDataTableHelper::FindNPCCharacterInfo(NpcCharacterId);
	if (!NcpInfo)
	{
		return;
	}

	UDLAssetManager& AssetManager = UDLAssetManager::Get();

	TArray<FName> LoadBundles;
	LoadBundles.Add(TEXT("Default"));

	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		LoadBundles.Add(TEXT("Server"));
	}
	else
	{
		LoadBundles.Add(TEXT("Client"));
	}

	StreamableHandle = AssetManager.LoadPrimaryAssets({ NcpInfo->BaseInfo.CharacterAssetId }, LoadBundles);
	if (!StreamableHandle.IsValid() || StreamableHandle->HasLoadCompleted())
	{
		this->OnLoadAssetComplate(NcpInfo->BaseInfo.CharacterAssetId);
	}
	else
	{
		StreamableHandle->BindCompleteDelegate(FStreamableDelegate::CreateUObject(this, &ThisClass::OnLoadAssetComplate, NcpInfo->BaseInfo.CharacterAssetId));
	}
}

void ADLNPCStartPoint::UnLoadAsset()
{
	this->OnUnloadAsset();

	if (StreamableHandle)
	{
		StreamableHandle->ReleaseHandle();
	}

	StreamableHandle = nullptr;
}

void ADLNPCStartPoint::OnLoadAssetComplate(FPrimaryAssetId AssetId)
{
	// 如果在服务端
	if (HasAuthority())
	{
		if (const auto GM = Cast<ADLGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			FDLCreateNPCArg Arg;
			Arg.NPCCharacterId = NpcCharacterId;
			Arg.StartPointId = StartPointId;
			Arg.SpawnTransform = GetActorTransform();
			const auto& AssetManager = UDLAssetManager::Get();
			UDLCharacterAsset* Asset = Cast<UDLCharacterAsset>(AssetManager.GetPrimaryAssetObject(AssetId));
			Arg.Asset = Asset;
			HoldCharacter = GM->CreateNPC(Arg);
		}
	}
	else
	{
		const auto GS = Cast<ADLGameState>(UGameplayStatics::GetGameState(this));
		if (GS)
		{
			GS->ClientLoadNPCCharacterComplate(NpcCharacterId);
		}
	}

}

void ADLNPCStartPoint::OnUnloadAsset()
{
	if (HasAuthority())
	{
		if (HoldCharacter.IsValid())
		{
			const auto PC = HoldCharacter->GetController();
			if (PC)
			{
				PC->UnPossess();
			}

			HoldCharacter->Destroy();

			HoldCharacter = nullptr;
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
TArray<FName> ADLNPCStartPoint::GetNPCNameOption() const
{
	auto AllData = FDLGameplayDataTableHelper::GetAllNPCCharacterInfo();

	TArray<FName> Ret;
	for (const FDTNPCCharacterInfoRow* Data : AllData)
	{
		Ret.AddZeroed_GetRef() = *Data->BaseInfo.Name.ToString();
	}
	return Ret;
}

#if WITH_EDITOR

void ADLNPCStartPoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (IsRunningCommandlet()) return;
	if (IsRunningGame()) return;

	auto AllData = FDLGameplayDataTableHelper::GetAllNPCCharacterInfo();


	const FDTNPCCharacterInfoRow* NcpInfo = nullptr;


	for (const FDTNPCCharacterInfoRow* Data : AllData)
	{
		if (!NPCName.IsEmpty())
		{
			if (Data->BaseInfo.Name.ToString() == NPCName)
			{
				NcpInfo = Data;
				NpcCharacterId = Data->BaseInfo.Id;
				break;
			}
		}
		else
		{
			if (Data->BaseInfo.Id == NpcCharacterId)
			{
				NcpInfo = Data;
				break;
			}
		}
	}


	if (NcpInfo)
	{
		const auto AssetId = NcpInfo->BaseInfo.CharacterAssetId;
		const UDLAssetManager& AssetManager = UDLAssetManager::Get();
		const auto Path = AssetManager.GetPrimaryAssetPath(AssetId);
		const auto Asset = Cast<UDLCharacterAsset>(Path.TryLoad());
		if (Asset)
		{
			const auto Class = Asset->CharacterClass.LoadSynchronous();
			if (Class)
			{
				const auto Mesh = Class->GetDefaultObject<ACharacter>()->GetMesh();
				if (Mesh && Mesh->SkeletalMesh)
				{
					PreviewMesh->SetSkeletalMesh(Mesh->SkeletalMesh);
				}
			}
		}
	}

}

#endif
