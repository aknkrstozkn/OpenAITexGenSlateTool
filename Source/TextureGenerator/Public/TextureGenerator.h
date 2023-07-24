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
	
	bool TryCreateTextureFromPngData(const TArray<uint8>& PngData) const;
	void PostDallEHttpRequest(const FDallEPrompt& DallEPrompt);
	void GetImageDownloadHttpRequest(const FString& Url);

	bool bLoadingImage = false;
	FString TextureSavePath = TEXT("/Game");

	// Widget
	FReply OnGenerateClicked();
	FReply OnPathClicked();
	
	void OnSpawnWindow();
	
	TSharedPtr<SMultiLineEditableTextBox> PromptEditableBox;
	TSharedPtr<SEditableTextBox> TextureSizeEditableBox;
	TSharedPtr<SEditableTextBox> TextureNameEditableBox;

	TSharedPtr<SWindow> MainWindow;
};
