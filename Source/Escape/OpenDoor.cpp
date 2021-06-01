// Alex Gulikers, 7 Minutes Dead, Copyright 2021


#include "OpenDoor.h"
#include "BehaviorTree/BehaviorTreeTypes.h"

// Sets default values for this component's properties.
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

// Called when the game starts.
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	// ...

}


// Called every frame.
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	const FRotator DoorRotation = Owner->GetActorRotation();
	// FInterpTo gives us a framerate-independent Lerp, by allowing us to pass in DeltaTime too.
	SmoothedTargetRotation.Yaw = FMath::FInterpTo(DoorRotation.Yaw, TargetRotation.Yaw, DeltaTime, OpenRate);
	Owner->SetActorRotation(SmoothedTargetRotation);
	// ...
}
