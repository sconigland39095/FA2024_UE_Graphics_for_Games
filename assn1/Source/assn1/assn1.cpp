// Copyright Epic Games, Inc. All Rights Reserved.

#include "assn1.h"
//#include "Modules/ModuleManager.h"

void FAssn1Module::StartupModule() {
	FString ShaderDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Project"), ShaderDir);
}

IMPLEMENT_PRIMARY_GAME_MODULE(FAssn1Module, assn1, "assn1" );
