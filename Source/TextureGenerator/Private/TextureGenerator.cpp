// Copyright Epic Games, Inc. All Rights Reserved.

#include "TextureGenerator.h"
#include "TextureGeneratorStyle.h"
#include "TextureGeneratorCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "WorkspaceMenuStructureModule.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

static const FName TextureGeneratorTabName("TextureGenerator");

#define LOCTEXT_NAMESPACE "FTextureGeneratorModule"

void FTextureGeneratorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FTextureGeneratorStyle::Initialize();
	FTextureGeneratorStyle::ReloadTextures();

	FTextureGeneratorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTextureGeneratorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FTextureGeneratorModule::PluginButtonClicked),
		FCanExecuteAction());

	//UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FTextureGeneratorModule::RegisterMenus));
	
	/*FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TextureGeneratorTabName, FOnSpawnTab::CreateRaw(this, &FTextureGeneratorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FTextureGeneratorTabTitle", "TextureGenerator"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);*/

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TextureGeneratorTabName, FOnSpawnTab::CreateRaw(this, &FTextureGeneratorModule::OnSpawnPluginTab))
			.SetDisplayName(LOCTEXT("FTextureGeneratorTabTitle", "TextureGenerator"))
			.SetTooltipText(LOCTEXT("TooltipText", "Generate AI based Textures"))
			.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());
}

void FTextureGeneratorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//UToolMenus::UnRegisterStartupCallback(this);

	//UToolMenus::UnregisterOwner(this);

	FTextureGeneratorStyle::Shutdown();

	FTextureGeneratorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TextureGeneratorTabName);
}

TSharedRef<SDockTab> FTextureGeneratorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	/*FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Texture Definition"),
		FText::FromString(TEXT("FTextureGeneratorModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("TextureGenerator.cpp"))
		);*/

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SVerticalBox)
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
					SNew(SMultiLineEditableTextBox)
					.AllowMultiLine(true)
					.Mult(0.8f)
					.HintText(LOCTEXT("TxtGenerationHint", "Relistic Green Grass"))
				]
			]

			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(16.f, 16.f)
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Top)
			[
				SNew(SButton)
				.Text(LOCTEXT("TxtGeneratorButton", "Generate"))
			]			
			
			+SVerticalBox::Slot()
			.FillHeight(1.0f)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("TxtGenerationSettings", "Settings"))
			]			
		];
}

void FTextureGeneratorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(TextureGeneratorTabName);
}

void FTextureGeneratorModule::RegisterMenus()
{

	/*FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TextureGeneratorTabName, FOnSpawnTab::CreateRaw(this, &FTextureGeneratorModule::OnSpawnPluginTab))
			.SetDisplayName(LOCTEXT("FTextureGeneratorTabTitle", "TextureGenerator"))
			.SetTooltipText(LOCTEXT("TooltipText", "Generate AI based Textures"))
			.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());*/
	
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	/*FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu();
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("TOOLS");
			Section.AddMenuEntryWithCommandList(FTextureGeneratorCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FTextureGeneratorCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}*/
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTextureGeneratorModule, TextureGenerator)