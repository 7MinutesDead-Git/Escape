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
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPE_API UItemGrabber : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties.
	UItemGrabber();
	// Called every frame.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Variables.
	FPlayerView PlayerView;
	FHitResult GrabbableHit;
	FVector LineTraceEnd;
	bool HoldingItem = false;
	float Reach = 1000.f;

	// Pointers.
	UPROPERTY(EditAnywhere)
	APlayerController* Player;
	UPROPERTY()
	AActor* GrabbableActor;
	UPROPERTY()
	UPhysicsHandleComponent* PhysicsHandle;
	UPROPERTY()
	UInputComponent* PlayerInput;

	// Functions.
	void GetGrabbableObject();
	void GetPhysicsHandle();
	void GetPlayerInput();
	void BindActionsToKeys();
	void Grab();
	void Drop();
	void Throw();

	// Debug helper functions.
	void DebugLineStuff();
	void NotifyLoading();

};
