#include "DLDrawDebugSubsystem.h"

#include "ConvertString.h"
#include "DLDrawDebugSetting.h"
#include "Kismet/KismetSystemLibrary.h"


class DrawDebugManager
{
private:
	TArray<FString> AllowedCategory;

	bool bIsAllowAllCategories = false;

	bool bIsHideAllCategories = false;

	TArray<FString> BlackList;

	TArray<FString> WhiteList;


public:
	static DrawDebugManager* Get()
	{
		static auto* Ins = new DrawDebugManager();
		return Ins;
	}

	bool RemoveCategory(const FString& Category)
	{
		const int RemoveCounts = AllowedCategory.RemoveAll(
			[Category](const auto Item)
					{
						return Category.Equals(Item, ESearchCase::IgnoreCase);
					}
		);
		return RemoveCounts > 0;
	}

	void AddCategory(const FString& Category)
	{
		bIsHideAllCategories = false;
		AllowedCategory.AddUnique(Category);
		BlackList.RemoveAll(
			[Category](const auto Item)
					{
						return Category.Equals(Item, ESearchCase::IgnoreCase);
					}
		);
	}

	void AddCategories(const TArray<FString>& Categories)
	{
		for (auto Arg : Categories)
		{
			AddCategory(Arg);
		}
	}

	void HideAll(const UWorld* World)
	{
		bIsAllowAllCategories = false;
		bIsHideAllCategories = true;

		if (IsValid(World))
		{
			UKismetSystemLibrary::FlushPersistentDebugLines(World);
		}
	}

	void ShowAll()
	{
		bIsAllowAllCategories = true;
		bIsHideAllCategories = false;
	}

	bool IsAllowedCategory(const FString& Category) const
	{


		if (bIsHideAllCategories)
		{
			return false;
		}


		if (bIsAllowAllCategories)
		{
			const bool bIsInBlackList = BlackList.ContainsByPredicate(
				[Category](const auto Item)
				{
					return Category.Equals(Item, ESearchCase::IgnoreCase);
				}
			);
			return !bIsInBlackList;
		}

		const bool bIsInWhiteList = WhiteList.ContainsByPredicate(
			[Category](const auto Item)
			{
				return Category.Equals(Item, ESearchCase::IgnoreCase);
			}
		);


		const bool bIsInAllowList = AllowedCategory.ContainsByPredicate(
			[Category](const auto Item)
			{
				return Category.Equals(Item, ESearchCase::IgnoreCase);
			}
		);

		return bIsInAllowList || bIsInWhiteList;
	}

	bool IsAllowedCategory(const EDrawDebugCategory& Category)
	{
		return IsAllowedCategory(ENUM_TO_STR(EDrawDebugCategory, Category));
	}

	bool LoadDefaultSetting()
	{
		const auto* Setting = UDLDrawDebugSetting::Get();

		if (!IsValid(Setting))
		{
			return false;
		}

		AllowedCategory.Reset();
		BlackList.Reset();
		WhiteList.Reset();
#if WITH_EDITOR
		bIsAllowAllCategories = Setting->bIsAllowAllCategories;
#endif
		bIsAllowAllCategories = false;
		bIsHideAllCategories = false;

		for (const auto& Item : Setting->WhiteList)
		{
			WhiteList.Add(ENUM_TO_STR(EDrawDebugCategory, Item));
		}

		for (const auto& Item : Setting->WhiteList_CustomCategory)
		{
			if (Item.IsEmpty())
			{
				continue;
			}
			WhiteList.Add(Item);
		}

		for (const auto& Item : Setting->BlackList)
		{
			BlackList.Add(ENUM_TO_STR(EDrawDebugCategory, Item));
		}

		for (const auto& Item : Setting->BlackList_CustomCategory)
		{
			if (Item.IsEmpty())
			{
				continue;
			}
			BlackList.Add(Item);
		}
		return true;
	}
};


static FAutoConsoleCommand CVarDLDrawDebug_ShowAll(
	TEXT("DL.DrawDebug.ShowAll")
	, TEXT("显示所有Debug绘制")
	, FConsoleCommandDelegate::CreateLambda(
		[]()
		{
			auto* Ins = DrawDebugManager::Get();
			if (Ins)
			{
				Ins->ShowAll();
			}

		})
	, ECVF_Cheat);

static FAutoConsoleCommandWithWorld CVarDLDrawDebug_HideAll(
	TEXT("DL.DrawDebug.HideAll")
	, TEXT("隐藏所有Debug绘制")
	, FConsoleCommandWithWorldDelegate::CreateLambda(
		[](UWorld* World)
		{
			auto* Ins = DrawDebugManager::Get();
			if (Ins)
			{
				Ins->HideAll(World);
			}
		})
	, ECVF_Cheat);

