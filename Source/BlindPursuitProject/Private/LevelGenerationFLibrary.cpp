#include "BlindPursuitProject/Public/LevelGenerationFLibrary.h"
#include "DrawDebugHelpers.h" // Include this to use the debug draw functions
//Declare constant CELL_LENGTH
#define CELL_LENGTH 500.0f

void ULevelGenerationFLibrary::DrawDebugCell(UObject* WorldContextObject, FVector CellCenter, FColor CellColor, float Duration, float Thickness)
{
	DrawDebugBox(WorldContextObject->GetWorld(), CellCenter, FVector(CELL_LENGTH / 2.0f), CellColor, false, Duration, 0, Thickness);
	DrawDebugPoint(WorldContextObject->GetWorld(), CellCenter, 10.0f, CellColor, false, Duration);
}

void ULevelGenerationFLibrary::DrawDebugGrid(UObject* WorldContextObject, TArray<bool> Grid, int32 XSize, int32 YSize, int32 ZSize, float CellLength, FVector GridTopLeftCellCenterLocation)
{
	for (int32 Z = 0; Z < ZSize; Z++) // Adjusted the loop to start from 0 for proper indexing
	{
		for (int32 Y = 0; Y < YSize; Y++) // Adjusted the loop to start from 0 for proper indexing
		{
			for (int32 X = 0; X < XSize; X++) // Adjusted the loop to start from 0 for proper indexing
			{
				int32 Index = X + (Y * XSize) + (Z * XSize * YSize);

				if (Grid.IsValidIndex(Index)) // Only draw occupied cells
				{
					// Calculate the center position of the current cell using GridTopLeftCellCenterLocation
					FVector CellCenter = GridTopLeftCellCenterLocation + FVector(X * CellLength + CellLength / 2.0f, Y * CellLength + CellLength / 2.0f, Z * CellLength + CellLength / 2.0f);
                    
					// Draw a box for the occupied cell
					if (!Grid[Index])
					{
						DrawDebugCell(WorldContextObject, CellCenter, FColor::White, -1.0f, 2.0f);
					}
					else
					{
						DrawDebugCell(WorldContextObject, CellCenter, FColor::Yellow, -1.0f, 3.0f);
					}
				}
			}
		}
	}
}

void ULevelGenerationFLibrary::DrawDebugGridCustom(UObject* WorldContextObject, TArray<bool> Grid, FVector GridAnchorWorldLocation, FVector GridSize, FVector RoomAnchorCoordinates, FVector RoomFootprint, FColor CellColor, float Duration, float Thickness)
{
	// Loop through the Z, Y, and X dimensions of the room footprint
	for (int32 Z = 0; Z < RoomFootprint.Z; Z++)
	{
		for (int32 Y = 0; Y < RoomFootprint.Y; Y++)
		{
			for (int32 X = 0; X < RoomFootprint.X; X++)
			{
				// Calculate the index of the current room cell in the grid
				int32 RoomX = X + RoomAnchorCoordinates.X;
				int32 RoomY = Y + RoomAnchorCoordinates.Y;
				int32 RoomZ = Z + RoomAnchorCoordinates.Z;

				// Ensure the room cell is within grid bounds
				if (RoomX >= 0 && RoomX < GridSize.X && RoomY >= 0 && RoomY < GridSize.Y && RoomZ >= 0 && RoomZ < GridSize.Z)
				{
					// Calculate the 1D index of the room cell in the flattened grid array
					int32 GridIndex = RoomX + (RoomY * GridSize.X) + (RoomZ * GridSize.X * GridSize.Y);

					if (Grid.IsValidIndex(GridIndex)) // Check if the cell is valid
					{
						// Calculate the world position of the center of the current room cell
						FVector CellCenterWorldLocation = GridAnchorWorldLocation + FVector(RoomX * CELL_LENGTH + CELL_LENGTH / 2.0f, RoomY * CELL_LENGTH + CELL_LENGTH / 2.0f, RoomZ * CELL_LENGTH + CELL_LENGTH / 2.0f);
						// Draw the debug box at the room cell's center
						DrawDebugCell(WorldContextObject, CellCenterWorldLocation, CellColor, Duration, Thickness);
					}
				}
			}
		}
	}
}

void ULevelGenerationFLibrary::InitializeGrid(TArray<bool>& Grid, FVector GridSize)
{
	int32 IntGridSize = GridSize.X * GridSize.Y * GridSize.Z;
	Grid.SetNum(IntGridSize);

	// Initialize all cells to 'false' (unoccupied)
	for (int32 i = 0; i < IntGridSize; i++)
	{
		Grid[i] = false;
	}
}

