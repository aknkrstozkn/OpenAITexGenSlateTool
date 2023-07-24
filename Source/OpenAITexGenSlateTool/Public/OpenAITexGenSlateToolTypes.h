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
