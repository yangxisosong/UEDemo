
#include "CoreMinimal.h"
#include "AbilityChain.h"

BEGIN_DEFINE_SPEC(FAbilityChainTest, TEXT("DL.AbilityChainTest"), EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

UAbilityChain* AbilityChain = nullptr;
UAbilitySystemComponent* AbilityComponent = nullptr;

END_DEFINE_SPEC(FAbilityChainTest);


static FString TestJson = TEXT(R"(
{
    "InputID": "None", 
    "AlternateAbilityList": [], 
    "ChildNodes": [
        {
            "InputID": "KeyOne", 
            "AlternateAbilityList": [
                {
                    "Priority": 0, 
                    "AbilityClass": "Blueprint'/Game/GAS/Test/GA_Test.GA_Test'"
                }, 
                {
                    "Priority": 1, 
                    "AbilityClass": "Blueprint'/Game/GAS/Test/GA_Test.GA_Test'"
                }
            ], 
            "ChildNodes": [
                {
                    "InputID": "KeyOne", 
                    "AlternateAbilityList": [
                        {
                            "Priority": 0, 
                            "AbilityClass": "Blueprint'/Game/GAS/Test/GA_Test.GA_Test'"
                        }
                    ], 
                    "ChildNodes": [ ]
                }
            ]
        },
		{
            "InputID": "KeyTwo", 
            "AlternateAbilityList": [
                {
                    "Priority": 0, 
                    "AbilityClass": "Blueprint'/Game/GAS/Test/GA_Test.GA_Test'"
                }, 
                {
                    "Priority": 1, 
                    "AbilityClass": "Blueprint'/Game/GAS/Test/GA_Test.GA_Test'"
                }
            ], 
            "ChildNodes": [
                {
                    "InputID": "KeyOne", 
                    "AlternateAbilityList": [
                        {
                            "Priority": 0, 
                            "AbilityClass": "Blueprint'/Game/GAS/Test/GA_Test.GA_Test'"
                        }
                    ], 
                    "ChildNodes": [ ]
                }
            ]
        }
    ]
}
)");


void FAbilityChainTest::Define()
{
	Describe(TEXT("Test 正常的连招序列"), [this]
	{
		BeforeEach([this]()
		{
			AbilityChain = NewObject<UAbilityChain>();
			AbilityChain->AddToRoot();

			//AbilityChain->BuildAbilityChainTree(TestJson);
		});

		BeforeEach([this]()
		{
			AbilityComponent = NewObject<UAbilitySystemComponent>();
			AbilityComponent->AddToRoot();

		});


		//It(TEXT("二连招 出招 KeyOne->KeyOne 应该结果正常"), [this]
		//{
		//	AbilityChain->SetEnabledAcceptInput(true);
		//	AbilityChain->InjectInput(EPrimaryInputID::KeyOne);
		//
		//	//FGameplayTagContainer* TargetTags = new FGameplayTagContainer;
		//
		//
		//	//UAbilityChain::FCheckOutAbilityArg Arg;
		//	//Arg.ASC = AbilityComponent;
		//	//Arg.TargetTags = new FGameplayTagContainer;
		//	//AbilityChain->CheckoutAlternateAbility(AbilityComponent, )
		//
		//});
		//
		//It(TEXT("三连招 出招 KeyOne->KeyOne->KeyTwo 应该结果正常"), [this]
		//{
		//	AbilityChain->SetEnabledAcceptInput(true);
		//	AbilityChain->InjectInput(EPrimaryInputID::KeyOne);
		//
		//	//FGameplayTagContainer* TargetTags = new FGameplayTagContainer;
		//
		//
		//	//UAbilityChain::FCheckOutAbilityArg Arg;
		//	//Arg.ASC = AbilityComponent;
		//	//Arg.TargetTags = new FGameplayTagContainer;
		//	//AbilityChain->CheckoutAlternateAbility(AbilityComponent, )
		//
		//});

		AfterEach([this]
		{
			AbilityComponent->RemoveFromRoot();
			AbilityComponent = nullptr;
		});

		AfterEach([this]
		{

			AbilityChain->RemoveFromRoot();
			AbilityChain = nullptr;
		});
	});

}