// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZombieManager.generated.h"


// Holds the amount of people having this remaining days left to become a Zombie
USTRUCT()
struct FConveyorBatch
{
	GENERATED_BODY()

	float amountOfPeople;      // people currently in conveyor (bitten)
	float remainingDays;       // days left before conversion
};


UCLASS()
class PROJECTZOMBEE_API AZombieManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZombieManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ----- TIME SPECS -----
	const int STARTTIME{ 0 };        // Simulation StartTime
	const int STOPTIME{ 120 };      // By this time all simulations should be finished
	const float DT{ 1.f };      // DeltaTime for the simulation, 1 == 1 day steps
	int32 day = 0; // Starting day of the simulation

	// ----- CONSTANTS / INITIALIZATION -----
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Zombie Model Values")
	float days_to_become_infected_from_bite{ 15.f };   // Days
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Zombie Model Values")
	float Bitten_capacity{ 100.f };                    // Capacity inside conveyor == people at start
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Zombie Model Values")
	float patient_zero{ 1.0f };                        // Zombies
	const float CONVERSION_FROM_PEOPLE_TO_ZOMBIES{ 1.f };    // Conversion constant, always 1
	const float normal_number_of_bites{ 1.f };               // People/zombie/day
	const float land_area{ 1000.f };                         // In m2
	const float normal_population_density{ 0.1f };           // people/m2

	// Stocks (initial)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Zombie Model Variables")
	float Susceptible{ 100.f };        // People
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Zombie Model Variables")
	float Bitten{ 0.f };
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Zombie Model Variables")
	float Zombies = patient_zero;     // Zombies == 1.f at start

	// Conveyor content is the Bitten people incubating
	// Will have one ConveyorBatch of each remainingDays
	// So max days_to_become_infected_from_bite different ConveyorBatch'es
	TArray<FConveyorBatch> conveyor;            // accepts multiple batches

	// GRAPH points: population_density_effect_on_zombie_bites
	// x = population_density / normal_population_density
	//UPROPERTY()
	TArray<FVector2d> graphPts = {
		{0.000f, 0.014f}, {0.200f, 0.041f}, {0.400f, 0.101f}, {0.600f, 0.189f}, {0.800f, 0.433f},
		{1.000f, 1.000f}, {1.200f, 1.217f}, {1.400f, 1.282f}, {1.600f, 1.300f}, {1.800f, 1.300f},
		{2.000f, 1.300f}
	};

	// Forward declarations -> functions after int main()
	float graph_lookup(float xIn);
	float conveyor_content();

	UFUNCTION(BlueprintCallable)
	void ZombieModel(int& bittenToday);
	//void write_row(int t, std::ofstream& csv);
};

