// Fill out your copyright notice in the Description page of Project Settings.


#include "GOLSubsystem.h"

#include "CellRenderer.h"
#include "GOLSettings.h"
#include "Components/InstancedStaticMeshComponent.h"

void UGOLSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	m_CurrentTime += DeltaTime;
	if (m_CurrentTime > UpdateTime)
	{	
		Step();
		m_CurrentTime = 0.0f;
	}
}

void UGOLSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// Read Settings
	// If Settings are not GOLSettings, change it on Edit Properties
	if(AGOLSettings* Settings = Cast<AGOLSettings>(GetWorld()->GetWorldSettings()))
	{
		GridExtents = Settings->GridExtents;
		bAutoSimulate = Settings->bAutoSimulate;
		CellSize = Settings->CellSize;
		CellMesh = Settings->CellMesh;
		CellRendererClass = Settings->CellRendererClass;
	}
}

void UGOLSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TStatId UGOLSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UGOLSubsystem, STATGROUP_Tickables);
}

void UGOLSubsystem::InitCellGrid(TArray<int32> aliveCells)
{
	// Init cell state base on grid
	m_currentCellGrid.Init(false, GridExtents.X * GridExtents.Y);
	
	for (int32 cellIndex : aliveCells)
	{
		m_currentCellGrid[cellIndex] = true;
	}
	
	m_ThreadNumber = FMath::Clamp(m_ThreadNumber, 1,  m_currentCellGrid.Num());
	
	// Ensure Render actor is created
	if (ensureMsgf(m_RenderActor, TEXT("RenderActor should exists!")))
	{
		if (UInstancedStaticMeshComponent* ISMC = m_RenderActor->GetISMComponent())
		{
			if (CellMesh != nullptr)
			{
				ISMC->SetStaticMesh(CellMesh);
			
				m_instanceArrayMap.Reserve(m_currentCellGrid.Num());
				
				float XExtent = CellMesh->GetBounds().BoxExtent.X * 2 * CellSize.X;
				float YExtent = CellMesh->GetBounds().BoxExtent.Y * 2 * CellSize.Y;
				
				FVector InitialPos = m_RenderActor->GetActorLocation();
				InitialPos.X -= XExtent * 0.5 - (XExtent * GridExtents.Y) * 0.5;
				InitialPos.Y -= YExtent * 0.5 - (YExtent * GridExtents.X) * 0.5;
				
				FTransform Transform;
				Transform.SetScale3D(CellSize);
				
				for (int i = 0; i < m_currentCellGrid.Num(); ++i)
				{
					int xPos = (i / GridExtents.Y);
					int yPos = i % static_cast<int>(GridExtents.Y);
					Transform.SetLocation(InitialPos + FVector(-XExtent * yPos, -YExtent * xPos, 0.0));
					int32 index = ISMC->AddInstance(Transform);
					m_instanceArrayMap.Add(i, index);
					SetColorAlive(index, ISMC, m_currentCellGrid[i]);
				}
			}
		}
	}
}


void UGOLSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	// Spawn the render actor in the world
	if (ensureMsgf(!m_RenderActor, TEXT("RenderActor should not exist on BeginPlay!")))
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.bNoFail = true;
		
		SpawnInfo.ObjectFlags = RF_Transient;
		
		if (IsValid(CellRendererClass))
		{
			m_RenderActor = InWorld.SpawnActor<ACellRenderer>(CellRendererClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);
		}
		
	}
	
	InitCellGrid();
}

bool UGOLSubsystem::GetIsAlive(int x, int y) const
{
	if (ensureMsgf(IsValidIndex(x,y), TEXT("Cell inaccessible [%d,%d]"), x, y))
	{
		return m_currentCellGrid[x * GridExtents.Y + y];
	}
	
	return false;
}

FVector2D UGOLSubsystem::GetMatrixIndex(int arrPos) const
{
	return FVector2D(arrPos / static_cast<int>(GridExtents.Y), arrPos % static_cast<int>(GridExtents.Y));
}

void UGOLSubsystem::SetIsAlive(int x, int y, bool value)
{
	if (ensureMsgf(IsValidIndex(x,y), TEXT("Cell inaccessible [%d,%d]"), x, y))
	{
		m_currentCellGrid[x * GridExtents.Y + y] = value;
	}
}

void UGOLSubsystem::ChangeState(int32 index)
{
	// Here we are assuming the index given is following the array order. In this specific situation it makes sense
	// but if we have something like additional instances not based on array this can be problematic
	// To avoid it, we can have another map with the relationship instanceIndex<->array index
	if (m_currentCellGrid.IsValidIndex(index))
	{
		m_currentCellGrid[index] = !m_currentCellGrid[index];
		if (UInstancedStaticMeshComponent* ISMC = m_RenderActor->GetISMComponent())
		{
			SetColorAlive(index, ISMC, m_currentCellGrid[index]);
		}
	}
}

void UGOLSubsystem::Step()
{
	ComputeNextStep();
	UpdateNextStep();
	GenerationNumber++;
}

void UGOLSubsystem::EnableAnimation(bool bAnimation)
{
	if (m_RenderActor)
	{
		m_RenderActor->SetAnimationEnabled(bAnimation);
	}
}

