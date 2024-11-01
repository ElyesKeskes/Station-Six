// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActorComponent.h"
#include "BlindPursuitProject/Public/LevelGenerationFLibrary.h"


// Sets default values for this component's properties
UGridActorComponent::UGridActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//Initialize the grid with default values
	//InitializeGrid(40, 40, 2, 500, FVector(0.0f, 0.0f, 0.0f));
	// ...
}


// Called when the game starts
void UGridActorComponent::BeginPlay()
{
	Super::BeginPlay();
	//Set tick rate to 0.5 seconds
	PrimaryComponentTick.TickInterval = 0.5f;
}


// Called every frame
void UGridActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(DrawDebug)
	{
		//DrawDebugGrid();
	}
}

//Initialize the grid with the specified dimensions
void UGridActorComponent::InitializeGrid(int32 XSize, int32 YSize, int32 ZSize, int32 CellSize,FVector AnchorWorldPosition)
{
	// Resize the grid to the specified dimensions
	Grid.SetNum(XSize);

	for (int32 X = 0; X < XSize; X++)
	{
		Grid[X].SetNum(YSize);

		for (int32 Y = 0; Y < YSize; Y++)
		{
			Grid[X][Y].SetNum(ZSize);
		}
	}
	// Initialize all grid cells to false
	for (int32 X = 0; X < XSize; X++)
	{
		for (int32 Y = 0; Y < YSize; Y++)
		{
			for (int32 Z = 0; Z < ZSize; Z++)
			{
				Grid[X][Y][Z] = false;
			}
		}
	}
	GridXSize = XSize;
	GridYSize = YSize;
	GridZSize = ZSize;
	GridCellSize = CellSize;
	GridAnchorWorldPosition = AnchorWorldPosition;
}
void UGridActorComponent::MarkSpaceOccupied(FVector SpaceAnchorCoordinates, FVector SpaceSize)
{
	// Loop through the space's grid and mark cells as occupied
	for (int32 X = SpaceAnchorCoordinates.X; X < SpaceAnchorCoordinates.X + SpaceSize.X; X++)
	{
		if (X >= 0 && X < GridXSize) // Check X bounds
		{
			for (int32 Y = SpaceAnchorCoordinates.Y; Y < SpaceAnchorCoordinates.Y + SpaceSize.Y; Y++)
			{
				if (Y >= 0 && Y < GridYSize) // Check Y bounds
				{
					for (int32 Z = SpaceAnchorCoordinates.Z; Z < SpaceAnchorCoordinates.Z + SpaceSize.Z; Z++)
					{
						if (Z >= 0 && Z < GridZSize) // Check Z bounds
						{
							Grid[X][Y][Z] = true; // Mark the cell as occupied
						}
						else
						{

							UE_LOG(LogTemp, Warning, TEXT("Space out of bounds: SpaceSizeX: %f, SpaceSizeY: %f, SpaceSizeZ: %f"), SpaceSize.X, SpaceSize.Y, SpaceSize.Z);
							UE_LOG(LogTemp, Warning, TEXT("SpaceAnchorCoordinatesX: %f, SpaceAnchorCoordinatesY: %f, SpaceAnchorCoordinatesZ: %f"), SpaceAnchorCoordinates.X, SpaceAnchorCoordinates.Y, SpaceAnchorCoordinates.Z);
							UE_LOG(LogTemp, Warning, TEXT("GridXSize: %d, GridYSize: %d, GridZSize: %d, GridXLength: %d, GridYLength: %d, GridZLength: %d"), GridXSize, GridYSize, GridZSize, Grid.Num(), Grid[0].Num(), Grid[0][0].Num());
						}
					}
				}
				else
				{

					UE_LOG(LogTemp, Warning, TEXT("Space out of bounds"));
				}
			}
		}
		else
		{

			UE_LOG(LogTemp, Warning, TEXT("Space out of bounds"));
		}
	}
}


void UGridActorComponent::DrawDebugCell(FVector CellCoordinates, FColor CellColor, float Duration, float Thickness)
{
	//Take in consideration FVector GridAnchorWorldPosition which is  the cell at the top-left corner of the grid, the the world location of the center of the that cell
	FVector CellCenterWorldLocation = GridAnchorWorldPosition + FVector(CellCoordinates.X * GridCellSize + GridCellSize / 2.0f, CellCoordinates.Y * GridCellSize + GridCellSize / 2.0f, CellCoordinates.Z * GridCellSize + GridCellSize / 2.0f);
	DrawDebugBox(GetWorld(), CellCenterWorldLocation, FVector(GridCellSize / 2.0f), CellColor, false, Duration, 0, Thickness);
	DrawDebugPoint(GetWorld(), CellCenterWorldLocation, 10.0f, CellColor, false, Duration);
}

