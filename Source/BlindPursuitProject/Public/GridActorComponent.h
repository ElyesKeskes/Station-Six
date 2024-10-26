// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//Include function library
#include "LevelGenerationFLibrary.h"
#include "Components/ActorComponent.h"
#include "GridActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDPURSUITPROJECT_API UGridActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGridActorComponent();
	
	//Define an array of 3 arrays representing a 3D grid
	TArray<TArray<TArray<bool>>> Grid;
	//Define grid dimensions
	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	int32 GridXSize;
	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	int32 GridYSize;
	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	int32 GridZSize;
	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	int32 GridCellSize; //Cubic cells
	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	FVector GridAnchorWorldPosition; //World position of the grid anchor, which is the cell at the top-left corner of the grid, the center of the that cell
	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	bool DrawDebug=true;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "Grid")
	void InitializeGrid(int32 XSize, int32 YSize, int32 ZSize, int32 CellSize, FVector AnchorWorldPosition);
	UFUNCTION(BlueprintCallable, Category = "Grid")
	void MarkSpaceOccupied(FVector SpaceAnchorPosition, FVector SpaceSize);
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DrawDebugCell(FVector CellCoordinates, FColor CellColor, float Duration, float Thickness);
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DrawDebugGrid();
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DrawDebugSpace(FVector SpaceAnchorCoordinates, FVector SpaceSize, FColor SpaceColor, float Duration,
	                    float Thickness);
	UFUNCTION(BlueprintPure, Category = "Grid")
	 FVector ConvertFromGridCoordinatesToWorldLocation(FVector GridCoordinates);
	UFUNCTION(BlueprintPure, Category = "Grid")
	FVector ConvertFromWorldLocationToGridCoordinates(FVector WorldLocation);
	UFUNCTION(BlueprintPure, Category = "Grid")
	bool CheckSpaceOccupied(FVector SpaceAnchorCoordinates, FVector SpaceSize);
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FVector GetSocketCoordinatesInRoom(FVector RelativeLocation, FVector RoomSize);
	UFUNCTION(BlueprintPure, Category = "Grid")
	FVector GetSpaceCenterWorldLocation(FVector SpaceAnchorCoordinates, FVector SpaceSize);
	UFUNCTION(BlueprintPure, Category = "Grid")
	FVector GetSpaceToConnectAnchorCoordinates(FVector ExistingSpaceAnchorCoordinates, FRoomSocketData SpaceToConnectSocketData,
	                                  FRoomSocketData ExistingSpaceSocketData);
};
