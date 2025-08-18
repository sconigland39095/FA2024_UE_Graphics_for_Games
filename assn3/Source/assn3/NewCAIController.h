// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/SphereComponent.h"

#include "NewCAIController.generated.h"

// Ball AI Controller
UCLASS()
class ASSN3_API ANewCAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Constructor
	ANewCAIController();

	virtual void BeginPlay();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// tracks neighbors within this radius
	UPROPERTY(EditAnywhere)
	float NeighborRadius;
	
	// Neighbor set
	UPROPERTY(VisibleAnywhere, Transient)
	TSet<AActor*> NeighborList;

	virtual void AttachToPawn(APawn* InPawn) override;

	// Tutorial Referenced: https://www.unrealexamples.com/docs/cpp/on-component-begin-overlap
	// Add nearby objects that come in range
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// Remove nearby objects that leave range
	// For some reason, had to remove the last two paramters to remove error in AttachToPawn
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	// This is the collision check radius
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComponent;

	// Calculates the collective Center of Mass for all of the items in NeighborList
	FVector CalculateNeighborsCenterOfMass();
	// Calculates the collective Velocity for all of the items in neighborList
	FVector CalculateNeighborsVelocity();
	// Rule 1: Boids try to fly towards the centre of mass of neighbouring boids
	FVector BoidRule1(AActor* OtherPawn);
	// Rule 2: Boids try to keep a small distance away from other objects (including other boids).
	FVector BoidRule2(AActor* OtherPawn);
	// Rule 3: Boids try to match velocity with near boids.
	FVector BoidRule3(AActor* OtherPawn);
};
