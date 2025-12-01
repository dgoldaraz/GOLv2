// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCell.h"


// Sets default values
ABaseCell::ABaseCell()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseCell::SetIsAlive(bool isAlive)
{
	m_isAlive = isAlive;
	UpdateVisuals(m_isAlive);
}

// Called when the game starts or when spawned
void ABaseCell::BeginPlay()
{
	Super::BeginPlay();
	
}

