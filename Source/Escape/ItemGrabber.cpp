// Copyright 2021, Alex Gulikers, 7 Minutes Dead.
// Although these are mostly notes for reference to myself, so you are welcome to pick it apart!

#include "ItemGrabber.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
// To give myself a heads up that something is an out parameter.
#define OUT


// ----------------------------------------------------------------------
/// Sets default values for this component's properties.
UItemGrabber::UItemGrabber()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
    // ...
}


// ----------------------------------------------------------------------
/// Called when the game starts.
void UItemGrabber::BeginPlay()
{
    Super::BeginPlay();
    NotifyLoading();
    Player = GetWorld()->GetFirstPlayerController();
    GetPhysicsHandle();
    GetPlayerInput();
    BindActionsToKeys();

    HoldDistanceDefault = HoldDistance;
    // ...
}


// ----------------------------------------------------------------------
/// Called every frame.
void UItemGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // If we're holding an object, then update hold point and move it towards that point.
    if (PhysicsHandle->GrabbedComponent) {
        MovePhysicsHandle();
    }

    if (EnableDebugLines) {
        DebugViewInfo();
    }
}


// ----------------------------------------------------------------------
/// Get player's viewpoint and set it to our PlayerView data structure.
void UItemGrabber::GetPlayerView()
{
    Player->GetPlayerViewPoint(
        OUT PlayerView.Location,
        OUT PlayerView.Rotation
    );
}


// ----------------------------------------------------------------------
/// Get player view. Return point in front of player based on HoldDistance.
FVector UItemGrabber::GetHoldPoint()
{
    GetPlayerView();
    return PlayerView.Location + PlayerView.Rotation.Vector() * HoldDistance;
}


// ----------------------------------------------------------------------
/// Get player view. Return point in front of player based on GrabReach.
FVector UItemGrabber::GetGrabReachEnd()
{
    GetPlayerView();
    return PlayerView.Location + PlayerView.Rotation.Vector() * GrabReach;
}


// ----------------------------------------------------------------------
/// Do a line trace and see what object we can grab. Update GrabbableHit via OUT.
void UItemGrabber::GetGrabbableObject()
{
    GetPlayerView();
    GrabReachEnd = GetGrabReachEnd();
    HoldPoint = GetHoldPoint();

    // Define what should be considered a collision for what can be grabbed.
    const FCollisionQueryParams TraceParameters(
        FName(TEXT("")),  // The tag name.
        true,             // Use complex collisions for grab attempt?
        GetOwner()        // Which objects to ignore (make sure the ray hitting ourselves doesn't count).
    );

    // LineTrace (raycast) from player to LineTraceEnd and see what we hit.
    GetWorld()->LineTraceSingleByObjectType(
        OUT GrabbableHit,
        PlayerView.Location,
        GrabReachEnd,
        FCollisionObjectQueryParams(ECC_PhysicsBody),
        TraceParameters
    );

    ComponentToGrab = GrabbableHit.GetComponent();
}


// ----------------------------------------------------------------------
/// Get the PhysicsHandle component that should be added to the Owner pawn.\n
/// Log error if Physics Handle Component is not attached to the Owner pawn.
void UItemGrabber::GetPhysicsHandle()
{
    // UItemGrabber is also attached to DefaultPawnBP, so GetOwner will work for this too.
    // Since FindComponentByClass is a function template, we construct by <type>.
    // This will return the first UPhysicsHandleComponent found in the Owner (DefaultPawnBP).
    PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

    if (!PhysicsHandle) {
        UE_LOG(LogTemp, Error,
               TEXT("%s has no Physics Handle Component (needed by ItemGrabber)!"),
               *GetOwner()->GetName()
        );
    }
}


// ---------------------------------------------------------------------
/// Called every frame. \n
/// Move Physics Handle of grabbed object smoothly between object's current vector, and hold point vector. \n\n
void UItemGrabber::MovePhysicsHandle()
{
    HoldPoint = GetHoldPoint();
    PhysicsHandle->SetTargetLocation(HoldPoint);
}


