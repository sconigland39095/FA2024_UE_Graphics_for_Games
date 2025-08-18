// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Engine/VolumeTexture.h"
#include "assn5ImporterLibrary.generated.h"


UCLASS()
class ASSN5IMPORTER_API Uassn5ImporterLibrary: public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	//Uassn5ImporterLibrary();
	//Uassn5ImporterLibrary(class FObjectInitializer const&);
	~Uassn5ImporterLibrary();

	// asset loader
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

	// LoadFileToArray but for uint16
	bool LoadFileToArray16(TArray<uint16>& Result, const TCHAR* Filename, uint32 Flags = 0);
};
