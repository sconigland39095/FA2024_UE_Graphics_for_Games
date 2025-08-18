// Copyright Epic Games, Inc. All Rights Reserved.

#include "assn5.h"

void FAssn5Module::StartupModule()
{
	FString ShaderDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Project"), ShaderDir);
}

IMPLEMENT_PRIMARY_GAME_MODULE( FAssn5Module, assn5, "assn5" );
