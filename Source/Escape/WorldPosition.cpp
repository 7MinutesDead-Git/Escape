// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldPosition.h"
#include "GameFramework/Actor.h"

/// Sets default values for this component's properties.
UWorldPosition::UWorldPosition()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	// You can turn these features off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


/// Called when the game starts.
void UWorldPosition::BeginPlay()
{
	Super::BeginPlay();

	// Gets the parent object's name that this script/component is attached to.
	FString CubeName = GetOwner()->GetName();
	// Get the location (vector) of the root component of this actor (if this actor is attached to anything),
	// and convert to compact string for easy reading.
	FString CubeLocation = GetOwner()->GetActorLocation().ToCompactString();
	UE_LOG(LogTemp, Warning, TEXT("Component Name: %s located at %s."), *CubeName, *CubeLocation);

}


/// Called every frame.
void UWorldPosition::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
