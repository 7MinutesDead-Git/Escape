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
    FHitResult GrabbableHit;
    FVector GrabReachEnd;
    FVector HoldPoint;

    bool HoldingItem = false;
    float DoubleClickSafetyTimer;

    // TODO: Find out why this is:
    /// If GrabReach is larger than HoldDistance, grabbed doesn't follow very well.
    UPROPERTY(EditAnywhere)
    float GrabReach = 180;

    /// If GrabReach is larger than HoldDistance, grabbed doesn't follow very well.
    UPROPERTY(EditAnywhere)
    float HoldDistance = 180;

    UPROPERTY(EditAnywhere)
    float ThrowStrength = 1000.f;

    UPROPERTY(EditAnywhere)
    bool EnableDebugLines = false;

    // Pointers. ----------------------------------------------------------------------
    UPROPERTY(EditAnywhere)
    APlayerController* Player;

    /// Our Physics Handle Component added via Blueprint to the player pawn.
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
    void GetPlayerInput();
    void BindActionsToKeys();
    void GrabToggle();
    void Throw();

    // Debug helper functions. ---------------------------------------------------------
    void DebugViewInfo();
    void NotifyLoading();
};
