// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Innhabitant.generated.h"

UENUM(BlueprintType)
enum class ZombieState : uint8 {
	Healthy UMETA(DisplayName = "Healthy"),
	Bitten UMETA(DisplayName = "Bitten"),
	Zombie UMETA(DisplayName = "Zombie")
};

UCLASS()
class PROJECTZOMBEE_API AInnhabitant : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AInnhabitant();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	ZombieState currentState = (ZombieState)0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	ZombieState prevState = (ZombieState)0;



	static int32 BittenToday;

	UFUNCTION(BlueprintCallable)
	int32 GetBittenToday();

	UFUNCTION(BlueprintCallable)
	void SetBittenToday(int32 newValue);
};