// ----------------------------------------------------------------------
/// Get the InputComponent on the owner (player pawn) of this component (ItemGrabber).
void UItemGrabber::GetPlayerInput()
{
    PlayerInput = GetOwner()->FindComponentByClass<UInputComponent>();

    if (!PlayerInput) {
        UE_LOG(LogTemp, Error,
               TEXT("%s has no Input Component (needed by ItemGrabber)!"),
               *GetOwner()->GetName()
        );
    }
}


// ----------------------------------------------------------------------
/// Bind each relevant function to each of our player inputs.
void UItemGrabber::BindActionsToKeys()
{
    // (1) Action/input name, (2) KeyEvent type, (3) Object, (4) Point to &address of function.
    PlayerInput->BindAction("Grab", IE_Pressed, this, &UItemGrabber::GrabToggle);
    PlayerInput->BindAction("Throw", IE_Pressed, this, &UItemGrabber::Throw);
    PlayerInput->BindAction("Push", IE_Pressed, this, &UItemGrabber::Push);
    PlayerInput->BindAction("Pull", IE_Pressed, this, &UItemGrabber::Pull);
}


// --------------------------------------------------------------------
/// Grab (or drop) object in view.
void UItemGrabber::GrabToggle()
{
    if (!HoldingItem) {
        GetGrabbableObject();
        AActor* GrabbableActor = GrabbableHit.GetActor();
        // If GrabbableHit has been hit by our line trace in GetGrabbableObject..
        if (GrabbableActor) {
            // Then Grab whatever component is there at GrabReachEnd.
            UE_LOG(LogTemp, Log, TEXT("Grabbed item."));
            PhysicsHandle->GrabComponentAtLocation(ComponentToGrab, NAME_None, GrabbableHit.Location);
            HoldingItem = true;
        }
    }
    else if (HoldingItem) {
        PhysicsHandle->ReleaseComponent();
        UE_LOG(LogTemp, Log, TEXT("Dropped item."));
        // Reset any hold distance adjustments we made with push/pull.
        HoldDistance = HoldDistanceDefault;
        HoldingItem = false;
    }
}

// --------------------------------------------------------------------
/// Throw item if holding one!
void UItemGrabber::Throw()
{
    if (HoldingItem) {
        UE_LOG(LogTemp, Log, TEXT("Dropped item."));
        // TODO: Add physics impulse to throw object away from player.
        PhysicsHandle->ReleaseComponent();
        // Reset any hold distance adjustments we made with push/pull.
        HoldDistance = HoldDistanceDefault;
        HoldingItem = false;
    }
}

// --------------------------------------------------------------------
/// Push the currently held object farther away.
void UItemGrabber::Push()
{
    if (HoldingItem) {
        UE_LOG(LogTemp, Log, TEXT("Pushing object!"));
        HoldDistance += PushPullStepSize;
    }
}

// --------------------------------------------------------------------
/// Pull the currently held object closer.
void UItemGrabber::Pull()
{
    if (HoldingItem) {
        UE_LOG(LogTemp, Log, TEXT("Pulling object!"));
        HoldDistance -= PushPullStepSize;
    }
}

// --------------------------------------------------------------------
// Debug helper functions.
// --------------------------------------------------------------------
/// Draw debug line for grabby hands, log grabbable object hit.
void UItemGrabber::DebugViewInfo()
{
    FColor Color;

    DrawDebugLine(
        GetWorld(),              // InWorld.
        PlayerView.Location,     // LineStart.
        GrabReachEnd,            // LineEnd.
        Color.FromHex("00FF15"), // Color.
        false,                   // PersistentLines.
        1,                       // LifeTime.
        0,                       // DepthPriority
        2                        // Thickness.
    );

    UE_LOG(LogTemp, Log,
           TEXT("Physics target location: %s."),
           *PhysicsHandle->TargetTransform.GetLocation().ToCompactString()
    );

    UE_LOG(LogTemp, Log,
           TEXT("HoldPoint: %s"),
           *HoldPoint.ToCompactString()
    );
}

// --------------------------------------------------------------------
/// Notify UItemGrabber component was loaded successfully.
void UItemGrabber::NotifyLoading()
{
    UE_LOG(LogTemp, Warning,
           TEXT("UItemGrabber loaded successfully on %s."),
           *GetOwner()->GetName()
    );
}
