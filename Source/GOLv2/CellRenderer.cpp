// Fill out your copyright notice in the Description page of Project Settings.


#include "CellRenderer.h"

#include "Components/InstancedStaticMeshComponent.h"


// Sets default values
ACellRenderer::ACellRenderer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	SetRootComponent(SceneRootComponent);
	ISMComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMComponent"));
	ISMComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ISMComponent->SetNumCustomDataFloats(3);
	ISMComponent->SetupAttachment(GetRootComponent());
}

void ACellRenderer::BeginPlay()
{
	Super::BeginPlay();
	
	m_Rotations.Add(FRotator(45.0f, 0.0f, 0.0f));
	m_Rotations.Add(FRotator(-45.0f, 0.0f, 0.0f));
	m_Rotations.Add(FRotator(0.0f, 0.0f, 45.0f));
	m_Rotations.Add(FRotator(0.0f, 0.0f, -45.0f));
}

void ACellRenderer::SetAnimationEnabled(bool bAnimation)
{
	if (bAnimation != m_IsAnimated)
	{
		m_IsAnimated = bAnimation;
		SetActorTickEnabled(m_IsAnimated);
	}
}

void ACellRenderer::UpdateAnimation(float Value)
{
	FRotator NewRotation = FMath::Lerp(FRotator::ZeroRotator, m_Rotations[m_CurrentRotation], Value);
	// Set Instances to new rotation
	for (int32 instanceIndex = 0; instanceIndex < ISMComponent->GetNumInstances(); ++instanceIndex)
	{
		FTransform NewTransform;
		ISMComponent->GetInstanceTransform(instanceIndex, NewTransform);
		NewTransform.SetRotation(NewRotation.Quaternion());
		ISMComponent->UpdateInstanceTransform(instanceIndex, NewTransform);
	}
}

void ACellRenderer::ChangeUpdateAnimation()
{
	m_CurrentRotation = (m_CurrentRotation + 1) % 4;
}
