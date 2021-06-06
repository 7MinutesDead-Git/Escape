// Copyright 2021, Alex Gulikers, 7 Minutes Dead.
// Although these are mostly notes for reference to myself, so you are welcome to pick it apart!

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "MagneticBalls.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ESCAPE_API UMagneticBalls : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties.
    UMagneticBalls();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    // Called when the game starts.
    virtual void BeginPlay() override;

private:
    /// Our Physics Handle Component added to the actor our MagneticBalls component goes on.
    UPROPERTY()
    UPhysicsHandleComponent* BallPhysicsHandle;
    /// The object we want our magnetic ball to move towards.
    UPROPERTY(EditAnywhere)
    AActor* DestinationObject;

    UPROPERTY(EditAnywhere)
    float FollowUpdateRate = 0.3f;
    float Distance;
    float Elapsed = 0;
    FVector Destination;
    FVector CurrentPosition;

    // Functions.
    void GetPhysicsHandle();
    void FindAndMoveToTarget();
};
