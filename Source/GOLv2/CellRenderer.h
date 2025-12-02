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
	
	virtual void BeginPlay() override;
	
	// Return ISMComponent
	TObjectPtr<UInstancedStaticMeshComponent> GetISMComponent() const { return ISMComponent;}
	
	UFUNCTION(BlueprintCallable)
	void SetAnimationEnabled(bool bAnimation);
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetAnimationEnabled() const {return m_IsAnimated;};
	
	// Method to update Animation
	UFUNCTION(BlueprintCallable)
	void UpdateAnimation(float Value);
	
	// Method to change rotation animation
	UFUNCTION(BlueprintCallable)
	void ChangeUpdateAnimation();
	
protected:
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInstancedStaticMeshComponent> ISMComponent;
	
	// Value to rotate when animating
	UPROPERTY(EditAnywhere)
	float UpdateRotationValue = 30.0f;
	
	bool m_IsAnimated = true;
	
	int32 m_CurrentRotation = 0;
	
	// Array of Rotations
	TArray<FRotator, TInlineAllocator<4>> m_Rotations;
};
