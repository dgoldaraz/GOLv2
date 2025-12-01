// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * Renderer or all Cells in game. Using ISM and change of visuals
 */
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CellRenderer.generated.h"

UCLASS()
class GOLV2_API ACellRenderer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACellRenderer();
	
	// Return ISMComponent
	TObjectPtr<UInstancedStaticMeshComponent> GetISMComponent() const { return ISMComponent;}
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInstancedStaticMeshComponent> ISMComponent;
};
