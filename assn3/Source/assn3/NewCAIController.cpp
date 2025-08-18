// Fill out your copyright notice in the Description page of Project Settings.

#include "NewCAIController.h"

// Constructor
ANewCAIController::ANewCAIController() {
	// Setting Default Radius for Collison Detection
	NeighborRadius = 100.0;
}

void ANewCAIController::BeginPlay() {
	// Call the base class  
	Super::BeginPlay();
}

void ANewCAIController::AttachToPawn(APawn* InPawn) {
	// To avoid UE crashing after stopping play
	if (InPawn != NULL) {
		// Setting up collison
		Super::AttachToPawn(InPawn);
		SphereComponent = NewObject<USphereComponent>(InPawn);
		SphereComponent->InitSphereRadius(NeighborRadius);
		SphereComponent->AttachToComponent(InPawn->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		SphereComponent->RegisterComponent();

		// For nieghbor tracking
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ANewCAIController::OnComponentBeginOverlap);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ANewCAIController::OnComponentEndOverlap);
	}
}

void ANewCAIController::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	
	// avoid adding self to list
	if (OtherActor != GetPawn()) {
		NeighborList.Add(OtherActor);
	}
}

void ANewCAIController::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	// avoid adding self to list
	if (OtherActor != GetPawn()) {
		NeighborList.Remove(OtherActor);
	}
}

void ANewCAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// MY AI BEHAVIOR: Boids - http://www.kfish.org/boids/pseudocode.html
	FVector v1, v2, v3;
	FVector FinalVelocity;
	FVector FinalMoveInput;
	int NumNeighbors = NeighborList.Num();

	// Loop through actors to drive Pawn direction
	for (auto itr : NeighborList) {
		v1 = BoidRule1(itr);
		v2 = BoidRule2(itr);
		v3 = BoidRule3(itr);

		FinalVelocity += v1 + v2 + v3;
		FinalMoveInput += FinalVelocity;
	}

	// use GetPawn()->AddMovementInput() to apply the forces
	// For the second (scale) argument to this function, use DeltaTime to scale the force with frame rate.
	GetPawn()->AddMovementInput(FinalMoveInput, DeltaTime);
}

FVector ANewCAIController::CalculateNeighborsCenterOfMass() {
	FVector CenterOfMass = {0, 0, 0};

	for (auto itr : NeighborList) {
		CenterOfMass += itr->GetActorLocation();
	}

	return CenterOfMass / (NeighborList.Num() - 1);
}

FVector ANewCAIController::CalculateNeighborsVelocity() {
	FVector Velocity = { 0, 0, 0 };

	for (auto itr : NeighborList) {
		Velocity += itr->GetVelocity();
	}

	return Velocity / (NeighborList.Num() - 1);
}

FVector ANewCAIController::BoidRule1(AActor* OtherPawn) {
	FVector CenterOfMass = CalculateNeighborsCenterOfMass();
	CenterOfMass = CenterOfMass + GetPawn()->GetActorLocation();
	CenterOfMass = CenterOfMass / (NeighborList.Num() - 1);
	return (CenterOfMass - OtherPawn->GetActorLocation()) / NeighborRadius;
}

FVector ANewCAIController::BoidRule2(AActor* OtherPawn) {
	FVector c = { 0, 0, 0 };
	c = c - (GetPawn()->GetActorLocation() - OtherPawn->GetActorLocation());
	return c;
}

FVector ANewCAIController::BoidRule3(AActor* OtherPawn) {
	FVector Velocity = CalculateNeighborsVelocity();
	Velocity = Velocity + GetPawn()->GetVelocity();
	Velocity = Velocity / (NeighborList.Num() - 1);
	return (Velocity - OtherPawn->GetVelocity()) / 8;
}