FVector ULevelGenerationFLibrary::GetSocketCoordinatesRelativeToSpaceAnchor(FVector SocketLocationRelativeToSpaceCenter, FVector SpaceSize)
{	FVector SocketCoordinatesRelativeToSpaceAnchor = FVector(0, 0, 0);
	
	float SpaceLengthX = (CELL_LENGTH * SpaceSize.X)+40;
	float SpaceLengthY = (CELL_LENGTH * SpaceSize.Y)+40;
	FVector SocketLocationRelativeToSpaceAnchor = FVector(0, 0, 0);
	SocketLocationRelativeToSpaceAnchor.X = SpaceLengthX/2 + SocketLocationRelativeToSpaceCenter.X;
	SocketLocationRelativeToSpaceAnchor.Y = SpaceLengthY/2 + SocketLocationRelativeToSpaceCenter.Y;

	SocketCoordinatesRelativeToSpaceAnchor.X = std::floor(SocketLocationRelativeToSpaceAnchor.X / CELL_LENGTH);
	SocketCoordinatesRelativeToSpaceAnchor.Y = std::floor(SocketLocationRelativeToSpaceAnchor.Y / CELL_LENGTH);

	if(SocketCoordinatesRelativeToSpaceAnchor.X == SpaceSize.X)
	{
		SocketCoordinatesRelativeToSpaceAnchor.X = SpaceSize.X - 1;
	}
	if(SocketCoordinatesRelativeToSpaceAnchor.Y == SpaceSize.Y)
	{
		SocketCoordinatesRelativeToSpaceAnchor.Y = SpaceSize.Y - 1;
	}
	if(SocketCoordinatesRelativeToSpaceAnchor.Z == SpaceSize.Z)
	{
		SocketCoordinatesRelativeToSpaceAnchor.Z = SpaceSize.Z - 1;
	}
		//UE_LOG(LogTemp, Warning, TEXT("SocketCoordinatesRelativeToSpaceAnchor.X: %f, SocketLocationRelativeToSpaceAnchor.X: %f, SocketLocationRelativeToSpaceCenter.X: %f, SpaceLengthX: %f"), SocketCoordinatesRelativeToSpaceAnchor.X, SocketLocationRelativeToSpaceAnchor.X, SocketLocationRelativeToSpaceCenter.X, SpaceLengthX);
		//UE_LOG(LogTemp, Warning, TEXT("SocketCoordinatesRelativeToSpaceAnchor.Y: %f, SocketLocationRelativeToSpaceAnchor.Y: %f, SocketLocationRelativeToSpaceCenter.Y: %f, SpaceLengthY: %f"), SocketCoordinatesRelativeToSpaceAnchor.Y, SocketLocationRelativeToSpaceAnchor.Y, SocketLocationRelativeToSpaceCenter.Y, SpaceLengthY);
	return SocketCoordinatesRelativeToSpaceAnchor;
}
FVector ULevelGenerationFLibrary::GetSocketCoordinatesInRoom(FVector RelativeLocation, FVector RoomFootprint, float GridCellLength, ESocketOrientation Orientation)
{
	// Ensure that the relative location is properly scaled based on the grid cell size

	float scaledLocationX = RelativeLocation.X / GridCellLength;
	float scaledLocationY = RelativeLocation.Y / GridCellLength;

	// Adjust the room's footprint bounds to center the calculations
	float HalfFootprintX = RoomFootprint.X / 2.0f;
	float HalfFootprintY = RoomFootprint.Y / 2.0f;

	// Calculate the coordinates within the room's grid footprint
	FVector CoordinatesInRoom;

	CoordinatesInRoom.X = std::floor(scaledLocationX + HalfFootprintX);
	CoordinatesInRoom.Y = std::floor(scaledLocationY + HalfFootprintY);
	//DEBUG LOG
	//ORIENTATION
	UE_LOG(LogTemp, Warning, TEXT("Orientation: %d"), (int)Orientation);
	UE_LOG(LogTemp, Warning, TEXT("HalfFootprintX: %f"), HalfFootprintX);
	UE_LOG(LogTemp, Warning, TEXT("HalfFootprintX: %f"), HalfFootprintX);
	//LOG SCALED LOCATION
	UE_LOG(LogTemp, Warning, TEXT("scaledLocationX: %f"), scaledLocationX);
	UE_LOG(LogTemp, Warning, TEXT("scaledLocationY: %f"), scaledLocationY);
	
	CoordinatesInRoom.Z = 0;
	if(CoordinatesInRoom.X == RoomFootprint.X)
	{
		CoordinatesInRoom.X = RoomFootprint.X - 1;
	}
	if(CoordinatesInRoom.Y == RoomFootprint.Y)
	{
		CoordinatesInRoom.Y = RoomFootprint.Y - 1;
	}
	if(CoordinatesInRoom.Z == RoomFootprint.Z)
	{
		CoordinatesInRoom.Z = RoomFootprint.Z - 1;
	}
	return CoordinatesInRoom;
}


