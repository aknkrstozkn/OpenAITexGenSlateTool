// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "TextureGeneratorStyle.h"

class FTextureGeneratorCommands : public TCommands<FTextureGeneratorCommands>
{
public:

	FTextureGeneratorCommands()
		: TCommands<FTextureGeneratorCommands>(TEXT("TextureGenerator"), NSLOCTEXT("Contexts", "TextureGenerator", "TextureGenerator Plugin"), NAME_None, FTextureGeneratorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};