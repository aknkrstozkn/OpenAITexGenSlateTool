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

#include "SOpenAITexGenSlateToolWindowWidget.h"
#include "Dialogs/DlgPickPath.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Notifications/SProgressBar.h"

#define LOCTEXT_NAMESPACE "FTextureGeneratorModule"

FReply SOpenAITexGenSlateToolWindowWidget::OnPathClicked()
{
	const TSharedRef<SDlgPickPath> DlgPickPath = SNew(SDlgPickPath)
			.Title(LOCTEXT("TextureSavePath", "Save Texture to Path"))
			.DefaultPath(FText::FromString(TEXT("/Game")));
	
	FSlateApplication::Get().AddModalWindow(DlgPickPath, MainWindow);
	FString SavePath = DlgPickPath->GetPath().ToString();
	if (!SavePath.IsEmpty())
	{
		TextureSavePath = MoveTemp(SavePath);
	}
	return FReply::Handled();
}

void SOpenAITexGenSlateToolWindowWidget::Construct(const FArguments& InArgs)
{
	OnGenerateClickedDelegate = InArgs._OnGenerateClicked;
	MainWindow = InArgs._MainWindow;
		
	ChildSlot
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
					.HintText(LOCTEXT("TxtGenerationHint", "Realistic Green Grass"))
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
			.OnClicked_Lambda([this]()
			{
				OnGenerateClickedDelegate.ExecuteIfBound();
				return FReply::Handled();
			})
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
						.OnClicked(this, &SOpenAITexGenSlateToolWindowWidget::OnPathClicked)
						.Text_Lambda([this]()
						{
							return FText::FromString(TextureSavePath);
						})
					]
				]				
			]
		]
	
	];
}
#undef LOCTEXT_NAMESPACE