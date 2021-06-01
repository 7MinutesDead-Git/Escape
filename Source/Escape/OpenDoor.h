// Alex Gulikers, 7 Minutes Dead, Copyright 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OpenDoor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	float TargetPitch = 0;
	float TargetYaw = 180;
	float TargetRoll = 0;
	float OpenRate = 1;
	FRotator TargetRotation{TargetPitch, TargetYaw, TargetRoll};
	FRotator SmoothedTargetRotation;


};
