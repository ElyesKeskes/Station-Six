#pragma once

#include "CoreMinimal.h"
#include <cmath>
#include "Kismet/BlueprintFunctionLibrary.h"
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

    // Properties
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    ESocketOrientation Orientation = ESocketOrientation::North; // Default value

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool InUse = false; // Default value

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool Connected = false; // Default value

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    AActor* ConnectedRoom = nullptr; // Default value

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector RelativeLocation = FVector::ZeroVector; // Default value

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector CoordinatesInRoom = FVector::ZeroVector; // Default value

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int Index = 0; // Default value

    // Constructor for additional initialization if needed
    FRoomSocketData()
        : Orientation(ESocketOrientation::North),
          InUse(false),
          Connected(false),
          ConnectedRoom(nullptr),
          RelativeLocation(FVector::ZeroVector),
          CoordinatesInRoom(FVector::ZeroVector),
          Index(0)
    {
    }
};

UCLASS()
class BLINDPURSUITPROJECT_API ULevelGenerationFLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    static void DrawDebugCell(UObject* WorldContextObject, FVector CellCenter, FColor CellColor, float Duration, float Thickness);

    UFUNCTION(BlueprintCallable, Category="Level Generation")
    static FVector GetSocketCoordinatesInRoom(FVector RelativeLocation, FVector RoomFootprint, float GridCellLength,ESocketOrientation Orientation);
    UFUNCTION(BlueprintCallable, Category="Level Generation",meta = (WorldContext=WorldContextObject))
    static void DrawDebugGrid(UObject* WorldContextObject, TArray<bool> Grid, int32 XSize, int32 YSize, int32 ZSize, float CellLength, FVector
                              GridTopLeftCellCenterLocation);
    UFUNCTION(BlueprintCallable, Category="Level Generation",meta = (WorldContext=WorldContextObject))
    static void DrawDebugGridCustom(UObject* WorldContextObject, TArray<bool> Grid, FVector
                                    GridAnchorWorldLocation, FVector GridSize, FVector RoomAnchorCoordinates, FVector RoomFootprint, FColor
                                    CellColor, float Duration, float
                                    Thickness);

    // Initialize a grid (1D array for a 2D or 3D grid)
    UFUNCTION(BlueprintCallable, Category = "Grid")
    static void InitializeGrid(TArray<bool>& Grid, FVector GridSize);
    UFUNCTION(BlueprintCallable, Category = "Grid")
    static FVector GetSocketCoordinatesRelativeToSpaceAnchor(FVector SocketLocationRelativeToSpaceCenter, FVector SpaceSize);

    // Check if a grid cell is occupied (in the 1D array)
    UFUNCTION(BlueprintPure, Category = "Grid")
    static bool IsCellOccupied(const TArray<bool>& Grid, int32 X, int32 Y, int32 Z, FVector GridSize);

    // Mark a grid cell as occupied
    UFUNCTION(BlueprintCallable, Category = "Grid")
    static TArray<bool> MarkCellOccupied(TArray<bool> Grid, int32 X, int32 Y, int32 Z, FVector GridSize);

    // Check if a room can be placed in the grid without overlap
    UFUNCTION(BlueprintPure, Category = "Grid",meta = (WorldContext=WorldContextObject))
    static bool CanPlaceRoom(UObject* WorldContextObject, const TArray<bool>& Grid, FVector RoomFootprint, FVector StartPosition, FVector GridSize);
    UFUNCTION(BlueprintCallable, Category = "Grid")
    static TArray<bool> MarkRoomOccupied(TArray<bool> Grid, FVector RoomFootprint, FVector StartPosition, FVector GridSize);
    UFUNCTION(BlueprintPure, Category = "Grid")
    static FVector ConvertWorldLocationToGridCoordinates(FVector WorldLocation, FVector GridAnchorWorldLocation,
                                                  FVector GridSize,
                                                  float CellLength);
    UFUNCTION(BlueprintPure, Category = "Grid")
    static FVector ConvertGridCoordinatesToWorldLocation(FVector GridCoordinates, FVector GridAnchorWorldLocation,
                                                  float CellLength);
};


