// Alex Gulikers, 7 Minutes Dead, Copyright 2021

#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"


#include "ItemGrabber.h"


// Sets default values for this component's properties
UItemGrabber::UItemGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UItemGrabber::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning,
		TEXT("UItemGrabber loaded successfully on %s."),
		*GetOwner()->GetName()
	);

	Player = GetWorld()->GetFirstPlayerController();
	// ...
}


// Called every frame
void UItemGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Get player's viewpoint and Set to our PlayerView data structure. (GetPlayerViewPoint uses out parameters)
	Player->GetPlayerViewPoint(PlayerView.Location, PlayerView.Rotation);

	// The max length point of the ray cast we want.
	FVector LineTraceEnd = PlayerView.Location + PlayerView.Rotation.Vector() * Reach;

	DebugLineStuff(LineTraceEnd);

	// Raycast from view to certain distance, see what raycast hits.
	// If key is pressed, grab object and move to set short distance in front of player.
	// If key is released, release object.
	// ...
}


// --------------------------------------------------------------------
/// Draw debug line for grabby hands, and log player position and rotation.
void UItemGrabber::DebugLineStuff(FVector LineEnd)
{
	FColor Color;

	DrawDebugLine(
		GetWorld(),				 // InWorld.
		PlayerView.Location,	 // LineStart.
		LineEnd,				 // LineEnd.
		Color.FromHex("00FF15"), // Color.
		false,					 // PersistentLines.
		1,						 // LifeTime.
		0,						 // DepthPriority
		3						 // Thickness.
	);

	UE_LOG(LogTemp, Warning,
		TEXT("Player Position: %s //// Player Rotation: %s"),
		*PlayerView.Location.ToCompactString(),
		*PlayerView.Rotation.ToCompactString()
	);
}
