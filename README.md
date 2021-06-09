Well, here's the result so far hahah:  
https://www.youtube.com/watch?v=ybqikdJpko0  
  
I was making balls to use as the mass triggers to open the door to escape.  
  
https://www.youtube.com/watch?v=UTkpKVV3H1c  
  
Having these metal balls floating around gave me an idea to try to make them magnetic, and not just try to move towards the closest ball, but move *in between* the two closest balls and form whatever shape may form from that. The effects are still a bit strange but I think I'm getting closer.

So to start, I wanted to be able to put a MagneticBall component on a ball, and have it interact with all other MagneticBall actors.

First thing we gotta do is find all the metal balls in the level, if we want to be able to interact with potentially all of them from the perspective of a single ball.

What I’ve found is [TObjectIterator and TActorIterator](https://ue4community.wiki/legacy/iterators:-object-and-actor-iterators-optional-class-scope-for-faster-search-l51dnrnd) (either one can work, but TActorIterator is less prone to weirdness if you can use it apparently) for iterating through actors in the scene to find all the magnetic ball instances. The best way to identify them cleanly I’m still trying to figure out, but I’ve got it working. It returns a TArray of AActor* pointers to our floating balls.


```cpp
// -----------------------------------------------------------------------------
/// Retrieve all actors whose name starts with "MagneticBall". Return TArray.
/// \n Not very fool proof, but it works for now.
TArray<AActor*> UMagneticBalls::GetAllMagneticBalls()
{
    const uint32 ThisObjectID = GetOwner()->GetUniqueID();
    TArray<AActor*> Balls;

    // Find all objects with UMagneticBalls component. Add them to our array.
    for (TActorIterator<AActor> Actor(GetWorld()); Actor; ++Actor) {
        // Ensure we're at the highest AActor level so we don't get unexpected crashes later.
        if (Actor->GetName().StartsWith("MagneticBall") && Actor->GetOwner() == nullptr) {
            // Skip itself to avoid marking itself as closest.
            if (Actor->GetUniqueID() == ThisObjectID) {
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
```

Next, we need to get the distance to all of the other balls from **this** ball, so that we can figure out which one(s) are the closest. I needed some way of pairing up each AActor* pointer to each ball, along with its distance. I'm new to C++ so I don't know how to best create a dictionary or hashmap, so I made a UStruct called FBallDistances:

```cpp

/// Contains AActor pointer to Ball, and it's distance in float.
USTRUCT()
struct FBallDistances
{
    GENERATED_BODY()
    UPROPERTY() AActor* Ball;
    float Distance;
};
```

And then checked all the distances in our Balls array and make a new array of these FBallDistances results:


```cpp
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
```

Next, we want to figure out which *two* balls in that unsorted array of balls/distances are the closest to **this** ball. Something like this for me is where practicing at least Easy problems on Leetcode can come in handy, just to get the practice of iterating in haha.

```cpp
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
            // We'll move the first ball checked directly into Closest.
            Closest.Ball = Ball.Ball;
            Closest.Distance = Ball.Distance;
            FirstCheck = false;
        }
        // Then for each iteration after, we push the closest ball to the top.
        else if (Ball.Distance < Closest.Distance) {
            // Push previous closest down to 2nd.
            SecondClosest.Ball = Closest.Ball;
            // Push newest up to closest.
            Closest.Ball = Ball.Ball;
            Closest.Distance = Ball.Distance;
        }
    }

    Results.Push(Closest.Ball);
    Results.Push(SecondClosest.Ball);

    return Results;

}
```

Unfortunately, something above is not reliable in that in some frames I'm getting null or weird results for the SecondClosest.Ball, which means I can't always determine a position based on two balls (any help or ideas would be great there) since functions like GetActorLocation() will crash on these incorrect pointers that *sometimes* show up.. I've been trying to troubleshoot this all day long. I think some data is lost somewhere when moving between variables above. No problem, I can come back to that later. For now let's do just the closest ball.

Next is to determine the destination. Now this I'm still trying to figure out as my knowledge of math with Vectors is about nil, but I *think* this is along the right lines.


```cpp
// -----------------------------------------------------------------------------
/// Set destination based on two closest balls.
void UMagneticBalls::SetDestination()
{
    BallsAndDistances = GetBallDistancePairs();
    ClosestBalls = FindClosestBalls(BallsAndDistances);
    // TODO: Figure out problems with sometimes missing ClosestBalls[1].

    // Single Ball Solution:
    UStaticMeshComponent* ClosestBallMesh = ClosestBalls[0]->FindComponentByClass<UStaticMeshComponent>();
    FVector ClosestBallLocation = ClosestBalls[0]->GetActorLocation();
    BallPhysicsHandle->GrabComponentAtLocation(ClosestBallMesh, NAME_None, ClosestBallLocation);
    // Current vector - target vector to get the direction, divided by 2 to get the midpoint.
    BallPhysicsHandle->SetTargetLocation(CurrentPosition - ClosestBallLocation / 2);
}
```

Now we can finally get to work in the tick component.


```cpp
// Called every frame.
void UMagneticBalls::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // All of this is to ensure we don't do access violations on null pointers,
    // like when the player isn't holding anything from ItemGrabber component.
    if (PlayerHoldingItem()) {
        FString GrabbedObjectName = PlayerPhysicsHandle->GrabbedComponent->GetOwner()->GetName();
        // If we're grabbing this item as a player..
        if (GrabbedObjectName == GetOwner()->GetName()) {
            // Then release it's hold on fellow balls. (not quite working yet)
            BallPhysicsHandle->ReleaseComponent();
        }
    }

    CurrentPosition = GetOwner()->GetActorLocation();
    SetDestination();

    // Debug view of travel lines.
    if (EnableDebugView) {
        FRotator TargetRotation;
        BallPhysicsHandle->GetTargetLocationAndRotation(OUT Destination, OUT TargetRotation);
        DrawDebugLine(
            GetWorld(),              // InWorld.
            CurrentPosition,         // LineStart.
            Destination,             // LineEnd.
            DebugLineColor,          // Color.
            false,                   // PersistentLines.
            0.1,                     // LifeTime.
            0,                       // DepthPriority
            1                        // Thickness.
        );
    }
}
```

And the result is in that first video haha. If you read all of this you can see I've got things not working how I want, and missing some other intended mechanics, but the result is really interesting so far regardless so I had to share it.
