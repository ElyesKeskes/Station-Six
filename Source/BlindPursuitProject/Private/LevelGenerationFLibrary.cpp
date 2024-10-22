#include "BlindPursuitProject/Public/LevelGenerationFLibrary.h"
#include <cmath>

#include "DrawDebugHelpers.h" // Include this to use the debug draw functions

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
						DrawDebugBox(WorldContextObject->GetWorld(), CellCenter, FVector(CellLength / 2.0f), FColor::Red, false, -1.0f, 0, 1.0f);
					}
					else
					{
						DrawDebugBox(WorldContextObject->GetWorld(), CellCenter, FVector(CellLength / 2.0f), FColor::Green, false, -1.0f, 0, 2.0f);
					}
				}
			}
		}
	}
}

void ULevelGenerationFLibrary::DrawDebugGridCustom(UObject* WorldContextObject, TArray<bool> Grid, FVector GridAnchorWorldLocation, FVector GridSize, float CellSize, FVector RoomAnchorCoordinates, FVector RoomFootprint, FColor CellColor, float Duration, float Thickness)
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
						FVector CellCenterWorldLocation = GridAnchorWorldLocation + FVector(RoomX * CellSize + CellSize / 2.0f, RoomY * CellSize + CellSize / 2.0f, RoomZ * CellSize + CellSize / 2.0f);

						// Draw the debug box at the room cell's center
						DrawDebugBox(WorldContextObject->GetWorld(), CellCenterWorldLocation, FVector(CellSize / 2.0f), CellColor, false, Duration, 0, Thickness);
					}
				}
			}
		}
	}
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

	CoordinatesInRoom.X = std::ceil(scaledLocationX + HalfFootprintX);
	CoordinatesInRoom.Y = std::ceil(scaledLocationY + HalfFootprintY);
	//DEBUG LOG
	//ORIENTATION
	UE_LOG(LogTemp, Warning, TEXT("Orientation: %d"), (int)Orientation);
	UE_LOG(LogTemp, Warning, TEXT("HalfFootprintX: %f"), HalfFootprintX);
	UE_LOG(LogTemp, Warning, TEXT("HalfFootprintX: %f"), HalfFootprintX);
	//LOG SCALED LOCATION
	UE_LOG(LogTemp, Warning, TEXT("scaledLocationX: %f"), scaledLocationX);
	UE_LOG(LogTemp, Warning, TEXT("scaledLocationY: %f"), scaledLocationY);
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


void ULevelGenerationFLibrary::RotateClockwiseBy90(AActor* RoomActor, TArray<FRoomSocketData>& SocketsData)
{
	// Rotate the room actor by 90 degrees on the Z axis
	if (RoomActor)
	{
		FRotator Rotation = FRotator(0, 90, 0);
		RoomActor->AddActorWorldRotation(Rotation);

		// Find the RoomComponent in the Blueprint
		UActorComponent* RoomComponent = RoomActor->FindComponentByClass<UActorComponent>();

		if (RoomComponent && RoomComponent->GetName() == "RoomComponent") 
		{
			// Use reflection to access GridFootprint variable
			FVector* GridFootprint = RoomComponent->GetClass()->FindPropertyByName(FName("GridFootprint"))->ContainerPtrToValuePtr<FVector>(RoomComponent);
			
			if (GridFootprint)
			{
				// Rotate the GridFootprint by swapping X and Y
				float Temp = GridFootprint->X;
				GridFootprint->X = GridFootprint->Y;
				GridFootprint->Y = Temp;
			}
		}
	}

	// Update the socket orientations after the rotation
	for (FRoomSocketData& Socket : SocketsData)
	{
		switch (Socket.Orientation)
		{
		case ESocketOrientation::North:
			Socket.Orientation = ESocketOrientation::East;
			break;
		case ESocketOrientation::East:
			Socket.Orientation = ESocketOrientation::South;
			break;
		case ESocketOrientation::South:
			Socket.Orientation = ESocketOrientation::West;
			break;
		case ESocketOrientation::West:
			Socket.Orientation = ESocketOrientation::North;
			break;
		}
	}
}


