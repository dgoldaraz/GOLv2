// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "GOLSettings.generated.h"

class ACellRenderer;
/**
 * Game of life simulation settings. Allow to configure subsystem settings on editor
 */
UCLASS()
class GOLV2_API AGOLSettings : public AWorldSettings
{
	GENERATED_BODY()
	
public:
	// Grid extents on Init
	UPROPERTY(EditAnywhere, Category = "Simulation")
	FVector2D GridExtents = FVector2D(20,20);
	
	// Use simulation
	UPROPERTY(EditAnywhere, Category = "Simulation")
	bool bAutoSimulate = false;
	
	// Scale of cells
	UPROPERTY(EditAnywhere, Category = "Simulation")
	FVector CellSize = FVector::OneVector;
	
	// Mesh to use on cells
	UPROPERTY(EditAnywhere, Category = "Simulation")
	TObjectPtr<UStaticMesh> CellMesh = nullptr;
	
	// Render Class to use
	UPROPERTY(EditAnywhere, Category = "Simulation")
	TSubclassOf<ACellRenderer> CellRendererClass;
};
