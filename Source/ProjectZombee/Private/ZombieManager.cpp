// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieManager.h"

// Sets default values
AZombieManager::AZombieManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AZombieManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AZombieManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    
}



float AZombieManager::graph_lookup(float xIn)
{
    // Piecewise linear interpolation with endpoint clamping
    
    // Special cases, xIn outside table range:
    

    if (xIn <= graphPts[0].X)
        return graphPts[0].Y;
    if (xIn >= graphPts.Last().X)
        return graphPts.Last().Y;

    for (size_t i = 1; i < graphPts.Num(); ++i)
    {
        if (xIn <= graphPts[i].X)
        {
            // Linear interpolation between points in graph:
            float x0 = graphPts[i - 1].X, x1 = graphPts[i].X;     //the x value below and above xIn
            float y0 = graphPts[i - 1].Y, y1 = graphPts[i].Y;   //the y value below and above xIn
            float t = (xIn - x0) / (x1 - x0);   //distance between x0 and x1
            return y0 + t * (y1 - y0);            //return the interpolated y value
        }
    }
    return graphPts.Last().Y; // fallback
}

// Sums up all bitten
float AZombieManager::conveyor_content()
{
    float sum = 0.0;
    for (FConveyorBatch& b : conveyor)
        sum += b.amountOfPeople;
    return sum;
}

void AZombieManager::ZombieModel(int& bittenToday)
{
    if (Bitten <= 0 && Susceptible <= 0)
        return;

    // Used for displaying the day
    day++;


    //std::cout << "\n***** Day: " << t << " *****\n";
    // --- Calculate auxiliaries (using values at current time t) ---
    Bitten = conveyor_content(); // current conveyor content (people)
    float non_zombie_population = Bitten + Susceptible; // People
    float population_density = non_zombie_population / land_area; // people/m2

    float x = population_density / normal_population_density; // dimensionless
    // std::cout << "population_density / normal_population_density: " << x << "\n";

    float population_density_effect_on_zombie_bites = graph_lookup(x);
    // std::cout << "population_density_effect_on_zombie_bites: " << population_density_effect_on_zombie_bites << "\n";

    float number_of_bites_per_zombie_per_day =
        normal_number_of_bites * population_density_effect_on_zombie_bites; // People/zombie/day

    // ROUND as in Stella (nearest int, half away from zero) -> std::round
    float total_bitten_per_day =
        round(Zombies * number_of_bites_per_zombie_per_day); // People/day - FMath::Floor 

    // Avoid division by zero by MAX(non_zombie_population, 1)
    float denom = std::max(non_zombie_population, 1.f);
    float number_of_bites_from_total_zombies_on_susceptible =
        round((Susceptible / denom) * total_bitten_per_day); // People/day

    // This is the actual number of bites all zombies should do this time step
    // So THIS is the number to use inside of Unreal
    //std::cout << "Number of Susceptible to bite this timestep: " << number_of_bites_from_total_zombies_on_susceptible << "\n";

    // getting_bitten UNIFLOW, but enforce NON-NEGATIVE Susceptible
    float getting_bitten = number_of_bites_from_total_zombies_on_susceptible;
    // Truncate to not exceed available Susceptible this step (Euler non-negative safeguard)
    getting_bitten = FMath::Min(getting_bitten, floor(Susceptible)); // ensure we don't drive below 0
    bittenToday = getting_bitten;

    // --- Conveyor mechanics for Bitten ---
    // 1) Progress existing batches and compute raw outflow (people exiting conveyor)
    for (FConveyorBatch& b : conveyor)
    {
        b.remainingDays -= DT;  // update remainingDays
        // std::cout << "ConveyorBatch amount of people:" << b.amountOfPeople << " remaining days: "
                  // << b.remainingDays << "\n";
    }
    // collect finished batches -> remainingDays == 0
    TArray<FConveyorBatch> next_conveyor;
    float raw_outflow_people{ 0.f };
    next_conveyor.Reserve(conveyor.Num()); // Allocates memory??
    for (FConveyorBatch& b : conveyor)
    {
        if (b.remainingDays <= 0.0)
            raw_outflow_people += b.amountOfPeople;
        else
            next_conveyor.Add(b);

    }
    conveyor = next_conveyor; // TODO: There may be problems with this

    // 2) Capacity check for new inflow (people)
    float current_content = conveyor_content();
    float free_cap = std::max(0.f, Bitten_capacity - current_content);
    float inflow_people = std::max(0.f, std::min(getting_bitten, free_cap));

    if (inflow_people > 0.f)
        conveyor.Add(FConveyorBatch{ inflow_people, days_to_become_infected_from_bite });

    // 3) Conveyor outflow converted to zombies
    // becoming_infected == becoming Zombies this timestep
    float becoming_infected = raw_outflow_people * CONVERSION_FROM_PEOPLE_TO_ZOMBIES; // Zombies/day

    // --- STOCK UPDATES (Euler) ---
    // Susceptible(t+1) = S(t) - getting_bitten
    Susceptible = std::max(0.f, Susceptible - getting_bitten * DT);

    // Zombies(t+1) = Z(t) + becoming_infecting
    Zombies = std::max(0.f, Zombies + becoming_infected * DT);

    // Bitten stock is already handled implicitly by conveyor vector
    // (content increased by inflow_people, decreased by raw_outflow_people)
    Bitten = conveyor_content();

    // --- (Optional) derived integer populations, not used further ---
    // float Susceptible_Population = floor(Susceptible);
    // float Zombie_Population = floor(Zombies);

    //std::cout << "Susceptible: " << Susceptible << ", Bitten: " << Bitten << ", Zombies: " << Zombies << "\n";

    // Write row for t+DT
    //write_row(t + (int)DT, csvFile);

    //FString msg =  ;

    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Susceptible: %f, Bitten: %f, Zombies: %f, On day: %d"), Susceptible, Bitten, Zombies, day));
}

/*
// Not needed in Unreal
void write_row(int t, std::ofstream& csvIn)
{
    float tempBitten = conveyor_content();
    csvIn << t << ","
        << std::fixed << std::setprecision(6)
        << Susceptible << ","
        << tempBitten << ","
        << Zombies << "\n";
}
*/