void ULevelGenerationFLibrary::RotateToAlignWithSocket(AActor* RoomActor, TArray<FRoomSocketData>& SocketsData, int32 OurSocketIndex, const FRoomSocketData& OtherRoomSocket)
{
	if (SocketsData.IsValidIndex(OurSocketIndex))
	{
		// Get the orientation of our socket and the target socket
		ESocketOrientation OurOrientation = SocketsData[OurSocketIndex].Orientation;
		ESocketOrientation TargetOrientation = OtherRoomSocket.Orientation;

		// Lambda to check if the orientations are inverse
		auto IsInverseOrientation = [](ESocketOrientation A, ESocketOrientation B) -> bool
		{
			// Compare the orientations based on their integer values
			return (static_cast<uint8>(A) + 2) % 4 == static_cast<uint8>(B);
		};

		// Rotate until the orientations are inverse
		int32 RotationAttempts = 0;
		while (!IsInverseOrientation(OurOrientation, TargetOrientation) && RotationAttempts < 4) // Maximum of 4 attempts (360 degrees)
		{
			RotateClockwiseBy90(RoomActor, SocketsData);
			OurOrientation = SocketsData[OurSocketIndex].Orientation;
			RotationAttempts++;
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

bool ULevelGenerationFLibrary::IsCellOccupied(const TArray<bool>& Grid, int32 X, int32 Y, int32 Z, int32 XSize, int32 YSize)
{
	int32 Index = X + (Y * XSize) + (Z * XSize * YSize);
	if (Grid.IsValidIndex(Index))
	{
		return Grid[Index];
	}
	return false;  // Out of bounds, treat as unoccupied
}

TArray<bool> ULevelGenerationFLibrary::MarkCellOccupied(TArray<bool> Grid, int32 X, int32 Y, int32 Z, int32 XSize, int32 YSize)
{
	int32 Index = X + (Y * XSize) + (Z * XSize * YSize);
	if (Grid.IsValidIndex(Index))
	{
		Grid[Index] = true;
	}
	return Grid;
}

bool ULevelGenerationFLibrary::CanPlaceRoom(UObject* WorldContextObject,const TArray<bool>& Grid, FVector RoomFootprint, FVector StartPosition, FVector GridSize)
{
	for (int32 X = StartPosition.X; X < StartPosition.X + RoomFootprint.X; X++)
	{
		for (int32 Y = StartPosition.Y; Y < StartPosition.Y + RoomFootprint.Y; Y++)
		{
			for (int32 Z = StartPosition.Z; Z < StartPosition.Z + RoomFootprint.Z; Z++)
			{
				if (IsCellOccupied(Grid, X, Y, Z, GridSize.X, GridSize.Y))
				{
					float CellLength = 500.0f;
					FVector CellCenter = FVector(X * CellLength + CellLength / 2.0f, Y * CellLength + CellLength / 2.0f, Z * CellLength + CellLength / 2.0f);
					DrawDebugBox(WorldContextObject->GetWorld(), CellCenter, FVector(CellLength / 2.0f), FColor::Yellow, false, 120.0f, 0, 5.0f);
					return false;
				}
			}
		}
	}
	return true;
}/*
{
	for (int32 X = StartPosition.X; X < StartPosition.X + RoomFootprint.X; X++)
	{
		for (int32 Y = StartPosition.Y; Y < StartPosition.Y + RoomFootprint.Y; Y++)
		{
			for (int32 Z = StartPosition.Z; Z < StartPosition.Z + RoomFootprint.Z; Z++)
			{
				if (IsCellOccupied(Grid, X, Y, Z, XSize, YSize))
				{	float CellLength = 500.0f;
					FVector CellCenter = FVector(X * CellLength + CellLength / 2.0f, Y * CellLength + CellLength / 2.0f, Z * CellLength + CellLength / 2.0f);
					DrawDebugBox(WorldContextObject->GetWorld(), CellCenter, FVector(CellLength / 2.0f), FColor::Yellow, false, 120.0f, 0, 5.0f);
					return false;  // Room overlaps with an existing occupied cell
				}
			}
		}
	}
	return true;  // No overlap
}*/

TArray<bool> ULevelGenerationFLibrary::MarkRoomOccupied(TArray<bool> Grid, FVector RoomFootprint, FVector StartPosition, FVector GridSize)
{
	for (int32 X = StartPosition.X; X < StartPosition.X + RoomFootprint.X; X++)
	{
		for (int32 Y = StartPosition.Y; Y < StartPosition.Y + RoomFootprint.Y; Y++)
		{
			for (int32 Z = StartPosition.Z; Z < StartPosition.Z + RoomFootprint.Z; Z++)
			{
				Grid = MarkCellOccupied(Grid, X, Y, Z, GridSize.X, GridSize.Y);
			}
		}
	}
	return Grid;
}

