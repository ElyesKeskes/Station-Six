// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/ObjectLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PackedLevelActor/PackedLevelActor.h"
#include "LevelGenerationFLibrary.generated.h"


UENUM(BlueprintType)
enum class ESocketOrientation : uint8
{
    North,
    East,
    South,
    West
};



USTRUCT(BlueprintType)
struct FRoomSocketData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    ESocketOrientation Orientation;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool InUse;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool Connected;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    AActor* ConnectedRoom;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector RelativeLocation;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector CoordinatesInRoom;
};

UCLASS()
class BLINDPURSUITPROJECT_API ULevelGenerationFLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    
    UFUNCTION(BlueprintCallable, Category="Level Generation",meta = (WorldContext=WorldContextObject))
    static void DrawDebugGrid(UObject* WorldContextObject, TArray<bool> Grid, int32 XSize, int32 YSize, int32 ZSize, float CellLength, FVector
                              GridTopLeftCellCenterLocation);
    UFUNCTION(BlueprintCallable, Category="Level Generation")
    static FVector GetSocketCoordinatesInRoom(FVector RelativeLocation, FVector RoomFootprint, float GridCellLength,ESocketOrientation Orientation);
    // Rotate the room by 90 degrees clockwise and update socket orientations
    UFUNCTION(BlueprintCallable, Category="Level Generation")
    static void RotateClockwiseBy90(AActor* RoomActor, TArray<FRoomSocketData>& SocketsData);
    
    // Rotate the room until the sockets are aligned
    UFUNCTION(BlueprintCallable, Category="Level Generation")
    static void RotateToAlignWithSocket(AActor* RoomActor, TArray<FRoomSocketData>& SocketsData, int32 OurSocketIndex, const FRoomSocketData& OtherRoomSocket);
    // Initialize a grid (1D array for a 2D or 3D grid)
    UFUNCTION(BlueprintCallable, Category = "Grid")
    static void InitializeGrid(TArray<bool>& Grid, int32 XSize, int32 YSize, int32 ZSize = 1);

    // Check if a grid cell is occupied (in the 1D array)
    UFUNCTION(BlueprintPure, Category = "Grid")
    static bool IsCellOccupied(const TArray<bool>& Grid, int32 X, int32 Y, int32 Z, int32 XSize, int32 YSize);

    // Mark a grid cell as occupied
    UFUNCTION(BlueprintCallable, Category = "Grid")
    static TArray<bool> MarkCellOccupied(TArray<bool> Grid, int32 X, int32 Y, int32 Z, int32 XSize, int32 YSize);

    // Check if a room can be placed in the grid without overlap
    UFUNCTION(BlueprintPure, Category = "Grid")
    static bool CanPlaceRoom(const TArray<bool>& Grid, FVector RoomFootprint, FVector StartPosition, int32 XSize, int32 YSize, int32 ZSize);
    UFUNCTION(BlueprintCallable, Category = "Grid")
    static TArray<bool> MarkRoomOccupied(TArray<bool> Grid, FVector RoomFootprint, FVector StartPosition, int32 XSize, int32 YSize);
};


