// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GOLSubsystem.generated.h"

/**
 * Subsystem that spawn, store and updates GOL simulation
 */
class ACellRenderer;

UCLASS()
class GOLV2_API UGOLSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return bAutoSimulate; }
	// ~ begin USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual TStatId GetStatId() const override;
	// ~ end USubsystem interface

	// ~ begin UWorldSubsystem interface
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	// ~ end UWorldSubsystem interface
	
	// Return if a cell in [x,y] is alive or not
	bool GetIsAlive(int x, int y) const;
	
	// Return coordinates on matrix for cell in array
	FVector2D GetMatrixIndex(int arrPos) const;
	
	// Set a cell state
	void SetIsAlive(int x, int y, bool value);
	
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	void ChangeState(int32 index);
	
	// Move forward one step
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	void Step();
	
	// Move forward one step
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	void SetAuto(bool bAuto) { bAutoSimulate = bAuto; };
	
	// Set Animation Enabled
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	void EnableAnimation(bool bAnimation);
	
	// Get if animation is enabled
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	bool GetEnableAnimation() const;
	
	// Set Multithreading enabled
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	FORCEINLINE void EnableMultiThreading(bool bMultithreading) { m_UseMultithreading = bMultithreading; };
	
	// Getter
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	FORCEINLINE bool GetEnableMultithreading() const { return m_UseMultithreading; };
	
	// Set Multithreading threads to use
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	FORCEINLINE void SetThreads(int32 threads);
	
	// Getter
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	FORCEINLINE int32 GetThreads() const { return m_ThreadNumber; };
	
	// Load an example
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	void LoadExample(int32 rows, int32 cols, TArray<int> liveCells);
	
	//Sets a random value 
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	void Randomize();
	
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	TArray<int> GetAliveCellsArray() const;
	
	UFUNCTION(BlueprintCallable, Category = "Simulation")
	FVector2D GetGridSize() const { return GridExtents; };
	
protected: 
	
	// Calculate next Step on buffer
	void ComputeNextStep();

	// swap current buffer and updates visuals
	void UpdateNextStep();
	
	// Function to init and prepare a cell grid
	void InitCellGrid(TArray<int32> aliveCells = TArray<int32>());
	
private:
	// return if [x,y] is a valid index
	bool IsValidIndex(int x, int y) const;
	// Set Color based on value on cell
	void SetColorAlive(int32 instanceIndex, TObjectPtr<UInstancedStaticMeshComponent> ISMcomponent, bool isAlive);
	//Function to compute next cell
	void ComputeNextCellState(int cellIndex);
	
public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Simulation")
	int32 GenerationNumber = 0;
	
	UPROPERTY(BlueprintReadWrite, Category = "Simulation", meta = (ForceUnits = "s"))
	float UpdateTime = 0.2f;
	
private:
	// Simulation properties
	FVector2D GridExtents = FVector2D::ZeroVector;
	
	// AutoRun simulation
	bool bAutoSimulate = false;
	
	// Scale of cells
	FVector CellSize = FVector::OneVector;
	
	// Mesh to use on Cell
	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> CellMesh = nullptr;
	
	// Array of 0/1 values
	TArray<bool> m_currentCellGrid; 
	
	// store arrayIndex <-> instance id 
	TMap<int, int32> m_instanceArrayMap;
	
	// Array of index for changed items. Expect 10 by guess
	TArray<int32, TInlineAllocator<10>> m_ChangedInstances;
	
	// Created renderer actor
	UPROPERTY(Transient)
	TObjectPtr<ACellRenderer> m_RenderActor = nullptr;
	
	// Accumulated delta time
	float m_CurrentTime = 0.0f;
	
	// Render Class to use
	UPROPERTY(Transient)
	TSubclassOf<ACellRenderer> CellRendererClass;
	
	// Flag to use multithreading calculations
	bool m_UseMultithreading = false;
	// Number of threads to use
	int32 m_ThreadNumber = 6;
	
	// Marked as mutable as it's often warranted by design
	mutable FCriticalSection DataGuard;
};
