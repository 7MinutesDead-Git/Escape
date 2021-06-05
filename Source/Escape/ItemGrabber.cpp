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
	// ...
}


// ----------------------------------------------------------------------
/// Called every frame.
void UItemGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Get player's viewpoint and set it to our PlayerView data structure.
	Player->GetPlayerViewPoint(
		OUT PlayerView.Location,
		OUT PlayerView.Rotation);

	GetGrabbableObject();
	DebugLineStuff();

	// If key is pressed, grab object and move to set short distance in front of player.
	// If key is released, release object.
	// ...
}


// ----------------------------------------------------------------------
/// Do a line trace and see what object we can grab. Update GrabbableHit.
void UItemGrabber::GetGrabbableObject()
{
	// The max length point of the ray cast we want.
	// Player location + rotation * the length of grab.
	LineTraceEnd = PlayerView.Location + PlayerView.Rotation.Vector() * Reach;

	// Define what should be considered a collision for what can be grabbed.
	const FCollisionQueryParams TraceParameters(
		FName(TEXT("")),	// The tag name.
		false,				// Use complex collisions?
		GetOwner()			// Which objects to ignore (make sure the ray hitting ourselves doesn't count).
	);

	// LineTrace (raycast) from player to LineTraceEnd and see what we hit.
	GetWorld()->LineTraceSingleByObjectType(
		OUT GrabbableHit,
		PlayerView.Location,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECC_PhysicsBody),
		TraceParameters
	);
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
	// Run Grab function when "Grab" key is pressed.
	// (1) Action/input name, (2) KeyEvent type, (3) Object, (4) Point to &address of function.
	PlayerInput->BindAction("Grab",	IE_Pressed,	this, &UItemGrabber::Grab);
	PlayerInput->BindAction("Drop", IE_Pressed, this, &UItemGrabber::Drop);
	PlayerInput->BindAction("Throw", IE_Pressed, this, &UItemGrabber::Throw);
}

// --------------------------------------------------------------------
/// Grab an object in view if not holding item (makes grab/drop a toggle).
void UItemGrabber::Grab()
{
	if (!HoldingItem) {
		HoldingItem = true;
		// Reach any actors with physics body collision channel set.
		// If we hit something, then attach physics handle.
		// TODO...
	}
}

// --------------------------------------------------------------------
/// Drop item if holding one (makes grab/drop a toggle).
void UItemGrabber::Drop()
{
	if (HoldingItem) {
		HoldingItem = false;
		// Release the physics handle of the actor we grabbed.
		// TODO...
	}
}

// --------------------------------------------------------------------
/// Throw item if holding one!
void UItemGrabber::Throw()
{
	if (HoldingItem) {
		HoldingItem = false;
		// Release the physics handle of the actor we grabbed.
		// Impulse object away from view.
		// TODO...
	}
}


// --------------------------------------------------------------------
// Debug helper functions.
// --------------------------------------------------------------------

/// Draw debug line for grabby hands, log grabbable object hit.
void UItemGrabber::DebugLineStuff()
{
	FColor Color;

	DrawDebugLine(
		GetWorld(),				 // InWorld.
		PlayerView.Location,	 // LineStart.
		LineTraceEnd,			 // LineEnd.
		Color.FromHex("00FF15"), // Color.
		false,					 // PersistentLines.
		1,						 // LifeTime.
		0,						 // DepthPriority
		2						 // Thickness.
	);

	// Checking if GrabbableHit has an actor yet,
	// should stop us from referencing a null pointer and crashing.
	if (GrabbableHit.GetActor()) {
		UE_LOG(LogTemp, Warning,
			TEXT("Grabbable object: %s"),
			*GrabbableHit.GetActor()->GetName()
		);
	}
}

/// Notify this component was loaded successfully.
void UItemGrabber::NotifyLoading()
{
	UE_LOG(LogTemp, Warning,
	TEXT("UItemGrabber loaded successfully on %s."),
	*GetOwner()->GetName()
);
}
