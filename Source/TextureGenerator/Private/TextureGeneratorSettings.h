// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TextureGeneratorSettings.generated.h"

/**
 * 
 */
UCLASS(DefaultConfig, Config = TextureGenerator)
class TEXTUREGENERATOR_API UTextureGeneratorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = TextureGenerator)
	FString ApiKey;
	
};
