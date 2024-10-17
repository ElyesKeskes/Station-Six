#include "BlindPursuitProject/Public/LevelGenerationFLibrary.h"

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
TArray<TSubclassOf<APackedLevelActor>> ULevelGenerationFLibrary::LoadRoomLibrary(const FString& Path)
{
    // Ensure the path is valid (starts with "/Game/")
    if (!Path.StartsWith("/Game"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid path: %s. Path must start with /Game/"), *Path);
        return {};
    }

    // Create an object library that will hold our room assets
    UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(APackedLevelActor::StaticClass(), false, GIsEditor);

    // Load assets from the relative content browser path
    ObjectLibrary->LoadAssetDataFromPath(Path);

    // Array to hold the asset data
    TArray<FAssetData> AssetDatas;
    ObjectLibrary->GetAssetDataList(AssetDatas);

    // Array to store the filtered room classes
    TArray<TSubclassOf<APackedLevelActor>> RoomLibrary;

    // Iterate through the loaded assets and filter by APackedLevelActor class
    for (const FAssetData& AssetData : AssetDatas)
    {
        UClass* AssetClass = AssetData.GetClass();
        if (AssetClass && AssetClass->IsChildOf(APackedLevelActor::StaticClass()))
        {
            // Add the class to the RoomLibrary array
            RoomLibrary.Add(AssetClass);
        }
    }

    return RoomLibrary;
}

void ULevelGenerationFLibrary::InitializeGrid(TArray<bool>& Grid, int32 XSize, int32 YSize, int32 ZSize)
{
	int32 GridSize = XSize * YSize * ZSize;
	Grid.SetNum(GridSize);

	// Initialize all cells to 'false' (unoccupied)
	for (int32 i = 0; i < GridSize; i++)
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

void ULevelGenerationFLibrary::MarkCellOccupied(TArray<bool>& Grid, int32 X, int32 Y, int32 Z, int32 XSize, int32 YSize)
{
	int32 Index = X + (Y * XSize) + (Z * XSize * YSize);
	if (Grid.IsValidIndex(Index))
	{
		Grid[Index] = true;
	}
}

bool ULevelGenerationFLibrary::CanPlaceRoom(const TArray<bool>& Grid, FVector RoomFootprint, FVector StartPosition, int32 XSize, int32 YSize, int32 ZSize)
{
	for (int32 X = StartPosition.X; X < StartPosition.X + RoomFootprint.X; X++)
	{
		for (int32 Y = StartPosition.Y; Y < StartPosition.Y + RoomFootprint.Y; Y++)
		{
			for (int32 Z = StartPosition.Z; Z < StartPosition.Z + RoomFootprint.Z; Z++)
			{
				if (IsCellOccupied(Grid, X, Y, Z, XSize, YSize))
				{
					return false;  // Room overlaps with an existing occupied cell
				}
			}
		}
	}
	return true;  // No overlap
}

void ULevelGenerationFLibrary::MarkRoomOccupied(TArray<bool>& Grid, FVector RoomFootprint, FVector StartPosition, int32 XSize, int32 YSize, int32 ZSize)
{
	for (int32 X = StartPosition.X; X < StartPosition.X + RoomFootprint.X; X++)
	{
		for (int32 Y = StartPosition.Y; Y < StartPosition.Y + RoomFootprint.Y; Y++)
		{
			for (int32 Z = StartPosition.Z; Z < StartPosition.Z + RoomFootprint.Z; Z++)
			{
				MarkCellOccupied(Grid, X, Y, Z, XSize, YSize);
			}
		}
	}
}
