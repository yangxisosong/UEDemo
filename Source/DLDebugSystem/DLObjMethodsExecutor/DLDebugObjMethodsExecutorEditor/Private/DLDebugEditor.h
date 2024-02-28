#pragma once
#include "CoreMinimal.h"
#include "DLDebugSystem/DLObjMethodsExecutor/DLDebugObjMethodsExecutor/Public/DLDebugObjMethodsExecutor.h"
#include "DLDebugUMG.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"
#include "DLDebugEditor.generated.h"

USTRUCT(BlueprintType)
struct FMyStruct
{
	GENERATED_BODY()
public:

	UPROPERTY()
		int32 Number;
	UPROPERTY()
		FString Name;
	UPROPERTY()
		FVector Pos;
};


UCLASS(BlueprintType)
class UFunStruct :public  UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
		int32 Number2;
	UPROPERTY(EditDefaultsOnly)
		FString Name2;
	UPROPERTY(EditDefaultsOnly)
		FVector Pos2;

	struct TestStruct
	{
		int32 Number;

		FString Name;

		FVector Pos;
	};

	TestStruct MyTest;

	void CopyValue()
	{
		MyTest.Number = Number2;
		MyTest.Name = Name2;
		MyTest.Pos = Pos2;
	};
	void* GetStruct()
	{
		CopyValue();
		return &MyTest;
	};
};



UCLASS(BlueprintType)
class DLDEBUGOBJMETHODSEXECUTOREDITOR_API UDetailInfo:public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TSubclassOf<UObject> ClassFilter;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TEnumAsByte<ENetRole> NetRole;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		int32 Num=10;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FDLDebugObjectFilter DLObjectFilter;

	UPROPERTY(EditDefaultsOnly)
		FMyStruct MyStruct;

		UFunction* FunTest;


	UFUNCTION()
	void TestFun(int32 Number, FString Name, FVector Pos);

	UFUNCTION()
	void TestFun2(UFunStruct* Param);

	UFUNCTION(meta=(MyTest = "FunStruct"))
	void TestFun3(int32 Number, FString Name, FVector Pos);
	
};

//UDetailInfo::UDetailInfo(const FObjectInitializer& ObjectInitializer)
//	:Super(ObjectInitializer)
//{
//	
//}


class FDLDebugEditor :public FNotifyHook
{
public:
	FDLDebugEditor();

	void Init(int32 Num);

	void CreatNewTab();

	void GetWorldInfo();

	//virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
							  FEditPropertyChain* PropertyThatChanged) override;

private:
	TSharedPtr<IDetailsView> CreatDetailTab();

	void OnSelectClass(const UClass* SelectInfo);

	void OnButtonClick();
private:
	TWeakObjectPtr<UDLDebugUMG> UserWidget;

	TArray<int32> PIEID;

	TSharedPtr<FTabManager::FLayout> EditorLayout;

	TArray<TSharedPtr<SDockTab>> SDockTabArray;

	TSharedPtr<IDetailsView> DetailsView;

	TSharedPtr<FTabManager> EditorTabManager;

	UPROPERTY()
	TWeakObjectPtr<UDetailInfo> DetailInfo;

	UFunStruct* TestStruct;
};
