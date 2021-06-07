// Copyright 2021, Alex Gulikers, 7 Minutes Dead.
// Although these are mostly notes for reference to myself, so you are welcome to pick it apart!

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "ItemGrabber.generated.h"


// -----------------------------------------------------------------------
/// The location/position and rotation of the player view.
struct FPlayerView
{
    FVector Location;
    FRotator Rotation;
};


// -----------------------------------------------------------------------
/// The location/position and rotation of the held object (Physics Handle Component).
struct FGrabbedObject
{
    FVector Location;
    FRotator Rotation;
};


// -----------------------------------------------------------------------
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ESCAPE_API UItemGrabber : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties.
    UItemGrabber();
    // Called every frame.
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

private:
    // Variables. ---------------------------------------------------------------------
    FPlayerView PlayerView;
    FGrabbedObject GrabbedObject;
    FHitResult GrabbableHit;
    FVector GrabReachEnd;
    FVector HoldPoint;

    bool HoldingItem = false;
    float DoubleClickSafetyTimer;

    /// How long your psychic arm is.
    UPROPERTY(EditAnywhere)
    float GrabReach = 800;

    /// How far in front of the player an object is held once grabbed.
    UPROPERTY(EditAnywhere)
    float HoldDistance = 180;
    float HoldDistanceDefault;

    /// Step size for held object push and pull distance.
    UPROPERTY(EditAnywhere)
    float PushPullStepSize = 20;

    /// Strength of forward impulse when throwing object rather than dropping.
    UPROPERTY(EditAnywhere)
    float ThrowStrength = 1000;

    UPROPERTY(EditAnywhere)
    bool EnableDebugLines = false;

    // Pointers. ----------------------------------------------------------------------
    UPROPERTY(EditAnywhere)
    APlayerController* Player;

    /// Our Physics Handle Component added to the player pawn blueprint.
    UPROPERTY()
    UPhysicsHandleComponent* PhysicsHandle;

    /// The Component we want to grab.
    UPROPERTY()
    UPrimitiveComponent* ComponentToGrab;

    /// Our input.
    UPROPERTY()
    UInputComponent* PlayerInput;

    // Functions. ----------------------------------------------------------------------
    void GetPlayerView();
    FVector GetHoldPoint();
    FVector GetGrabReachEnd();
    void GetGrabbableObject();

    void GetPhysicsHandle();
    void MovePhysicsHandle();

    void GetPlayerInput();
    void BindActionsToKeys();

    void GrabToggle();
    void Throw();
    void Push();
    void Pull();

    // Debug helper functions. ---------------------------------------------------------
    void DebugViewInfo();
    void NotifyLoading();
};