static FAutoConsoleCommandWithWorldAndArgs CVarDLDrawDebug_Show(
	TEXT("DL.DrawDebug.Show")
	, TEXT("显示指定目录的Debug绘制")
	, FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			auto* Ins = DrawDebugManager::Get();
			if (Ins)
			{
				Ins->AddCategories(Args);
			}

		}), ECVF_Cheat
);

static FAutoConsoleCommandWithWorldAndArgs CVarDLDrawDebug_Hide(
	TEXT("DL.DrawDebug.Hide")
	, TEXT("隐藏指定目录的Debug绘制")
	, FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			auto* Ins = DrawDebugManager::Get();
			if (Ins)
			{
				for (auto Arg : Args)
				{
					Ins->RemoveCategory(Arg);
				}
			}

		}), ECVF_Cheat
);

static FAutoConsoleCommand CVarDLDrawDebug_ResetToDefault(
TEXT("DL.DrawDebug.ResetToDefault")
, TEXT("重置为默认设置")
, FConsoleCommandDelegate::CreateLambda(
	[]()
	{
		auto* Ins = DrawDebugManager::Get();
		if (Ins)
		{
			Ins->LoadDefaultSetting();
		}

	})
	, ECVF_Cheat);

#define CHECK_IS_MANAGER_VALID() \
	auto* Ins = DrawDebugManager::Get();\
	if (!Ins)\
	{\
		return;\
	} 

#define CHECK_IS_ALLOWED_CATEGORY(Category) \
	CHECK_IS_MANAGER_VALID();\
	if (!DrawDebugManager::Get()->IsAllowedCategory(Category))\
	{\
		return;\
	}

#define CHECK_IS_CATEGORY_EMPTY(Category)\
	if (Category.IsEmpty())\
	{\
		Category = FString("Temp");\
	}




void UDLDrawDebugSubsystem::ShowCategory(const EDrawDebugCategory Category)
{
	CHECK_IS_MANAGER_VALID();
	ShowCategory_CustomCategory(ENUM_TO_STR(EDrawDebugCategory, Category));
}

void UDLDrawDebugSubsystem::ShowCategory_CustomCategory(FString Category)
{
	CHECK_IS_MANAGER_VALID();
	DrawDebugManager::Get()->AddCategory(Category);
}

void UDLDrawDebugSubsystem::HideCategory(const EDrawDebugCategory Category)
{
	CHECK_IS_MANAGER_VALID();
	HideCategory_CustomCategory(ENUM_TO_STR(EDrawDebugCategory, Category));
}

void UDLDrawDebugSubsystem::HideCategory_CustomCategory(FString Category)
{
	CHECK_IS_MANAGER_VALID();
	DrawDebugManager::Get()->RemoveCategory(Category);
}

void UDLDrawDebugSubsystem::HideAllCategories()
{
	CHECK_IS_MANAGER_VALID();

	DrawDebugManager::Get()->HideAll(GetWorld());
}

void UDLDrawDebugSubsystem::ShowAllCategories()
{
	CHECK_IS_MANAGER_VALID();

	DrawDebugManager::Get()->ShowAll();
}

void UDLDrawDebugSubsystem::DrawDebugArrow(const EDrawDebugCategory Category, const FVector LineStart,
										   const FVector LineEnd, float ArrowSize, FLinearColor LineColor, float Duration, float Thickness)
{
	CHECK_IS_ALLOWED_CATEGORY(Category);

	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), LineStart, LineEnd, ArrowSize, LineColor, Duration, Thickness);
}

void UDLDrawDebugSubsystem::DrawDebugArrow_CustomCategory(FString Category, const FVector LineStart,
	const FVector LineEnd, float ArrowSize, FLinearColor LineColor, float Duration, float Thickness)
{
	CHECK_IS_ALLOWED_CATEGORY(Category);
	CHECK_IS_CATEGORY_EMPTY(Category);


	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), LineStart, LineEnd, ArrowSize, LineColor, Duration, Thickness);
}

void UDLDrawDebugSubsystem::DrawDebugPoint(const EDrawDebugCategory Category, const FVector Position, float Size,
	FLinearColor PointColor, float Duration)
{
	CHECK_IS_ALLOWED_CATEGORY(Category);

	UKismetSystemLibrary::DrawDebugPoint(GetWorld(), Position, Size, PointColor, Duration);
}

void UDLDrawDebugSubsystem::DrawDebugPoint_CustomCategory(FString Category, const FVector Position, float Size,
	FLinearColor PointColor, float Duration)
{
	CHECK_IS_ALLOWED_CATEGORY(Category);
	CHECK_IS_CATEGORY_EMPTY(Category);


	UKismetSystemLibrary::DrawDebugPoint(GetWorld(), Position, Size, PointColor, Duration);
}

