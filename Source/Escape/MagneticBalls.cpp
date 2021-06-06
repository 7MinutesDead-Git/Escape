// Copyright 2021, Alex Gulikers, 7 Minutes Dead.
// Although these are mostly notes for reference to myself, so you are welcome to pick it apart!
#include "MagneticBalls.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"


// Sets default values for this component's properties.
UMagneticBalls::UMagneticBalls()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
    // ...
}


// Called when the game starts.
void UMagneticBalls::BeginPlay()
{
    Super::BeginPlay();

    GetPhysicsHandle();
    // Warning if destination object isn't attached via editor.
    if (!DestinationObject) {
        UE_LOG(LogTemp, Error, TEXT("'%s' is missing a destination object!"), *GetOwner()->GetName());
    }
    // ...
}


// Called every frame.
void UMagneticBalls::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (DestinationObject) {
        Elapsed += DeltaTime;

        if (Elapsed > FollowUpdateRate) {
            FindAndMoveToTarget();
            Elapsed = 0;
        }

        BallPhysicsHandle->SetTargetLocation(Destination);
    }
    // ...
}


/// Get physics handle component for this component's owner.
void UMagneticBalls::GetPhysicsHandle()
{
    BallPhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

    if (!BallPhysicsHandle) {
        UE_LOG(LogTemp, Error,
               TEXT("%s has no Physics Handle Component! Needed by MagneticBalls component."),
               *GetOwner()->GetName()
        );
    }
}


/// Get owner's location, get target's location, calculate distance, then set target location for physics handle.
void UMagneticBalls::FindAndMoveToTarget()
{
    // This object's current position.
    CurrentPosition = GetOwner()->GetTransform().GetLocation();
    // Our destination actor based on selection in editor.
    Destination = DestinationObject->GetTransform().GetLocation();
    // How far away this object is from the destination actor.
    Distance = GetOwner()->GetDistanceTo(DestinationObject);
    // TODO: This doesn't seem to do anything.
    BallPhysicsHandle->SetTargetLocation(Destination);

}
