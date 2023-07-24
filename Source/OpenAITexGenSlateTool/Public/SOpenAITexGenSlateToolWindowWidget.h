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

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

class SMultiLineEditableTextBox;
class SEditableTextBox;

class SOpenAITexGenSlateToolWindowWidget : public SCompoundWidget
{
public:
	DECLARE_DELEGATE(FOnGenerateClicked)

	SLATE_BEGIN_ARGS(SOpenAITexGenSlateToolWindowWidget) {}
		SLATE_EVENT(FOnGenerateClicked, OnGenerateClicked)
		SLATE_ARGUMENT(TSharedPtr<SWindow>, MainWindow)
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);

	void SetLoading(bool bLoading) { bLoadingImage = bLoading; }
	
	const FString& GetTexturePath() const { return TextureSavePath; }
	const FString& GetTexturePrompt() const { return PromptEditableBox->GetText().ToString(); }
	const FString& GetTextureSize() const { return TextureSizeEditableBox->GetText().ToString(); }
	const FString& GetTextureName() const { return TextureNameEditableBox->GetText().ToString(); }
	
private:

	bool bLoadingImage = false;
	FString TextureSavePath = TEXT("/Game");
	
	FOnGenerateClicked OnGenerateClickedDelegate;
	TSharedPtr<SWindow> MainWindow;
	FReply OnPathClicked();
	
	TSharedPtr<SMultiLineEditableTextBox> PromptEditableBox;
	TSharedPtr<SEditableTextBox> TextureSizeEditableBox;
	TSharedPtr<SEditableTextBox> TextureNameEditableBox;	
};
