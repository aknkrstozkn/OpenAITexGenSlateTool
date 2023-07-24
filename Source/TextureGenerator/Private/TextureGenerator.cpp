// Copyright Epic Games, Inc. All Rights Reserved.

#include "TextureGenerator.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "TextureGeneratorSettings.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Dialogs/DlgPickPath.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializerMacros.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/Notifications/SProgressBar.h"

static const FName TextureGeneratorName("TextureGenerator");

#define LOCTEXT_NAMESPACE "FTextureGeneratorModule"

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

void FTextureGeneratorModule::StartupModule()
{
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([this]()
	{
		UToolMenu* WidgetsMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
		FToolMenuSection& WidgetsSection = WidgetsMenu->AddSection(TextureGeneratorName, LOCTEXT("TextureGenerator_Section", "Texture Generator"));

		const FToolMenuEntry WidgetsEntry = FToolMenuEntry::InitToolBarButton(
			TextureGeneratorName,
			FToolUIActionChoice(FExecuteAction::CreateRaw(this, &FTextureGeneratorModule::OnSpawnWindow)),
			LOCTEXT("TextureGenerator_Section", "Texture Generator"),
			LOCTEXT("TextureGenerator_Tooltip", "Generate AI based Textures"));
		WidgetsSection.AddEntry(WidgetsEntry);
	}));
}

void FTextureGeneratorModule::ShutdownModule()
{
	UToolMenu* WidgetsMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	WidgetsMenu->RemoveSection(TextureGeneratorName);
}

void FTextureGeneratorModule::OnSpawnWindow()
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
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SProgressBar)
			.Visibility_Lambda([this]() { return bLoadingImage ? EVisibility::Visible : EVisibility::Collapsed; })
		]
		
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 16.f)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(16.f, 0.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("TxtGenerationTextureDefinition", "Texture Definition"))
			]

			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(16.f, 0.f)
			[
				SNew(SBox)
				.MinDesiredHeight(120.f)
				.MinDesiredWidth(200.f)
				[
					SAssignNew(PromptEditableBox, SMultiLineEditableTextBox)
					.IsEnabled_Lambda([this]()
					{
						return !bLoadingImage;
					})
					.AllowMultiLine(true)
					.HintText(LOCTEXT("TxtGenerationHint", "Relistic Green Grass"))
				]
			]
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(16.f, 16.f)
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Top)
		[
			SNew(SButton)
			.IsEnabled_Lambda([this]()
			{
				return !bLoadingImage;
			})
			.OnClicked_Raw(this, &FTextureGeneratorModule::OnGenerateClicked)
			.Text(LOCTEXT("GenerateButton", "Generate"))
		]			
			
		+SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			SNew(SExpandableArea)
			.InitiallyCollapsed(true)
			.AreaTitle(LOCTEXT("TxtGenerationSettings", "Settings"))
			.BodyContent()
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(16.f, 8.f)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SizeTextLabel", "Size"))
					]
	
					+SHorizontalBox::Slot()
					.FillWidth(1.f)
					.Padding(0.f, 8.f)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					[
						SAssignNew(TextureSizeEditableBox, SEditableTextBox)
						.IsEnabled_Lambda([this]()
						{
							return !bLoadingImage;
						})
						.Text(LOCTEXT("SizeText", "1024x1024"))
					]						
				]

				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(16.f, 8.f)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("TextureNameTextLabel", "Name"))
					]
	
					+SHorizontalBox::Slot()
					.FillWidth(1.f)
					.Padding(0.f, 8.f)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					[
						SAssignNew(TextureNameEditableBox, SEditableTextBox)
						.IsEnabled_Lambda([this]()
						{
							return !bLoadingImage;
						})
						.Text(LOCTEXT("TextureNameText", "TestTexture"))
					]						
				]
				
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				[								
					SNew(SBox)
					.Padding(16.f, 8.f)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					[
						SNew(SButton)
						.IsEnabled_Lambda([this]()
						{
							return !bLoadingImage;
						})
						.OnClicked_Raw(this, &FTextureGeneratorModule::OnPathClicked)
						.Text_Lambda([this]()
						{
							return FText::FromString(TextureSavePath);
						})
					]
				]				
			]
		]	
	];

	MainWindow->GetOnWindowClosedEvent().AddLambda([this](const TSharedRef<SWindow>&)
	{
		MainWindow = nullptr;
	});

	FSlateApplication::Get().AddWindow(MainWindow.ToSharedRef());	
}

void FTextureGeneratorModule::PostDallEHttpRequest(const FDallEPrompt& DallEPrompt)
{
	bLoadingImage = true;
	const TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FTextureGeneratorModule::OnAPIRequestComplete);
	HttpRequest->SetVerb(TEXT("POST"));

	HttpRequest->SetURL(TEXT("https://api.openai.com/v1/images/generations"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + GetDefault<UTextureGeneratorSettings>()->ApiKey);
	
	HttpRequest->SetContentAsString(DallEPrompt.ToJson());
	
	HttpRequest->ProcessRequest();
}

void FTextureGeneratorModule::GetImageDownloadHttpRequest(const FString& Url)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FTextureGeneratorModule::OnImageDownloadComplete);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(Url);
	HttpRequest->ProcessRequest();
}

void FTextureGeneratorModule::OnAPIRequestComplete(FHttpRequestPtr /*Request*/, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if(!bConnectedSuccessfully || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		bLoadingImage = false;
		ShowNotification("Texture Generation Failed", false);
		UE_LOG(LogTemp, Warning, TEXT("Api request failed"));
		return;
	}
	
	FDallEResponse DallEResponse;
	if(!DallEResponse.FromJson(Response->GetContentAsString()) || DallEResponse.UrlArray.IsEmpty())
	{
		bLoadingImage = false;
		ShowNotification("Texture Generation Failed", false);
		UE_LOG(LogTemp, Warning, TEXT("Response couldn't parse"));
		return;
	}	

	GetImageDownloadHttpRequest(DallEResponse.UrlArray[0].Url);
}

bool FTextureGeneratorModule::TryCreateTextureFromPngData(const TArray<uint8>& PngData) const
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

		FString PackageName = TextureSavePath;
		const FString TextureName = TextureNameEditableBox->GetText().ToString();
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

void FTextureGeneratorModule::OnImageDownloadComplete(FHttpRequestPtr /*Request*/, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	ON_SCOPE_EXIT { bLoadingImage = false; };
	
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

	ShowNotification(FString::Printf(TEXT("Texture Successfully Generated at %s"), *(TextureSavePath / TextureNameEditableBox->GetText().ToString())) , true);
}

FReply FTextureGeneratorModule::OnGenerateClicked()
{
	FDallEPrompt DallEPrompt;
	DallEPrompt.Prompt = PromptEditableBox->GetText().ToString();
	DallEPrompt.ImageSize = TextureSizeEditableBox->GetText().ToString();

	PostDallEHttpRequest(DallEPrompt);
	
	return FReply::Handled();
}

FReply FTextureGeneratorModule::OnPathClicked()
{
	const TSharedRef<SDlgPickPath> DlgPickPath = SNew(SDlgPickPath)
			.Title(LOCTEXT("TextureSavePath", "Save Texture to Path"))
			.DefaultPath(FText::FromString(TEXT("/Game")));
	
	FSlateApplication::Get().AddModalWindow(DlgPickPath, MainWindow);
	FTextureGeneratorModule::TextureSavePath = DlgPickPath->GetPath().ToString();
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTextureGeneratorModule, TextureGenerator)