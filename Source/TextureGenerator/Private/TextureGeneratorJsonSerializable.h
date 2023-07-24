// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Serialization/JsonSerializerMacros.h"

struct FDallEPrompt final : FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
		JSON_SERIALIZE("prompt", Prompt);
		JSON_SERIALIZE("n", ImageCount);
		JSON_SERIALIZE("size", ImageSize);
	END_JSON_SERIALIZER

	FString Prompt;
	int32 ImageCount = 1;
	FString ImageSize = "1024x1024";
};

struct FURLData final : FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
		JSON_SERIALIZE("url", Url);
	END_JSON_SERIALIZER

	FString Url;
};

struct FDallEResponse final : FJsonSerializable
{
	// Example Response Format
	// {"created": 1690130733, "data": [{"url": "..."}]}
	
	BEGIN_JSON_SERIALIZER
		JSON_SERIALIZE_ARRAY_SERIALIZABLE("data", UrlArray, FURLData);
	END_JSON_SERIALIZER

	TArray<FURLData> UrlArray;
};