void UDLDrawDebugSubsystem::DrawDebugBox(const EDrawDebugCategory Category, const FVector Center, FVector Extent,
	FLinearColor LineColor, const FRotator Rotation, float Duration, float Thickness)
{
	CHECK_IS_ALLOWED_CATEGORY(Category);

	UKismetSystemLibrary::DrawDebugBox(GetWorld(), Center, Extent, LineColor, Rotation, Duration, Thickness);
}

void UDLDrawDebugSubsystem::DrawDebugBox_CustomCategory(FString Category, const FVector Center, FVector Extent,
	FLinearColor LineColor, const FRotator Rotation, float Duration, float Thickness)
{
	CHECK_IS_ALLOWED_CATEGORY(Category);
	CHECK_IS_CATEGORY_EMPTY(Category);


	UKismetSystemLibrary::DrawDebugBox(GetWorld(), Center, Extent, LineColor, Rotation, Duration, Thickness);
}

void UDLDrawDebugSubsystem::DrawDebugSphere(const EDrawDebugCategory Category, const FVector Center, float Radius,
	int32 Segments, FLinearColor LineColor, float Duration, float Thickness)
{
	CHECK_IS_ALLOWED_CATEGORY(Category);

	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Center, Radius, Segments, LineColor, Duration, Thickness);
}

void UDLDrawDebugSubsystem::DrawDebugSphere_CustomCategory(FString Category, const FVector Center, float Radius,
	int32 Segments, FLinearColor LineColor, float Duration, float Thickness)
{
	CHECK_IS_ALLOWED_CATEGORY(Category);
	CHECK_IS_CATEGORY_EMPTY(Category);


	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Center, Radius, Segments, LineColor, Duration, Thickness);
}

void UDLDrawDebugSubsystem::DrawDebugCapsule(const EDrawDebugCategory Category, const FVector Center, float HalfHeight,
	float Radius, const FRotator Rotation, FLinearColor LineColor, float Duration, float Thickness)
{
	CHECK_IS_ALLOWED_CATEGORY(Category);

	UKismetSystemLibrary::DrawDebugCapsule(GetWorld(), Center, HalfHeight, Radius, Rotation, LineColor, Duration, Thickness);
}

void UDLDrawDebugSubsystem::DrawDebugCapsule_CustomCategory(FString Category, const FVector Center,
	float HalfHeight, float Radius, const FRotator Rotation, FLinearColor LineColor, float Duration, float Thickness)
{
	CHECK_IS_ALLOWED_CATEGORY(Category);
	CHECK_IS_CATEGORY_EMPTY(Category);


	UKismetSystemLibrary::DrawDebugCapsule(GetWorld(), Center, HalfHeight, Radius, Rotation, LineColor, Duration, Thickness);
}

void UDLDrawDebugSubsystem::DrawDebugLine(const EDrawDebugCategory Category, const FVector LineStart,
	const FVector LineEnd, FLinearColor LineColor, float Duration, float Thickness)
{
	CHECK_IS_ALLOWED_CATEGORY(Category);

	UKismetSystemLibrary::DrawDebugLine(GetWorld(), LineStart, LineEnd, LineColor, Duration, Thickness);
}

void UDLDrawDebugSubsystem::DrawDebugLine_CustomCategory(FString Category, const FVector LineStart,
	const FVector LineEnd, FLinearColor LineColor, float Duration, float Thickness)
{
	CHECK_IS_ALLOWED_CATEGORY(Category);
	CHECK_IS_CATEGORY_EMPTY(Category);


	UKismetSystemLibrary::DrawDebugLine(GetWorld(), LineStart, LineEnd, LineColor, Duration, Thickness);
}

#if WITH_EDITORONLY_DATA
static FDelegateHandle DelegateHandle;
#endif

void UDLDrawDebugSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if WITH_EDITOR
	auto* Setting = UDLDrawDebugSetting::Get();
	if (Setting)
	{
		if (DelegateHandle.IsValid())
		{
			Setting->OnSettingChanged().Remove(DelegateHandle);
		}
		DelegateHandle = Setting->OnSettingChanged().AddLambda(
	[](UObject* Obj, struct FPropertyChangedEvent& ChangedEvent)
		{
			CHECK_IS_MANAGER_VALID();

			DrawDebugManager::Get()->LoadDefaultSetting();
		}
		);
	}

#endif

	static bool bIsInitialized = false;

	if (bIsInitialized)
	{
		return;
	}

	bIsInitialized = DrawDebugManager::Get()->LoadDefaultSetting();
}

void UDLDrawDebugSubsystem::Deinitialize()
{
	Super::Deinitialize();
#if WITH_EDITOR
	auto* Setting = UDLDrawDebugSetting::Get();
	if (Setting)
	{
		if (DelegateHandle.IsValid())
		{
			Setting->OnSettingChanged().Remove(DelegateHandle);
		}
	}
#endif
}


