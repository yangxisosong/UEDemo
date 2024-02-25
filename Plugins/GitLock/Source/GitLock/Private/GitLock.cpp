// Copyright Epic Games, Inc. All Rights Reserved.

#include "GitLock.h"
#include "GitLockStyle.h"
#include "GitLockCommands.h"
#include "HttpModule.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "Json.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"


static const FName GitLockTabName("GitLock");

#define LOCTEXT_NAMESPACE "FGitLockModule"

void FGitLockModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FGitLockStyle::Initialize();
	FGitLockStyle::ReloadTextures();

	FGitLockCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FGitLockCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FGitLockModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FGitLockModule::RegisterMenus));

	OnObjectSavedHandle = FCoreUObjectDelegates::OnObjectSaved.AddRaw(this, &FGitLockModule::OnObjectSaved);
	if (AutoEnable)
	{
		InitGitData();
	}
}

void FGitLockModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FGitLockStyle::Shutdown();

	FGitLockCommands::Unregister();
	FCoreUObjectDelegates::OnObjectSaved.Remove(OnObjectSavedHandle);
}

void FGitLockModule::PluginButtonClicked()
{
	ProcessingRequestItems.Empty();
	IgnoreItems.Empty();
	if (!EnableLock)
	{
		HTTPRequest_IsReady();
	}
	else
	{
		EnableLock = false;
		auto ret = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(TEXT("已关闭GitLock功能,是否解锁所有文件？")), &Title);
		if (ret == EAppReturnType::Yes)
		{
			HTTPRequest_CleanLock();
		}
	}
}

void FGitLockModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");
		UToolMenu* DLToolsMenu = Menu->AddSubMenu(FToolMenuOwner(Menu->GetFName()),FName("DLTools"),FName("DLTools"),FText::FromString("DLTools"));
		FToolMenuSection& Section = DLToolsMenu->FindOrAddSection("DLTools");
		Section.AddMenuEntryWithCommandList(FGitLockCommands::Get().PluginAction, PluginCommands);
		
	}

	// {
	// 	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
	// 	{
	// 		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
	// 		{
	// 			FToolMenuEntry& Entry = Section.AddEntry(
	// 				FToolMenuEntry::InitToolBarButton(FGitLockCommands::Get().PluginAction));
	// 			Entry.SetCommandList(PluginCommands);
	// 		}
	// 	}
	// }
}

void FGitLockModule::OnObjectSaved(UObject* SavedObject)
{
	if (IsRunningCommandlet()) return;
	if (!EnableLock) return;
	// Ensure the saved object is a non-UWorld asset (UWorlds are handled separately)
	if (!SavedObject->IsA<UWorld>() && SavedObject->IsAsset())
	{
		if (GEngine->IsAutosaving()) return;
		FString PathName = SavedObject->GetPathName();
		if (PathName.EndsWith("_C")) return;
		FString Path = FPaths::GetPath(PathName);
		Path = Path.Replace(TEXT("/Game/"),TEXT("Content/"));
		FString Name = FPaths::GetBaseFilename(PathName);
		HTTPRequest_SetLock(Path + "/" + Name + ".uasset");
	}
	if (SavedObject->IsA<UWorld>() && SavedObject->IsAsset())
	{
		if (GEngine->IsAutosaving()) return;
		FString PathName = SavedObject->GetPathName();
		if (PathName.EndsWith("_C")) return;
		FString Path = FPaths::GetPath(PathName);
		Path = Path.Replace(TEXT("/Game/"),TEXT("Content/"));
		FString Name = FPaths::GetBaseFilename(PathName);
		HTTPRequest_SetLock(Path + "/" + Name + ".umap");
	}
}

void FGitLockModule::HTTPRequest_IsReady()
{
	//创建Http 请求
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetVerb("GET");
	//设置请求地址
	FString URL = FString::Printf(TEXT("%s/isready"), *ServerURL);
	HttpRequest->SetURL(URL);

	//绑定回调
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FGitLockModule::HandleRequest_IsReady);

	//超时时间
	HttpRequest->SetTimeout(1);

	//发送请求
	HttpRequest->ProcessRequest();
}

void FGitLockModule::HandleRequest_IsReady(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!HttpRequest.IsValid() || !HttpResponse.IsValid())
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("连接服务器失败！")), &Title);
		return;
	}
	int32 responseCode = HttpResponse->GetResponseCode();
	if (bSucceeded && EHttpResponseCodes::IsOk(responseCode))
	{
		if (InitGitData())
		{
			FMessageDialog::Open(EAppMsgType::Ok,
								 FText::FromString(
									 FString::Printf(TEXT("服务器正常，已开始GitLock功能！\n使用用户名：%s\n当前分支：%s\n"), *UserID,*GitBranch)), &Title);
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("初始化Git信息错误！")), &Title);
		}
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("连接服务器失败！")), &Title);
	}
}

