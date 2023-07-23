// Copyright Epic Games, Inc. All Rights Reserved.

#include "TextureGeneratorCommands.h"

#define LOCTEXT_NAMESPACE "FTextureGeneratorModule"

void FTextureGeneratorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "TextureGenerator", "Bring up TextureGenerator window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
