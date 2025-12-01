// Fill out your copyright notice in the Description page of Project Settings.


#include "CellRenderer.h"

#include "Components/InstancedStaticMeshComponent.h"


// Sets default values
ACellRenderer::ACellRenderer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	USceneComponent* SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	SetRootComponent(SceneRootComponent);
	ISMComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMComponent"));
	ISMComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ISMComponent->SetNumCustomDataFloats(3);
	ISMComponent->SetupAttachment(GetRootComponent());
}
