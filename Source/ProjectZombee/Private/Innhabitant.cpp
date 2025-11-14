// Fill out your copyright notice in the Description page of Project Settings.


#include "Innhabitant.h"

// Sets default values
AInnhabitant::AInnhabitant()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInnhabitant::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInnhabitant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	if (prevState != currentState)
	{
		switch (currentState)
		{
		case Healthy:
			break;
		case Bitten:
			break;
		case Zombie:
			break;
		default:
			break;
		}
	}

	prevState = currentState;*/
}

// Called to bind functionality to input
void AInnhabitant::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