void UGridActorComponent::DrawDebugGrid()
{
	for (int32 Z = 0; Z < GridZSize; Z++)
	{
		for (int32 Y = 0; Y < GridYSize; Y++)
		{
			for (int32 X = 0; X < GridXSize; X++)
			{
				if (Grid[X][Y][Z])
				{
					DrawDebugCell(FVector(X, Y, Z), FColor::Yellow, 0.5f, 3.0f);
				}
				else
				{
					DrawDebugCell(FVector(X, Y, Z), FColor::White, 0.5f, 2.0f);
				}
			}
		}
	}
}

void UGridActorComponent::DrawDebugSpace(FVector SpaceAnchorCoordinates, FVector SpaceSize, FColor SpaceColor, float Duration, float Thickness)
{
	for(int32 X = SpaceAnchorCoordinates.X; X < SpaceAnchorCoordinates.X + SpaceSize.X; X++)
	{
		if(X >= 0 && X < GridXSize) // Check X bounds
		{
			for(int32 Y = SpaceAnchorCoordinates.Y; Y < SpaceAnchorCoordinates.Y + SpaceSize.Y; Y++)
			{
				if(Y >= 0 && Y < GridYSize) // Check Y bounds
				{
					for(int32 Z = SpaceAnchorCoordinates.Z; Z < SpaceAnchorCoordinates.Z + SpaceSize.Z; Z++)
					{
						if(Z >= 0 && Z < GridZSize) // Check Z bounds
						{
							DrawDebugCell(FVector(X, Y, Z), SpaceColor, Duration, Thickness);
						}
					}
				}
			}
		}
	}
}

FVector UGridActorComponent::ConvertFromGridCoordinatesToWorldLocation(FVector GridCoordinates)
{
	FVector value = GridAnchorWorldPosition + FVector(GridCoordinates.X * GridCellSize + GridCellSize / 2.0f, GridCoordinates.Y * GridCellSize + GridCellSize / 2.0f, GridCoordinates.Z * GridCellSize + GridCellSize / 2.0f);
	return value + FVector(-GridCellSize/2, -GridCellSize/2, -GridCellSize/2);
}

FVector UGridActorComponent::ConvertFromWorldLocationToGridCoordinates(FVector WorldLocation)
{
	FVector RelativeLocation = WorldLocation - GridAnchorWorldPosition;
	int32 X = FMath::FloorToInt(RelativeLocation.X / GridCellSize);
	int32 Y = FMath::FloorToInt(RelativeLocation.Y / GridCellSize);
	int32 Z = FMath::FloorToInt(RelativeLocation.Z / GridCellSize);
	return FVector(X, Y, Z);
}

bool UGridActorComponent::CheckSpaceOccupied(FVector SpaceAnchorCoordinates, FVector SpaceSize)
{	
	//Handle out of grid bounds cases
	if(SpaceAnchorCoordinates.X < 0 || SpaceAnchorCoordinates.Y < 0 || SpaceAnchorCoordinates.Z < 0 || SpaceAnchorCoordinates.X + SpaceSize.X > GridXSize || SpaceAnchorCoordinates.Y + SpaceSize.Y >=GridYSize || SpaceAnchorCoordinates.Z + SpaceSize.Z > GridZSize)
	{
		UE_LOG(LogTemp, Error, TEXT("Space out of bounds: SpaceAnchorCoordinate X: %f, Y: %f, Z: %f, SpaceLimitCoordinates X: %f, Y: %f, Z: %f"),SpaceAnchorCoordinates.X, SpaceAnchorCoordinates.Y, SpaceAnchorCoordinates.Z, SpaceAnchorCoordinates.X + SpaceSize.X, SpaceAnchorCoordinates.Y + SpaceSize.Y, SpaceAnchorCoordinates.Z + SpaceSize.Z);
		return false;
	}
	for(int32 X = SpaceAnchorCoordinates.X; X < SpaceAnchorCoordinates.X + SpaceSize.X; X++)
	{
		for(int32 Y = SpaceAnchorCoordinates.Y; Y < SpaceAnchorCoordinates.Y + SpaceSize.Y; Y++)
		{
			for(int32 Z = SpaceAnchorCoordinates.Z; Z < SpaceAnchorCoordinates.Z + SpaceSize.Z; Z++)
			{
				if(Grid[X][Y][Z])
				{
					return false;
				}
			}
		}
	}
	return true;
}

