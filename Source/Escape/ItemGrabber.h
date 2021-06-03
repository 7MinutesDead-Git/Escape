// Alex Gulikers, 7 Minutes Dead, Copyright 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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
	// Sets default values for this component's properties
	UItemGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere) APlayerController* Player;
	FPlayerView PlayerView;
	float Reach = 1000;

	// Debug helper functions.
	void DebugLineStuff(FVector LineEnd);

};
