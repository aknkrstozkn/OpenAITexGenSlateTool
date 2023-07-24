// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Modules/ModuleManager.h"
#include "TextureGeneratorJsonSerializable.h"

class SMultiLineEditableTextBox;
class SEditableTextBox;

class FTextureGeneratorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:

	void OnImageDownloadComplete(FHttpRequestPtr /*Request*/, FHttpResponsePtr /*Response*/, bool /*bConnectedSuccessfully*/);
	void OnAPIRequestComplete(FHttpRequestPtr /*Request*/, FHttpResponsePtr /*Response*/, bool /*bConnectedSuccessfully*/);

	FReply OnGenerateClicked();
	FReply OnPathClicked();
	void OnSpawnWindow();

	TSharedPtr<SMultiLineEditableTextBox> PromptEditableBox;
	TSharedPtr<SEditableTextBox> TextureSizeEditableBox;
	TSharedPtr<SEditableTextBox> TextureNameEditableBox;

	bool TryCreateTextureFromPngData(const TArray<uint8>& PngData) const;
	void PostDallEHttpRequest(const FDallEPrompt& DallEPrompt);
	void GetImageDownloadHttpRequest(const FString& Url);

	bool bLoadingImage = false;
	FString TextureSavePath = TEXT("/Game");
	TSharedPtr<SWindow> MainWindow;
};
