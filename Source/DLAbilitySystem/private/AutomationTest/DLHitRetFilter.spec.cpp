#include "AbilitySystemAutomationTest.h"
#include "Misc/AutomationTest.h"
#include "DLHitRetFilter.h"


#if WITH_DEV_AUTOMATION_TESTS


struct FGCTestData : public FGCObject
{
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObject(SelectorFilter);
		Collector.AddReferencedObject(World);
		Collector.AddReferencedObject(TargetActor);
	}


	virtual FString GetReferencerName() const override
	{
		return TEXT("FGCTestData DLUnitSelectorFilterDefaultSpec");
	}

	FGCTestData()
	{
		World = UWorld::CreateWorld(EWorldType::Game, false);
		FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
		WorldContext.SetCurrentWorld(World);

		const FURL URL;
		World->InitializeActorsForPlay(URL);
		World->BeginPlay();

		SelectorFilter = NewObject<UDLUnitSelectorFilterDefault>(World);

		TargetActor = World->SpawnActor<AAbilitySystemTestCharacter>();
		InsActor = World->SpawnActor<AAbilitySystemTestCharacter>();

		Context.Instigator = InsActor;
	}

public:

	FUnitSelectorFilterContext Context;

	AAbilitySystemTestCharacter* TargetActor = nullptr;
	AAbilitySystemTestCharacter* InsActor = nullptr;

	UWorld* World = nullptr;
	UDLUnitSelectorFilterDefault* SelectorFilter = nullptr;
};


BEGIN_DEFINE_SPEC(FDLUnitSelectorFilterDefaultSpec,
	"DL.AbilitySystem.DLUnitSelectorFilterDefault",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

	TSharedPtr<FGCTestData> TestData;

END_DEFINE_SPEC(FDLUnitSelectorFilterDefaultSpec)




void FDLUnitSelectorFilterDefaultSpec::Define()
{
	static FGameplayTag TagTestA = FGameplayTag::RequestGameplayTag(TEXT("DLUnitSelectorFilterDefaultSpec.TestA"));
	static FGameplayTag TagTestB = FGameplayTag::RequestGameplayTag(TEXT("DLUnitSelectorFilterDefaultSpec.TestB"));
	static FGameplayTag TagTestC = FGameplayTag::RequestGameplayTag(TEXT("DLUnitSelectorFilterDefaultSpec.TestC"));

	BeforeEach([this]
	{
		TestData = MakeShared<FGCTestData>();
	});

	Describe(TEXT("FilterHitUnit"), [this]
	{
		Describe(TEXT("当给目标Actor指定 Tag 时"), [this]
		{
			BeforeEach([this]
			{
				const auto TargetActor = TestData->TargetActor;
				TargetActor->MyTags.AddTag(TagTestA);
				TargetActor->MyTags.AddTag(TagTestB);

			});

			It(TEXT("应该在没有指定 RequirementTags, 通过 TargetActor"), [this]
			{
				FHitResult Ret;
				Ret.HitObjectHandle = TestData->TargetActor;
				
				const bool Pass = !TestData->SelectorFilter->FilterHitUnit(TestData->Context, Ret);
				TestTrue(TEXT("应该能够通过过滤器"), Pass);
			});

			It(TEXT("应该在指定 RequirementTags IgnoreTags->TagTestC  RequireTags->TagTestB 通过 TargetActor"), [this]
			{
				FHitResult Ret;
				Ret.HitObjectHandle = TestData->TargetActor;

				TestData->SelectorFilter->FilterData.RequirementTags.IgnoreTags = TagTestC.GetSingleTagContainer();
				TestData->SelectorFilter->FilterData.RequirementTags.RequireTags = TagTestB.GetSingleTagContainer();

				const bool Pass = !TestData->SelectorFilter->FilterHitUnit(TestData->Context, Ret);
				TestTrue(TEXT("应该能够通过过滤器"), Pass);
			});

			AfterEach([this]
			{

			});
		});
	});

	AfterEach([this]
	{
		TestData = nullptr;
	});

}


#endif