// Copyright 2021, Alex Gulikers, 7 Minutes Dead.
// Although these are mostly notes for reference to myself, so you are welcome to pick it apart!

// https://stackoverflow.com/questions/12934213/how-to-find-out-geometric-median
/* How to find the middle point between two vectors.
 *  (End - Start) = The direction.
 *  (End - Start) / 2 = In between the two vectors.
*/

#include "MagneticBalls.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
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

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    PlayerPhysicsHandle = PlayerPawn->FindComponentByClass<UPhysicsHandleComponent>();

    if (!PlayerPhysicsHandle) {
        UE_LOG(LogTemp, Error, TEXT("Player controller/pawn needs a PhysicsHandle Component! Add in editor."));
    }

    GetPhysicsHandle();
    BallsInLevel = GetAllMagneticBalls();

    // Temporary initial setup before frame.
    Destination = SetDestination();
    CurrentPosition = GetOwner()->GetActorLocation();
}


// Called every frame.
void UMagneticBalls::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    Elapsed += DeltaTime;

    CurrentPosition = GetOwner()->GetActorLocation();

    // Probably shouldn't traverse our array of balls every single frame.
    if (Elapsed > UpdateRate) {
        Elapsed = 0;
        CurrentPosition = GetOwner()->GetActorLocation();
        Destination = SetDestination();
    }

    FVector Smoothed = FMath::VInterpTo(CurrentPosition, Destination, DeltaTime, FollowSpeed);

    // All of this is to ensure we don't do access violations on null pointers, like when
    // the player isn't holding anything.
    if (PlayerHoldingItem()) {
        if (PlayerPhysicsHandle->GrabbedComponent->GetOwner()->GetName() != GetOwner()->GetName()) {
            // If the player isn't holding THIS object, continue to move.
            GetOwner()->K2_SetActorLocation(Smoothed, true, OUT Bump, false);
        }
    }
    else {
        // If the player isn't grabbing any object, continue to move.
        GetOwner()->K2_SetActorLocation(Smoothed, true, OUT Bump, false);
    }

    if (EnableDebugView) {
        FColor Color;
        DrawDebugLine(
            GetWorld(),              // InWorld.
            CurrentPosition,         // LineStart.
            Destination,             // LineEnd.
            Color.FromHex("00FF15"), // Color.
            false,                   // PersistentLines.
            1,                       // LifeTime.
            0,                       // DepthPriority
            2                        // Thickness.
        );

    }
}

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
/// Find the two closest balls to this actor. Return result.
TArray<AActor*> UMagneticBalls::FindClosestBalls(TArray<FBallDistances> ListOfBalls)
{
    FBallDistances Closest;
    FBallDistances SecondClosest;
    TArray<AActor*> Results;
    bool FirstCheck = true;

    for (FBallDistances Ball: ListOfBalls) {
        if (FirstCheck) {
            Closest.Ball = Ball.Ball;
            Closest.Distance = Ball.Distance;
            FirstCheck = false;
        }
        else if (Ball.Distance < Closest.Distance) {
            // Push 1st down to 2nd.
            SecondClosest.Ball = Closest.Ball;
            // Push newest up to 1st.
            Closest.Ball = Ball.Ball;
            Closest.Distance = Ball.Distance;
        }
    }

    if (!Closest.Ball) {
        UE_LOG(LogTemp, Warning, TEXT("Closest.Ball is null!!"))
    }
    if (!SecondClosest.Ball) {
        UE_LOG(LogTemp, Warning, TEXT("SecondClosest.Ball is null!!"))
    }

    // Order matters I think? We'll be calculating the midpoint vector between the two.
    Results.Add(Closest.Ball);
    Results.Add(SecondClosest.Ball);

    return Results;

}

// -----------------------------------------------------------------------------
/// Retrieve all actors whose name starts with "MagneticBall". Return TArray.
/// \n Not very fool proof, but it works for now.
/// \n\n TODO: Replace current method with checking for component or tag instead.
TArray<AActor*> UMagneticBalls::GetAllMagneticBalls()
{
    FString ThisObjectName = *GetOwner()->GetName();
    TArray<AActor*> Balls;

    // Find all objects with UMagneticBalls component. Add them to our array.
    for (TActorIterator<AActor> Actor(GetWorld()); Actor; ++Actor) {
        // TODO: StaticMeshes are sliding into this list causing crashes. Need to fix.
        // TODO: Try tags maybe?
        if (Actor->GetName().StartsWith("MagneticBall")) {

            // Skip itself to avoid marking itself as closest.
            if (Actor->GetName() == ThisObjectName) {
                continue;
            }
            // Make sure we add the pointer to the list, not the actual object.
            Balls.Add(*Actor);
        }
    }

    if (Balls.Num() == 0) {
        UE_LOG(LogTemp, Error, TEXT("Unable to find any balls. Did you name them MagneticBall?"));
    }

    return Balls;
}

// -----------------------------------------------------------------------------
/// Calculate distances to all balls in level, and pair those distances up with each ball Actor pointer. \n\n
/// Return resulting TArray of FBallDistances (custom UStruct in header).
TArray<FBallDistances> UMagneticBalls::GetBallDistancePairs()
{
    // Make Array where we can store each ball pointer and its distance.
    TArray<FBallDistances> Dict;

    for (AActor* Ball : BallsInLevel) {
        if (Ball) {
            const float DistanceToBall = GetOwner()->GetDistanceTo(Ball);

            FBallDistances ThisBall;
            ThisBall.Ball = Ball;
            ThisBall.Distance = DistanceToBall;

            Dict.Add(ThisBall);
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Encountered null pointer. Expected pointer to MagneticBall actor."));
        }
    }

    return Dict;
}

// -----------------------------------------------------------------------------
/// Set destination based on two closest balls.
FVector UMagneticBalls::SetDestination()
{
    BallsAndDistances = GetBallDistancePairs();
    ClosestBalls = FindClosestBalls(BallsAndDistances);
    AActor* BallOne = ClosestBalls[0];
    AActor* BallTwo = ClosestBalls[1];

    if (!BallOne) {
        UE_LOG(LogTemp, Error, TEXT("BallOne is null!"));
        return FVector(0, 0, 0);
    }

    if (!BallTwo) {
        UE_LOG(LogTemp, Error, TEXT("BallTwo is null!"));
        return FVector(0, 0, 0);
    }

    // TODO: For some reason, UStaticMesh* is getting into my AActor* arrays on some frames.
    // GetActorLocation() isn't for UStaticMesh so we crash here.
    FVector Result = (BallTwo->GetActorLocation() - BallOne->GetActorLocation()) / 2;
    return Result;
}

// -----------------------------------------------------------------------------
/// If the player is currently grabbing a component, then it's safe to interact with said component.
/// \n\n This is to avoid null pointer crashes when the player is not holding an object.
bool UMagneticBalls::PlayerHoldingItem()
{
    if (PlayerPhysicsHandle->GrabbedComponent) {
        return true;
    }
    return false;
}
