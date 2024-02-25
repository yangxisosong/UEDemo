// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Modules/ModuleManager.h"

typedef TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> FHttpRequestPtr;
typedef TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> FHttpResponsePtr;

class FToolBarBuilder;
class FMenuBuilder;

class FGitLockModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
private:

	void RegisterMenus();

	void OnObjectSaved(UObject* SavedObject);

	void HTTPRequest_IsReady();
	void HandleRequest_IsReady(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	bool InitGitData();
	void HTTPRequest_SetLock(FString item_id);
	void HandleRequest_SetLock(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void HTTPRequest_CleanLock();

private:
	FText Title = FText::FromString(TEXT("GitLock"));
	bool AutoEnable = true;
	bool EnableLock = false;
	TArray<FString> ProcessingRequestItems;
	TArray<FString> IgnoreItems;
	TArray<FString> LockedItems;
	FString ServerURL = "http://dlpkg.ops.ilongyuan.cn:4321";
	//FString ServerURL = "http://127.0.0.1:8000";
	FString UserID = "NotFound";
	FString GitBranch = "NotFound";
	FString GitHashId = "NotFound";
	FDelegateHandle OnObjectSavedHandle;
	TSharedPtr<class FUICommandList> PluginCommands;
};
