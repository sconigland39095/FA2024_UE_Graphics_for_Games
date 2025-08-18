// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueNoisePlacementBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
//D:\g4g2024\assn4\Plugins\BlueNoisePlacement\Source\BlueNoisePlacement\Public\BlueNoisePlacementBPLibrary.h
// USTRUCT(BlueprintType, meta = (HasNativeMake = "BlueprintJson.BlueprintJsonLibrary.JsonMakeString"))
USTRUCT(BlueprintType, meta = (HasNativeMake = "BlueNoisePlacement.BlueNoisePlacementBPLibrary.MakeBlueNoise"))
struct FBlueNoise {
	GENERATED_USTRUCT_BODY()

	FBlueNoise() {}
	FBlueNoise(FRandomStream GivenSeed, FVector2D GivenMinSize, FVector2D GivenMaxSize) {
		Seed = GivenSeed;
		MinSize = GivenMinSize;
		MaxSize = GivenMaxSize;
	}

	// random stream object
	UPROPERTY()
	FRandomStream Seed;

	// the range of/for generated points
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D MinSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D MaxSize;

	// the set of genertaed 2D point locations
	//UPROPERTY()
	TSet<FVector> CurCoordinates;

	// Helper Variable
	FVector LastSamplePoint;
};

UCLASS()
class UBlueNoisePlacementBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Sample function", Keywords = "BlueNoisePlacement sample test testing"), Category = "BlueNoisePlacementTesting")
	static float BlueNoisePlacementSampleFunction(float Param);

	UFUNCTION(BlueprintCallable, meta = (NativeMakeFunc))
	static void MakeBlueNoise(FBlueNoise &BlueNoise, int Seed, FVector2D MinSize, FVector2D MaxSize);

	UFUNCTION(BlueprintCallable)
	static FVector GetBlueNoisePoint(UPARAM(ref) FBlueNoise &Stream);

	// Helper Functions
	static FVector GenerateRandomPoint(FVector2D MinSize, FVector2D MaxSize);
	static double MeasureDistance(FVector point1, FVector point2);
};