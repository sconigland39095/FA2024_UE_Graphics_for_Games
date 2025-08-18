// To be able to debug & break file
#ifdef __clang__
#pragma clang optimize off
#else
#pragma optimize("", off)
#endif

// Fill out your copyright notice in the Description page of Project Settings.
#include "Maze.h"
#include "Kismet/KismetRenderingLibrary.h"

// Sets default values; The Constructor
AMaze::AMaze()
{
	// STEP: Adding a SceneComponent & StaticMeshComponent
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"), true);
	check(SceneComponent != nullptr);
	this->RootComponent = SceneComponent;
	//SetRootComponent(Scene);
	
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (StaticMesh != NULL) {
		// Clearing StaticMesh
		StaticMesh = NULL;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	//UStaticMesh* StaticMesh = MeshRef.Object;
	StaticMesh = MeshRef.Object;
	check(StaticMesh != nullptr);
	
	/*
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyMesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetStaticMesh(StaticMesh);
	*/

	FRandomStream randomSeed;
	Seed = randomSeed.FRand();
}

void AMaze::OnConstruction(const FTransform& Transform)
{
	// Clear Instanced Static Mesh if Created Prior
	if (MeshComponent != NULL) {
		MeshComponent->ClearInstances();
	}

	// Adding Instanced Static Mesh - Referenced from https://suvam0451.github.io/tutorials/ue4/instanced-static-meshes/1
	MeshComponent = NewObject<UInstancedStaticMeshComponent>(this);
	MeshComponent->RegisterComponent();
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponent->SetStaticMesh(StaticMesh);

	if (Material) {
		MeshComponent->SetMaterial(0, Material);
	}

	//BuildFixedMazeWithDirections();
	// For Maze Generation Psuedo Code, Referneced: https://web.archive.org/web/20140413193031/https://weblog.jamisbuck.org/2011/1/12/maze-generation-recursive-division-algorithm
	int width = Size[0];
	int height = Size[1];
	// rows, columns
	vector<vector<int>> grid(height, vector<int>(width));
	DivideField(grid, 0, 0, width, height, ChooseOrientation(width, height));
	DisplayMaze(grid);
}

Rotation AMaze::ChooseOrientation(int width, int height) {
	if (width < height) {
		return HORIZONTAL;
	}
	else if (height < width) {
		return VERTICAL;
	}
	else {
		// rand(2) == rand() % 2
		return (rand() % 2) == 0 ? HORIZONTAL : VERTICAL;
	}
}

void AMaze::DivideField(vector<vector<int>> grid, int x, int y, int width, int height, Rotation orientation) {
	// has maze reached target resolution
	// changed from < to <= to avoid diving by 0 later
	if (width <= 2 || height <= 2) {
		return;
	}

	DisplayMaze(grid);
	// sleep 0.02
	
	// horizontal = orientation == HORIZONTAL
	Rotation horizontal;
	if (orientation == HORIZONTAL) {
		horizontal = HORIZONTAL;
	}
	else {
		horizontal = VERTICAL;
	}

	// where will wall be drawn from?
	int wx = x + (horizontal ? 0 : rand() % (width - 2));
	int wy = y + (horizontal ? rand() % (height - 2) : 0);
	
	// where will passange through wall exist?
	int px = wx + (horizontal ? rand() % width : 0);
	int py = wy + (horizontal ? 0 : rand() % height);

	// helper values
	// which direction will the wall be drawn?
	bool dx = horizontal ? 1 : 0;
	bool dy = horizontal ? 0 : 1;
	// how long will the wall be?
	int length = horizontal ? width : height;
	// what direction is perpendicular to the wall?
	int dir = horizontal ? S : E;

	// Draw wall
	for (int i = 0; i < length; i++) {
		if (wx != px || wy != py) {
			// grid[wy][wx] |= dir if wx != px || wy != py
			grid[wy][wx] |= dir;
		}
		wx += dx;
		wy += dy;
	}

	// Determine Subfield Bounds & Recurse
	int nx = x;
	int ny = y;
	//w, h = horizontal ? [width, wy - y + 1] : [wx - x + 1, height]
	int w;
	int h;
	if (horizontal) {
		w = width;
		h = wy - y + 1;
	}
	else {
		w = wx - x + 1;
		h = height;
	}
	DivideField(grid, nx, ny, w, h, ChooseOrientation(w, h));

	//nx, ny = horizontal ? [x, wy + 1] : [wx + 1, y]
	if (horizontal) {
		nx = x;
		ny = wy + 1;
	}
	else {
		nx = wx + 1;
		ny = y;
	}
	//w, h = horizontal ? [width, y + height - wy - 1] : [x + width - wx - 1, height]
	if (horizontal) {
		w = width;
		h = y + height - wy - 1;
	}
	else {
		w = x + width - wx - 1;
		h = height;
	}
	DivideField(grid, nx, ny, w, h, ChooseOrientation(w, h));
}

void AMaze::DisplayMaze(vector<vector<int>> grid) {
	int width = Size[0];
	int height = Size[1];
	
	// make top wall
	TArray<MoveDirection> wallPlacementBorder = { NONE };
	for (int i = 0; i < Size[1]; i++) {
		if (i == 1) {
			wallPlacementBorder[0] = RIGHT;
		}
		else if (i > 1) {
			wallPlacementBorder.Add(RIGHT);
		}

		AddWall(HORIZONTAL, wallPlacementBorder);
	}

	wallPlacementBorder = { LEFT, DOWN };
	// loop through rows
	for (int y = 1; y < height; y++) {
		// each row begins w/ a vertical wall placement
		AddWall(VERTICAL, wallPlacementBorder);
		wallPlacementBorder.Add(DOWN);

		// clear array and reset with how many rows down we are in the maze
		TArray<MoveDirection> wallPlacementRow = {};
		for (int i = 0; i < y; i++) {
			wallPlacementRow.Add(DOWN);
		}

		// loop through columns
		for (int x = 0; x < width; x++) {
			int bottom = y + 1 >= grid.size();
			int south = ((grid[y][x] & S) != 0) || bottom;
			int south2 = ((x + 1 < grid[y].size() && grid[y][x + 1]) & S) != 0 || bottom;
			int east = ((grid[y][x] & E) != 0) || (x + 1 >= grid[y].size());

			// each time a wall is or is not added, move to the right to be ready for the next wall
			if (south) {
				AddWall(HORIZONTAL, wallPlacementRow);
				wallPlacementRow.Add(RIGHT);
			}
			// else no wall added
			else {
				wallPlacementRow.Add(RIGHT);
			}
			
			if (east) {
				AddWall(VERTICAL, wallPlacementRow);
				wallPlacementRow.Add(RIGHT);
			}
			else {
				if (south && south2) {
					AddWall(HORIZONTAL, wallPlacementRow);
					wallPlacementRow.Add(RIGHT);
				}
				// else no wall added
				else {
					wallPlacementRow.Add(RIGHT);
				}
			}
		}
	}
}

void AMaze::BuildFixedMazeWithDirections() {
	// Top Wall
	TArray<MoveDirection> wallPlacement = { NONE };
	AddWall(HORIZONTAL, wallPlacement);
	wallPlacement = { RIGHT };
	AddWall(HORIZONTAL, wallPlacement);
	wallPlacement = { LEFT };
	AddWall(HORIZONTAL, wallPlacement);
	
	// Middle Wall - Top
	wallPlacement = { NONE, DOWN };
	AddWall(HORIZONTAL, wallPlacement);
	wallPlacement = { RIGHT, DOWN };
	AddWall(HORIZONTAL, wallPlacement);
	wallPlacement = { LEFT, DOWN };
	AddWall(HORIZONTAL, wallPlacement);
	
	// Middle Wall - Bottom
	wallPlacement = { NONE, DOWN, DOWN };
	AddWall(HORIZONTAL, wallPlacement);
	wallPlacement = { RIGHT, DOWN, DOWN };
	AddWall(HORIZONTAL, wallPlacement);
	wallPlacement = { LEFT, DOWN, DOWN };
	AddWall(HORIZONTAL, wallPlacement);
	
	// Bottom Wall
	wallPlacement = { NONE, DOWN, DOWN, DOWN };
	AddWall(HORIZONTAL, wallPlacement);
	wallPlacement = { RIGHT, DOWN, DOWN, DOWN };
	AddWall(HORIZONTAL, wallPlacement);
	wallPlacement = { LEFT, DOWN, DOWN, DOWN };
	AddWall(HORIZONTAL, wallPlacement);

	// Left Middle Wall
	wallPlacement = { LEFT, DOWN };
	AddWall(VERTICAL, wallPlacement);
	wallPlacement = { LEFT, DOWN, DOWN };
	AddWall(VERTICAL, wallPlacement);
	wallPlacement = { LEFT, DOWN, DOWN, DOWN };
	AddWall(VERTICAL, wallPlacement);
	
	// Left Wall
	wallPlacement = { LEFT, LEFT, LEFT, DOWN };
	AddWall(VERTICAL, wallPlacement);
	wallPlacement = { LEFT, LEFT, LEFT, DOWN, DOWN };
	AddWall(VERTICAL, wallPlacement);
	wallPlacement = { LEFT, LEFT, LEFT, DOWN, DOWN, DOWN };
	AddWall(VERTICAL, wallPlacement);

	// Right Middle Wall
	wallPlacement = { RIGHT, DOWN };
	AddWall(VERTICAL, wallPlacement);
	wallPlacement = { RIGHT, DOWN, DOWN };
	AddWall(VERTICAL, wallPlacement);
	wallPlacement = { RIGHT, DOWN, DOWN, DOWN };
	AddWall(VERTICAL, wallPlacement);
	
	// Right Wall
	wallPlacement = { RIGHT, RIGHT, RIGHT, DOWN };
	AddWall(VERTICAL, wallPlacement);
	wallPlacement = { RIGHT, RIGHT, RIGHT, DOWN, DOWN };
	AddWall(VERTICAL, wallPlacement);
	wallPlacement = { RIGHT, RIGHT, RIGHT, DOWN, DOWN, DOWN };
	AddWall(VERTICAL, wallPlacement);
}

void AMaze::AddWall(Rotation rdir, TArray<MoveDirection> mdir) {
	/* 
	FTransform() takes in the following values in this order...
		// Rotation = {0,0,0,1)
		Rotation = GlobalVectorConstants::Float0001;
		// Translation = {0,0,0,0)
		Translation = GlobalVectorConstants::FloatZero;
		// Scale3D = {1,1,1,0);
		Scale3D = GlobalVectorConstants::Float1110;
	The Rotation parts takes FRotator(y, x, z);
	*/
	// movement in the X, Y, Z direction, respectively
	int mdirX = 0;
	int mdirY = 0;
	int mdirZ = 0;

	if (rdir == HORIZONTAL) {
		for (int i = 0; i < mdir.Num(); i++) {
			if (mdir[i] == RIGHT) {
				mdirY += 100;
			}
			else if (mdir[i] == LEFT) {
				mdirY -= 100;
			}
			else if (mdir[i] == TOP) {
				mdirX += 100;
			}
			else if (mdir[i] == DOWN) {
				mdirX -= 100;
			}
		}
		// puuting X & Y in the wrong order worked out so wasn't changed
		MeshComponent->AddInstance(FTransform(FRotator(90, 0, 0), FVector(mdirX, mdirY, mdirZ)));
	}
	else if (rdir == VERTICAL) {
		/* When placing a vertical wall, in correlation to the top, starting wall, the wall must move down 50.
			However, every subsequent walls move down 100. Thus, it must be checked if we have done thw first down.
		*/
		bool passedFirstDown = false;

		for (int i = 0; i < mdir.Num(); i++) {
			if (mdir[i] == RIGHT) {
				mdirY += 50;
			}
			else if (mdir[i] == LEFT) {
				mdirY -= 50;
			}
			else if (mdir[i] == TOP) {
				if (passedFirstDown) {
					mdirX += 100;
				}
				else {
					mdirX += 50;
					passedFirstDown = true;
				}
			}
			else if (mdir[i] == DOWN) {
				if (passedFirstDown) {
					mdirX -= 100;
				}
				else {
					mdirX -= 50;
					passedFirstDown = true;
				}
			}
		}

		MeshComponent->AddInstance(FTransform(FRotator(90, 0, 90), FVector(mdirX, mdirY, mdirZ)));
	}
}

void AMaze::AddWall(Rotation rdir, TArray<int> coord) {
	// if Z is not given, make it 0
	if (coord.Num() < 3) {
		coord.Add(0);
	}

	if (rdir == HORIZONTAL) {
		// y, x, z
		MeshComponent->AddInstance(FTransform(FRotator(90, 0, 0), FVector(coord[1], coord[0], coord[2])));
	}
	else if (rdir == VERTICAL) {
		MeshComponent->AddInstance(FTransform(FRotator(90, 0, 90), FVector(coord[1], coord[0], coord[2])));
	}
}

/*
// Called when the game starts or when spawned
void AMaze::BeginPlay()
{
	Super::BeginPlay();
	// DrawMaterialToRenderTarget(UObject* WorldContextObject, UTextureRenderTarget2D* TextureRenderTarget, UMaterialInterface* Material);
	//class UMaterialInterface* Material;
	//class UTextureRenderTarget2D* RenderTarget;
	if (Material && RenderTarget) {
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), RenderTarget, Material);
	}
}
*/
/*
// Called every frame
void AMaze::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
*/

// Don't want this to be permanently unoptimized
#ifdef __clang__
#pragma clang optimize on
#else
#pragma optimize("", on)
#endif