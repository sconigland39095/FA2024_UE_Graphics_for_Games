// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/InstancedStaticMeshComponent.h"

#include <iostream> 
#include <vector>
using namespace std;

#include "Maze.generated.h"

enum Rotation {
	HORIZONTAL = 1,
	VERTICAL = 2
};

enum PassageDirection {
	S = 1,
	E = 2
};

enum MoveDirection {
	NONE,
	RIGHT,
	LEFT,
	TOP, // couldn't use UP
	DOWN
};

UCLASS()
class ASSN2_API AMaze : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMaze();

	// virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	// Helper Functions - Build Maze Algorithm
	Rotation ChooseOrientation(int width, int height);
	void DivideField(vector<vector<int>> grid, int x, int y, int width, int height, Rotation orientation);
	void DisplayMaze(vector<vector<int>> grid);

	// Helper Functions - Wall Placement
	void BuildFixedMazeWithDirections();
	// rdir = rotation direction; mdir = movement direction
	void AddWall(Rotation rdir, TArray<MoveDirection> mdir);
	void AddWall(Rotation rdir, TArray<int> coord);

	// Called every frame
	// virtual void Tick(float DeltaTime) override;

protected:
	/*Once you get to the point where you're using an OnConstruction() function,
	you can add a dummy UPROPERTY(EditAnywhere) bool Update; to your class.
	That'll show as a checkbox, and clicking will force a re-run of OnConstruction().*/
	UPROPERTY(EditAnywhere)
	bool Update;

	// Maze Size
	UPROPERTY(EditAnywhere)
	FIntVector2 Size = {0, 0};

	UPROPERTY(EditAnywhere)
	int Seed;
	
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

private:
	UPROPERTY()
	class USceneComponent* SceneComponent;

	UPROPERTY()
	class UInstancedStaticMeshComponent* MeshComponent;
	//class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere)
	class UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* Material;

	UPROPERTY(EditAnywhere)
	class UTextureRenderTarget2D* RenderTarget;
};