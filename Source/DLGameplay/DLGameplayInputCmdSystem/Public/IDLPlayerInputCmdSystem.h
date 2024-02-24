#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTags.h"
#include "DLPlayerInputCmd.h"
#include "GameplayEffectTypes.h"
#include "IDLPlayerInputCmdSystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDLGamePlayInputCmd, Log, All);


UCLASS()
class DLGAMEPLAYINPUTCMDSYSTEM_API UDLInputCmdSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
		void PushInputCmd(UDLPlayerInputCmdBase* InputCmd);

	UFUNCTION(BlueprintCallable)
		void AddAllowCmdTags(const FGameplayTagContainer& InTags, const FString& DebugStr);

	UFUNCTION(BlueprintCallable)
		void RemoveCmdTags(const FGameplayTagContainer& InTags, const FString& DebugStr);

	UFUNCTION(BlueprintCallable)
		UDLPlayerInputCmdBase* GetCurrentCacheCmd()const;

	UFUNCTION(BlueprintCallable)
		void ClearInputCmd();

	UFUNCTION(BlueprintCallable)
		bool ExecCmd(UDLPlayerInputCmdBase* InputCmd, const FString& ContextDebugStr);

	UFUNCTION(BlueprintCallable)
		void TryCheckMovementCmd();

	UFUNCTION(BlueprintCallable)
		FString AllowExecCmdToString() const;
public:

	static UDLInputCmdSubsystem* Get(const APlayerController* PC);
	static UDLInputCmdSubsystem* Get(const ULocalPlayer* LocalPlayer);
	static UDLInputCmdSubsystem* Get(const UObject* WorldObjectContext);

	class UDLPlayerInputCmdBase* AllocateCmd(TSubclassOf<UDLPlayerInputCmdBase> Class, const struct FDLPlayerInputCmdConstructBase* Data);

	void OnClientSetup();

	void OnClientClear();

protected:

	virtual void Deinitialize() override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:

	UPROPERTY(Transient)
		UDLPlayerInputCmdBase* CacheInputCmd;

	UPROPERTY(Transient)
		TArray<UDLPlayerInputCmdBase*> CmdPool;

	UPROPERTY(Transient)
		int32 ViewportHandle = INDEX_NONE;

	UPROPERTY(Transient)
		bool bIsInit = false;

protected:

	FGameplayTagCountContainer AllowExecInputCmdTags;

};



UCLASS(Config = Game)
class DLGAMEPLAYINPUTCMDSYSTEM_API UDLPlayerInputCmdFactory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = PlayerInputCmd, meta = (AutoCreateRefTerm = Arg, WorldContext = WorldContextObject))
		static UDLPlayerInputCmdBase* CreateMovementCmd(UObject* WorldContextObject, const FDLPlayerInputCmdConstructMovement& Arg);


	UFUNCTION(BlueprintCallable, Category = PlayerInputCmd, meta = (AutoCreateRefTerm = Arg, WorldContext = WorldContextObject))
		static UDLPlayerInputCmdBase* CreateActivateAbilityCmd(UObject* WorldContextObject, const FDLPlayerInputCmdConstructAbility& Arg);


	UFUNCTION(BlueprintCallable, Category = PlayerInputCmd, meta = (AutoCreateRefTerm = Arg, WorldContext = WorldContextObject))
		static UDLPlayerInputCmdBase* CreateRollCmd(UObject* WorldContextObject, const FDLPlayerInputCmdConstructRoll& Arg);


	UFUNCTION(BlueprintCallable, Category = PlayerInputCmd, meta = (AutoCreateRefTerm = Arg, WorldContext = WorldContextObject))
		static UDLPlayerInputCmdBase* CreateJumpCmd(UObject* WorldContextObject, const FDLPlayerInputCmdConstructJump& Arg);


	UFUNCTION(BlueprintCallable, Category = PlayerInputCmd, meta = (AutoCreateRefTerm = Arg, WorldContext = WorldContextObject))
		static UDLPlayerInputCmdBase* CreateSwitchSneakStateCmd(UObject* WorldContextObject);

private:

	static  UDLPlayerInputCmdBase* AllocateCmd(const UObject* WorldContextObject, FName FunctionName, const struct FDLPlayerInputCmdConstructBase* Data);

public:

	// 工厂方法的名字 -> Class
	UPROPERTY(Config, EditAnywhere)
		TMap<FName, TSoftClassPtr<UDLPlayerInputCmdBase>> CmdClassConfig;
};