bool ULevelGenerationFLibrary::IsCellOccupied(const TArray<bool>& Grid, int32 X, int32 Y, int32 Z, FVector GridSize)
{
	int32 Index = X + (Y * GridSize.X) + (Z * GridSize.X * GridSize.Y);
	if (Grid.IsValidIndex(Index))
	{
		return Grid[Index];
	}
	return false;  // Out of bounds, treat as unoccupied
}

TArray<bool> ULevelGenerationFLibrary::MarkCellOccupied(TArray<bool> Grid, int32 X, int32 Y, int32 Z, FVector GridSize)
{
	int32 Index = X + (Y * GridSize.X) + (Z * GridSize.X * GridSize.Y);
	if (Grid.IsValidIndex(Index))
	{
		Grid[Index] = true;
	}
	return Grid;
}

bool ULevelGenerationFLibrary::CanPlaceRoom(UObject* WorldContextObject,const TArray<bool>& Grid, FVector RoomFootprint, FVector StartPosition, FVector GridSize)
{
	for (int32 X = StartPosition.X-1; X < StartPosition.X + RoomFootprint.X; X++)
	{
		for (int32 Y = StartPosition.Y; Y < StartPosition.Y + RoomFootprint.Y; Y++)
		{
			for (int32 Z = StartPosition.Z; Z < StartPosition.Z + RoomFootprint.Z; Z++)
			{
				if (IsCellOccupied(Grid, X, Y, Z, GridSize))
				{
					float CellLength = 500.0f;
					FVector CellCenter = FVector(X * CellLength + CellLength / 2.0f, Y * CellLength + CellLength / 2.0f, Z * CellLength + CellLength / 2.0f);
					DrawDebugCell(WorldContextObject, CellCenter, FColor::Red, 60.0f, 10.0f);
					return false;
				}
			}
		}
	}
	return true;
}

TArray<bool> ULevelGenerationFLibrary::MarkRoomOccupied(TArray<bool> Grid, FVector RoomFootprint, FVector StartPosition, FVector GridSize)
{
	for (int32 X = StartPosition.X; X < StartPosition.X + RoomFootprint.X; X++)
	{
		for (int32 Y = StartPosition.Y; Y < StartPosition.Y + RoomFootprint.Y; Y++)
		{
			for (int32 Z = StartPosition.Z; Z < StartPosition.Z + RoomFootprint.Z; Z++)
			{
				Grid = MarkCellOccupied(Grid, X, Y, Z, GridSize);
			}
		}
	}
	return Grid;
}

FVector ULevelGenerationFLibrary::ConvertWorldLocationToGridCoordinates(FVector WorldLocation, FVector GridAnchorWorldLocation, FVector GridSize, float CellLength)
{
	// Calculate the offset from the grid anchor location
	FVector Offset = WorldLocation - GridAnchorWorldLocation;

	// Calculate the grid coordinates based on the cell length
	int32 X = FMath::FloorToInt(Offset.X / CellLength);
	int32 Y = FMath::FloorToInt(Offset.Y / CellLength);
	int32 Z = FMath::FloorToInt(Offset.Z / CellLength);

	// Clamp the grid coordinates to the grid size
	X = FMath::Clamp(X, 0, FMath::FloorToInt(GridSize.X) - 1);
	Y = FMath::Clamp(Y, 0, FMath::FloorToInt(GridSize.Y) - 1);
	Z = FMath::Clamp(Z, 0, FMath::FloorToInt(GridSize.Z) - 1);

	return FVector(X, Y, Z);
}

FVector ULevelGenerationFLibrary::ConvertGridCoordinatesToWorldLocation(FVector GridCoordinates, FVector GridAnchorWorldLocation, float CellLength)
{
	// Calculate the world location based on the grid coordinates and cell length
	FVector WorldLocation = GridAnchorWorldLocation + FVector(GridCoordinates.X * CellLength, GridCoordinates.Y * CellLength, GridCoordinates.Z * CellLength);

	return WorldLocation;
}

