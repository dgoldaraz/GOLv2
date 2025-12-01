// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseCell.generated.h"

/**
 * Base class of a cell
 */
UCLASS()
class GOLV2_API ABaseCell : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseCell();

	// Getter
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsAlive() const { return m_isAlive; }
	
	// function to set Alive state
	void SetIsAlive(bool isAlive);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Function called on cell change
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateVisuals(bool isAlive);

private:
	bool m_isAlive = false;
	
};