bool FGitLockModule::InitGitData()
{
	FString GitExe = FPaths::Combine(FPaths::ProjectDir(),TEXT(".."),TEXT("git/cmd/git.exe"));
	GitExe = FPaths::ConvertRelativePathToFull(GitExe);
	int32 ReturnCode;
	FString OutStdOut;
	FString OutStdErr;
	if (FPaths::FileExists(*GitExe))
	{
		bool getErr = false;
		FPlatformProcess::ExecProcess(*GitExe, TEXT("config user.name"), &ReturnCode, &OutStdOut, &OutStdErr);
		if (ReturnCode == 0)
		{
			UserID = OutStdOut.Replace(TEXT("\n"),TEXT(""));
		}
		else
		{
			getErr = true;
		}
		
		FPlatformProcess::ExecProcess(*GitExe, *FString::Printf(TEXT("-C %s branch --show-current"),*FPaths::ProjectDir()), &ReturnCode, &OutStdOut, &OutStdErr);
		if (ReturnCode == 0)
		{
			GitBranch = OutStdOut.Replace(TEXT("\n"),TEXT(""));
		}
		else
		{
			getErr = true;
		}
		FPlatformProcess::ExecProcess(*GitExe, TEXT("log --pretty=format:'%H' -n 1"), &ReturnCode, &OutStdOut,
									  &OutStdErr);
		if (ReturnCode == 0)
		{
			GitHashId = OutStdOut.Replace(TEXT("\n"),TEXT("")).Replace(TEXT("'"),TEXT(""));
		}
		else
		{
			getErr = true;
		}
		if (getErr)
		{
			//FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("调用Git 出现错误")), &Title);
			UE_LOG(LogTemp, Log, TEXT("GitLock 调用Git 出现错误!!"));
		}
		else
		{
			EnableLock = true;
		}
	}
	else
	{
		//FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("没有找到Git.exe")), &Title);
		UE_LOG(LogTemp, Log, TEXT("GitLock 没有找到Git.exe!!"));
	}

	return EnableLock;
}


void FGitLockModule::HTTPRequest_SetLock(FString item_id)
{
	if (LockedItems.Find(item_id) > INDEX_NONE) return;
	if (ProcessingRequestItems.Find(item_id) > INDEX_NONE) return;
	ProcessingRequestItems.AddUnique(item_id);
	/**json 数据**/
	FString Serverdata;
	TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<
		TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Serverdata);
	JsonWriter->WriteObjectStart();
	JsonWriter->WriteValue("item_id", item_id);
	JsonWriter->WriteValue("locked", true);
	JsonWriter->WriteValue("user", UserID);
	JsonWriter->WriteValue("branch", GitBranch);
	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	//创建Http 请求
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();

	//设置Header
	HttpRequest->SetHeader("accept", "application/json;charset=UTF-8");
	HttpRequest->SetHeader("Content-Type", "application/json;charset=UTF-8");

	HttpRequest->SetVerb("POST");
	//设置请求地址
	FString URL = FString::Printf(TEXT("%s/setlock/%s"), *ServerURL, *GitHashId);
	HttpRequest->SetURL(URL);
	//设置请求发送的数据
	HttpRequest->SetContentAsString(Serverdata);

	//绑定回调
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FGitLockModule::HandleRequest_SetLock);
	//发送请求
	HttpRequest->ProcessRequest();
}

void FGitLockModule::HandleRequest_SetLock(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
                                           bool bSucceeded)
{
	if (!HttpRequest.IsValid() || !HttpResponse.IsValid())
	{
		return;
	}

	FString MessageBody = HttpResponse->GetContentAsString();

	FString item_id;
	bool locked = false;
	FString user;
	FString branch;
	bool result = false;
	bool committed = false;

	int32 responseCode = HttpResponse->GetResponseCode();
	if (bSucceeded && EHttpResponseCodes::IsOk(responseCode))
	{
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(MessageBody);
		TSharedPtr<FJsonObject> JsonObject;
		TSharedPtr<FJsonObject> TempJson;
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			item_id = JsonObject->GetStringField("item_id");
			locked = JsonObject->GetBoolField("locked");
			user = JsonObject->GetStringField("user");
			branch = JsonObject->GetStringField("branch");
			result = JsonObject->GetBoolField("result");
			committed = JsonObject->GetBoolField("committed");
			ProcessingRequestItems.Remove(item_id);

			if (!result)
			{
				if (locked && user != UserID)
				{
					if (IgnoreItems.Find(item_id) == INDEX_NONE)
					{
						auto ret = FMessageDialog::Open(EAppMsgType::YesNo,
						                                FText::FromString(FString::Printf(
							                                TEXT("锁定失败\n\n%s\n\n已经被{%s}在{%s}分支中锁定了\n\n忽略提示？"),
							                                *item_id, *user,*branch)), &Title);
						if (ret == EAppReturnType::Yes)
						{
							IgnoreItems.AddUnique(item_id);
						}
					}
				}

				if (committed)
				{
					if (IgnoreItems.Find(item_id) == INDEX_NONE)
					{
						auto ret = FMessageDialog::Open(EAppMsgType::YesNo,
														FText::FromString(FString::Printf(
															TEXT("锁定失败\n\n%s\n\n服务器的{%s}分支中{%s}提交了新版本\n\n忽略提示？"),
															*item_id,*branch,*user)), &Title);
						if (ret == EAppReturnType::Yes)
						{
							IgnoreItems.AddUnique(item_id);
						}
					}
				}
			}
			else
			{
				LockedItems.AddUnique(item_id);
			}
		}
	}
}

void FGitLockModule::HTTPRequest_CleanLock()
{
	//创建Http 请求
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();

	//设置Header
	HttpRequest->SetHeader("Content-Type", "application/json;charset=UTF-8");

	HttpRequest->SetVerb("POST");
	//设置请求地址
	FString URL = FString::Printf(TEXT("%s/cleanlock/%s"), *ServerURL, *UserID);
	HttpRequest->SetURL(URL);
	
	//发送请求
	HttpRequest->ProcessRequest();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGitLockModule, GitLock)
