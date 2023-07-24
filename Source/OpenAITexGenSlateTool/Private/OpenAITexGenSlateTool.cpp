/*
* Copyright (C) 2023 Akın Kürşat Özkan <akinkursatozkan@gmail.com>
 * 
 * This file is part of OpenAITexGenSlateTool
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the MIT License as published by
 * the Open Source Initiative, either version 1.0 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * MIT License for more details.
 * 
 * You should have received a copy of the MIT License
 * along with this program. If not, see <https://opensource.org/licenses/MIT>.
 *
 * Source code on GitHub: https://github.com/aknkrstozkn/OpenAITexGenSlateTool
 */

#include "OpenAITexGenSlateTool.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "SOpenAITexGenSlateToolWindowWidget.h"
#include "OpenAITexGenSlateToolSettings.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializerMacros.h"
#include "Widgets/Notifications/SNotificationList.h"

static const FName OpenAITexGenSlateToolName("OpenAITexGenSlateTool");

#define LOCTEXT_NAMESPACE "OpenAITexGenSlateTool"

namespace
{
	void ShowNotification(const FString& Message, bool bIsSuccess)
	{
		FNotificationInfo Info(LOCTEXT("NotificationTitle", "Texture Generator"));
		Info.SubText = FText::FromString(Message);
		Info.ExpireDuration = 3.0f;
		Info.Image = FAppStyle::GetBrush(!bIsSuccess ? "NotificationList.FailImage" : "NotificationList.SuccessImage");
		FSlateNotificationManager::Get().AddNotification(Info);	
	}
}

void FOpenAITexGenSlateToolModule::StartupModule()
{
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([this]()
	{
		UToolMenu* WidgetsMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.LevelEditorToolBar.User"));
		FToolMenuSection& WidgetsSection = WidgetsMenu->AddSection(OpenAITexGenSlateToolName, LOCTEXT("TextureGenerator_Section", "Texture Generator"));

		FToolMenuEntry WidgetsEntry = FToolMenuEntry::InitToolBarButton(
			OpenAITexGenSlateToolName,
			FToolUIActionChoice(FExecuteAction::CreateRaw(this, &FOpenAITexGenSlateToolModule::OnSpawnWindow)),
			LOCTEXT("TextureGenerator_Label", "Texture Generator"),
			LOCTEXT("TextureGenerator_Tooltip", "Generate AI based Textures"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.TextEditor"));
		WidgetsEntry.StyleNameOverride = TEXT("CalloutToolbar");
		
		WidgetsSection.AddEntry(WidgetsEntry);
	}));
}

void FOpenAITexGenSlateToolModule::ShutdownModule()
{
	if (UToolMenus* Menus = UToolMenus::Get())
	{
		UToolMenu* WidgetsMenu = Menus->ExtendMenu(TEXT("LevelEditor.LevelEditorToolBar.User"));
		WidgetsMenu->RemoveSection(OpenAITexGenSlateToolName);
	}	
}

void FOpenAITexGenSlateToolModule::OnSpawnWindow()
{
	if (MainWindow)
	{
		MainWindow->BringToFront();
		return;
	}
	
	MainWindow = SNew(SWindow)
	.Title(LOCTEXT("WindowTitle", "Texture Generator"))
	.SizingRule(ESizingRule::Autosized)
	[
		SAssignNew(TextureGeneratorWindowWidget, SOpenAITexGenSlateToolWindowWidget)
		.OnGenerateClicked_Raw(this, &FOpenAITexGenSlateToolModule::OnGenerateClicked)
		.MainWindow(MainWindow)
	];

	MainWindow->GetOnWindowClosedEvent().AddLambda([this](const TSharedRef<SWindow>&)
	{
		MainWindow = nullptr;
	});

	FSlateApplication::Get().AddWindow(MainWindow.ToSharedRef());	
}

void FOpenAITexGenSlateToolModule::PostDallEHttpRequest(const FDallEPrompt& DallEPrompt)
{
	TextureGeneratorWindowWidget->SetLoading(true);
	const TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FOpenAITexGenSlateToolModule::OnAPIRequestComplete);
	HttpRequest->SetVerb(TEXT("POST"));

	HttpRequest->SetURL(TEXT("https://api.openai.com/v1/images/generations"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + GetDefault<UOpenAITexGenSlateToolSettings>()->ApiKey);
	
	HttpRequest->SetContentAsString(DallEPrompt.ToJson());
	
	HttpRequest->ProcessRequest();
}

void FOpenAITexGenSlateToolModule::GetImageDownloadHttpRequest(const FString& Url)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FOpenAITexGenSlateToolModule::OnImageDownloadComplete);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(Url);
	HttpRequest->ProcessRequest();
}