bool UGOLSubsystem::GetEnableAnimation() const
{
	if (m_RenderActor)
	{
		return m_RenderActor->GetAnimationEnabled();
	}
	return false;
}

void UGOLSubsystem::SetThreads(int32 threads)
{
	if (threads > 0)
	{
		m_ThreadNumber = FMath::Min(threads, m_currentCellGrid.Num());
	}
}

void UGOLSubsystem::LoadExample(int32 rows, int32 cols, TArray<int> liveCells)
{	
	GridExtents.Set(rows, cols);
	if (ensureMsgf(m_RenderActor, TEXT("Render Actor should exists")))
	{
		if (UInstancedStaticMeshComponent* ISMC = m_RenderActor->GetISMComponent())
		{
			ISMC->ClearInstances();
		}
	}
	
	InitCellGrid(liveCells);
}

void UGOLSubsystem::Randomize()
{
	for (int i = 0; i < m_currentCellGrid.Num(); ++i)
	{
		m_currentCellGrid[i] = FMath::RandBool();
	}
}

TArray<int> UGOLSubsystem::GetAliveCellsArray() const
{
	TArray<int> aliveCells;
	for (int i = 0; i < m_currentCellGrid.Num(); ++i)
	{
		if (m_currentCellGrid[i])
		{
			aliveCells.Add(i);
		}
	}
	return aliveCells;
}

void UGOLSubsystem::ComputeNextStep()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ComputeNextStep);
	m_ChangedInstances.Empty();
	if (m_UseMultithreading)
	{
		int32 numCells = m_currentCellGrid.Num() / m_ThreadNumber;
		ParallelFor(m_ThreadNumber, [&](int32 index)
		{
			int32 startIndex = index * numCells;
			for (int i = startIndex; i < startIndex + numCells; ++i)
			{
				ComputeNextCellState(i);
			}
		});
		
		// This way we use batches and create lots of threads, This might no be the best option, as per 
		// https://forums.unrealengine.com/t/parallelfor-optimization/1680779/5, it's better to create less threads
		// to do a lot of samll calculations that lots of threads for 1 simple task
		// for (int i = 0; i < m_BatchNumber; ++i)
		// {
		// 	ParallelFor(numCells, [numCells, i, this](int32 index)
		// 	{
		// 		ComputeNextCellState(index + numCells * i );
		// 	});
		// }
	}
	else
	{
		for (int cellIndex = 0; cellIndex < m_currentCellGrid.Num(); ++cellIndex)
		{
			ComputeNextCellState(cellIndex);
		}
	}
}

void UGOLSubsystem::ComputeNextCellState(int cellIndex)
{
	// get neighbours and decide if cell is alive or not
	bool isAlive = m_currentCellGrid[cellIndex];
	int liveNeight = 0;
	FVector2D cellPos = GetMatrixIndex(cellIndex);
	for (int i = -1; i <=1; ++i)
	{
		for (int j = -1; j <=1; ++j)
		{
			if (i == 0 && j == 0)
			{
				continue;
			}
			int cellX = cellPos.X + i;
			int cellY = cellPos.Y + j;
			if (IsValidIndex(cellX, cellY))
			{
				liveNeight += GetIsAlive(cellX, cellY);
			}
		}
	}
	if (isAlive)
	{
		if (liveNeight < 2 || liveNeight > 3)
		{
			FScopeLock Lock(&DataGuard);
			// save change on cell
			m_ChangedInstances.Add(cellIndex);
		}
	}
	else
	{
		if (liveNeight == 3)
		{
			FScopeLock Lock(&DataGuard);
			// Save change on cell
			m_ChangedInstances.Add(cellIndex);
		}
	}
}

void UGOLSubsystem::UpdateNextStep()
{
	
	if (IsValid(m_RenderActor))
	{
		if (UInstancedStaticMeshComponent* ISMC = m_RenderActor->GetISMComponent())
		{
			for (int i : m_ChangedInstances)
			{
				int32 index = m_instanceArrayMap[i];
				{
					// Not really needed at this point, however if we got a sepatarion between reading/writing on the buffer this saves the access
					FScopeLock Lock(&DataGuard);
					m_currentCellGrid[index] = !m_currentCellGrid[index];
				}
				SetColorAlive(index, ISMC, m_currentCellGrid[i]);
			}
		}
	}
}

bool UGOLSubsystem::IsValidIndex(int x, int y) const
{
	return x >= 0 && y >= 0 && x < GridExtents.X && y < GridExtents.Y;
}

void UGOLSubsystem::SetColorAlive(int32 instanceIndex, TObjectPtr<UInstancedStaticMeshComponent> ISMcomponent, bool isAlive)
{
	if (IsValid(ISMcomponent))
	{
		FColor color = isAlive ? FColor::Red : FColor::White;
		ISMcomponent->SetCustomDataValue(instanceIndex, 0, color.R);
		ISMcomponent->SetCustomDataValue(instanceIndex, 1, color.G);
		ISMcomponent->SetCustomDataValue(instanceIndex, 2, color.B);
	}
}

