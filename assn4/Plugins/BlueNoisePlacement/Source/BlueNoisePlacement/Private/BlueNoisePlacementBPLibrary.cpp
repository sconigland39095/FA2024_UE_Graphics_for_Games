// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueNoisePlacementBPLibrary.h"
#include "BlueNoisePlacement.h"
#include <random>
using namespace std;

UBlueNoisePlacementBPLibrary::UBlueNoisePlacementBPLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

float UBlueNoisePlacementBPLibrary::BlueNoisePlacementSampleFunction(float Param)
{
	return -1;
}

void UBlueNoisePlacementBPLibrary::MakeBlueNoise(FBlueNoise &BlueNoise, int Seed, FVector2D MinSize, FVector2D MaxSize) {
	FRandomStream RandomSeed(Seed);
	FBlueNoise NewBlueNoise(RandomSeed, MinSize, MaxSize);

	BlueNoise = NewBlueNoise;
	//return NewBlueNoise;
}


FVector UBlueNoisePlacementBPLibrary::GetBlueNoisePoint(UPARAM(ref) FBlueNoise& Stream) {
	/*
	The algorithm is as follows:
		1. Place a random sample point as the first sample point.
		2. Generate some number of random dots as candidates to be the next sample point.
		3. Whichever of these dots is farthest away from the closest existing sample point is the winner. Place that dot as the new sample point.
		4. GOTO 2 and Repeat until you have as many sample points as you want.

	The algorithm is pretty simple, but there are two other important details that are needed to give you good results:
		~ When calculating distance between dots, you need to consider wrap around. More info on how to do that here:
		Calculating the Distance Between Points in “Wrap Around” (Toroidal) Space.
		~ The number of candidates you generate should scale up with the number of existing sample points.
		As the original paper describing this technique says, doing that helps ensure that the statistics of your sample points stay constant as the number of sample
		points changes.
	*/


	// Implement Mitchell's best-candidate algorithm in your Blueprint node.
	// If N points have been generated so far, your node should try at least 5N candidate points, and return the one farthest from any already placed point.

	FVector result;
	// 1. Place a random sample point as the first sample point.
	if (Stream.CurCoordinates.Num() == 0) {
		result = GenerateRandomPoint(Stream.MinSize, Stream.MaxSize);
	}
	else {
		// 2. Generate some number of random dots as candidates to be the next sample point.
		int GenerateCounter = Stream.CurCoordinates.Num() * 5;
		// List of possible coordinates
		TSet<FVector> PosCoordinates;
		for (int i = 0; i < GenerateCounter; i++) {
			PosCoordinates.Add(GenerateRandomPoint(Stream.MinSize, Stream.MaxSize));
		}
		// 3. Whichever of these dots is farthest away from the closest existing sample point is the winner.Place that dot as the new sample point.
		int counter = 0;
		FVector FarPoint;
		double CurDistance = MeasureDistance(Stream.LastSamplePoint, FarPoint);

		for (auto itr : PosCoordinates) {
			if (counter == 0) {
				FarPoint = itr;
			}
			else {
				double PosDistance = MeasureDistance(Stream.LastSamplePoint, itr);
				if (PosDistance > CurDistance) {
					FarPoint = itr;
					CurDistance = PosDistance;
				}
			}
		}

		result = FarPoint;
	}

	// 4. GOTO 2 and Repeat until you have as many sample points as you want.
	Stream.CurCoordinates.Add(result);
	Stream.LastSamplePoint = result;
	return result;
}

FVector UBlueNoisePlacementBPLibrary::GenerateRandomPoint(FVector2D MinSize, FVector2D MaxSize) {
	FVector result;
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> distributionX(MinSize[0], MaxSize[0]);
	uniform_real_distribution<> distributionY(MinSize[1], MaxSize[1]);

	double randomX = distributionX(gen);
	double randomY = distributionY(gen);
	result[0] = randomX;
	result[1] = randomY;
	return result;
}

double UBlueNoisePlacementBPLibrary::MeasureDistance(FVector point1, FVector point2) {
	return FVector::Distance(point1, point2);
}