FVector UGridActorComponent::GetSocketCoordinatesInRoom(FVector SocketRelativeLocationInRoom, FVector RoomSize) //RoomSize is the room grid space size
{
	// Ensure that the relative location is properly scaled based on the grid cell size

	float scaledLocationX = SocketRelativeLocationInRoom.X / GridCellSize;
	float scaledLocationY = SocketRelativeLocationInRoom.Y / GridCellSize;
	float scaledLocationZ = SocketRelativeLocationInRoom.Z / GridCellSize;

	// Adjust the room's footprint bounds to center the calculations
	float HalfRoomSizeX = RoomSize.X / 2.0f;
	float HalfRoomSizeY = RoomSize.Y / 2.0f;
	float HalfRoomSizeZ = RoomSize.Z / 2.0f;

	FVector CoordinatesInRoom;

	CoordinatesInRoom.X = std::ceil(scaledLocationX + HalfRoomSizeX);
	CoordinatesInRoom.Y = std::ceil(scaledLocationY + HalfRoomSizeY);
	CoordinatesInRoom.Z = std::ceil(scaledLocationZ + HalfRoomSizeZ);

	if(CoordinatesInRoom.X == 0)
	{
		CoordinatesInRoom.X = 1;
	}
	if(CoordinatesInRoom.Y == 0)
	{
		CoordinatesInRoom.Y = 1;
	}
	
	CoordinatesInRoom.Z = 1;

	return CoordinatesInRoom;
	
}
inline bool IsEven(int32 Number)
{
	return Number % 2 == 0;
}
FVector UGridActorComponent::GetSpaceCenterWorldLocation(FVector SpaceAnchorCoordinates, FVector SpaceSize)
{
	// Calculate grid coordinates for the center of the space/room
	FVector SpaceCenterGridCoordinates = SpaceAnchorCoordinates + (SpaceSize / 2.0f);

	// Correct for even sizes by subtracting half a cell size if the size is even
	if (!IsEven((int32)SpaceSize.X))
	{
		//SpaceCenterGridCoordinates.X -= 0.5f; // Align for even X size
	}
	if (!IsEven((int32)SpaceSize.Y))
	{
		//SpaceCenterGridCoordinates.Y -= 0.5f; // Align for even Y size
	}
	if (!IsEven((int32)SpaceSize.Z))
	{
		//SpaceCenterGridCoordinates.Z -= 0.5f; // Align for even Z size
	}

	// Convert grid space center to world location based on GridAnchorWorldPosition
	FVector SpaceCenterWorldLocation = GridAnchorWorldPosition + (SpaceCenterGridCoordinates * GridCellSize) ;

	return SpaceCenterWorldLocation;
}

FVector UGridActorComponent::GetSpaceToConnectAnchorCoordinates(FVector ExistingSpaceAnchorCoordinates, FRoomSocketData SpaceToConnectSocketData, FRoomSocketData ExistingSpaceSocketData)
{
	FVector SpaceToConnectAnchorCoordinates = FVector(0,0,0);
	FVector ExistingSpaceSocketCoordinates = ExistingSpaceAnchorCoordinates + ExistingSpaceSocketData.CoordinatesInRoom;
	switch(SpaceToConnectSocketData.Orientation)
	{
	case ESocketOrientation::North:
		 SpaceToConnectAnchorCoordinates = ExistingSpaceSocketCoordinates + FVector(0, 1, 0);
		break;
	case ESocketOrientation::East:
		 SpaceToConnectAnchorCoordinates = ExistingSpaceSocketCoordinates + FVector(-1, 0, 0);
		break;
	case ESocketOrientation::South:
		 SpaceToConnectAnchorCoordinates = ExistingSpaceSocketCoordinates + FVector(0, -1, 0);
		break;
	case ESocketOrientation::West:
		 SpaceToConnectAnchorCoordinates = ExistingSpaceSocketCoordinates + FVector(1, 0, 0);
	}

	return SpaceToConnectAnchorCoordinates= SpaceToConnectAnchorCoordinates - (SpaceToConnectSocketData.CoordinatesInRoom);
}


	

