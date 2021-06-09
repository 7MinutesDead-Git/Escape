// Copyright 2021, Alex Gulikers, 7 Minutes Dead.
// Although these are mostly notes for reference to myself, so you are welcome to pick it apart!

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "MagneticBalls.generated.h"


/// Contains AActor pointer to Ball, and it's distance in float.
USTRUCT()
struct FBallDistances
{
    GENERATED_BODY()
    UPROPERTY() AActor* Ball;
    float Distance;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ESCAPE_API UMagneticBalls : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties.
    UMagneticBalls();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /// Object grabbed by player via ItemGrabber component.
    UPROPERTY() UPrimitiveComponent* GrabbedObject = nullptr;

protected:
    // Called when the game starts.
    virtual void BeginPlay() override;

private:
    /// Our Physics Handle Component added to the actor our MagneticBalls component goes on.
    UPROPERTY() UPhysicsHandleComponent* BallPhysicsHandle;
    UPROPERTY() TArray<AActor*> BallsInLevel;
    UPROPERTY() TArray<FBallDistances> BallsAndDistances;
    /// Reference to player pawn (DefaultPawnBP) that will have relevant components attached.
    UPROPERTY() APawn* PlayerPawn;
    /// Reference to player controller's physics handle.
    UPROPERTY() UPhysicsHandleComponent* PlayerPhysicsHandle;

    UPROPERTY(EditAnywhere) float UpdateRate = 0.3f;
    UPROPERTY(EditAnywhere) float FollowSpeed = 5;
    UPROPERTY(EditAnywhere) float MaxFollowDistance = 5;
    UPROPERTY(EditAnywhere) bool EnableDebugView = false;
    UPROPERTY(EditAnywhere) FColor DebugLineColor;
    float Distance;
    float Elapsed = 0;
    FVector Destination;
    FVector CurrentPosition;
    FHitResult Bump;
    UPROPERTY() TArray<AActor*> ClosestBalls;


    // Functions.
    void SetDestination();
    bool PlayerHoldingItem();
    /// Get physics handle from owner.
    void GetPhysicsHandle();
    /// Find the closest AActor* Ball in a list of FBallDistances pairs and return that AActor*.
    TArray<AActor*> FindClosestBalls(TArray<FBallDistances>);
    /// Get array of AActor* balls in the level and return it.
    TArray<AActor*> GetAllMagneticBalls();
    /// Calculate distance from owner to each ball, and store reference to ball and the distance. \n
    /// Return new array of Balls and Distances.
    TArray<FBallDistances> GetBallDistancePairs();
};