void FOpenAITexGenSlateToolModule::OnAPIRequestComplete(FHttpRequestPtr /*Request*/, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if(!bConnectedSuccessfully || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		TextureGeneratorWindowWidget->SetLoading(false);
		ShowNotification("Texture Generation Failed", false);
		if(Response->GetResponseCode() == 400)
		{
			UE_LOG(LogTemp, Warning, TEXT("Not enough DALL-E credits for request"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Api request failed"));
		}
		return;
	}
	
	FDallEResponse DallEResponse;
	if(!DallEResponse.FromJson(Response->GetContentAsString()) || DallEResponse.UrlArray.IsEmpty())
	{
		TextureGeneratorWindowWidget->SetLoading(false);
		ShowNotification("Texture Generation Failed", false);
		UE_LOG(LogTemp, Warning, TEXT("Response couldn't parse"));
		return;
	}	

	GetImageDownloadHttpRequest(DallEResponse.UrlArray[0].Url);
}

bool FOpenAITexGenSlateToolModule::TryCreateTextureFromPngData(const TArray<uint8>& PngData) const
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	const TSharedPtr<IImageWrapper> PngImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (!(PngImageWrapper.IsValid() && PngImageWrapper->SetCompressed(PngData.GetData(), PngData.Num())))
	{
		UE_LOG(LogTemp, Warning, TEXT("Png Image Wrapper is not valid!"));
		return false;	
	}
	
	TArray64<uint8> RawImageData;
	if (PngImageWrapper->GetRaw(ERGBFormat::RGBA, 8, RawImageData))
	{
		const int32 Width = PngImageWrapper->GetWidth();
		const int32 Height = PngImageWrapper->GetHeight();

		TArray<FColor> SrcData;
		for(int32 Index = 0; Index < RawImageData.Num(); Index += 4)
		{
			SrcData.Add(FColor(RawImageData[Index], RawImageData[Index + 1], RawImageData[Index + 2], RawImageData[Index + 3]));
		}

		FString PackageName = TextureGeneratorWindowWidget->GetTexturePath();
		const FString TextureName = TextureGeneratorWindowWidget->GetTextureName();
		PackageName = PackageName / TextureName;
		UPackage* Package = CreatePackage(*PackageName);
		if (!Package)
		{
			UE_LOG(LogTemp, Warning, TEXT("Package creation failed!"));
			ShowNotification("Texture Generation Failed", false);
			return false;
		}
		Package->FullyLoad();

		UTexture2D* NewTexture = FImageUtils::CreateTexture2D(Width, Height, SrcData, Package, TextureName, RF_Public | RF_Standalone | RF_MarkAsRootSet, FCreateTexture2DParameters{});
		if (!NewTexture)
		{
			UE_LOG(LogTemp, Warning, TEXT("2D Texture creation failed!"));
			ShowNotification("Texture Generation Failed", false);
			return false;
		}
		FAssetRegistryModule::AssetCreated(NewTexture);

		return true;
	}

	return false;
}

void FOpenAITexGenSlateToolModule::OnImageDownloadComplete(FHttpRequestPtr /*Request*/, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	ON_SCOPE_EXIT { TextureGeneratorWindowWidget->SetLoading(false); };
	
	if(!bConnectedSuccessfully || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Api request failed"));
		ShowNotification("Texture Generation Failed", false);
		return;
	}

	const TArray<uint8>& PngData = Response->GetContent();
	if (PngData.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Png data is empty!"));
		ShowNotification("Texture Generation Failed", false);
		return;	
	}

	if(!TryCreateTextureFromPngData(PngData))
	{
		ShowNotification("Texture Generation Failed", false);
		UE_LOG(LogTemp, Warning, TEXT("Texture creation failed!"));
	}

	ShowNotification(FString::Printf(TEXT("Texture Successfully Generated at %s"), *(TextureGeneratorWindowWidget->GetTexturePath() / TextureGeneratorWindowWidget->GetTextureName())) , true);
}

void FOpenAITexGenSlateToolModule::OnGenerateClicked()
{
	FDallEPrompt DallEPrompt;
	DallEPrompt.Prompt = TextureGeneratorWindowWidget->GetTexturePrompt();
	DallEPrompt.ImageSize = TextureGeneratorWindowWidget->GetTextureSize();

	PostDallEHttpRequest(DallEPrompt);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenAITexGenSlateToolModule, OpenAITexGenSlateTool)