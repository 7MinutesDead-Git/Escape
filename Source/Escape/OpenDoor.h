// Copyright 2021, Alex Gulikers, 7 Minutes Dead.
// Although these are mostly notes for reference to myself, so you are welcome to pick it apart!
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Light.h"
#include "Engine/TriggerVolume.h"
#include "Math/Rotator.h"
#include "OpenDoor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:
	// -------------------------------------------------------------------------
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// -------------------------------------------------------------------------
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// -------------------------------------------------------------------------
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// -------------------------------------------------------------------------
	void OpenTheDoor(float DeltaTime);
	void CloseTheDoor(float DeltaTime);
    float GetTotalMassInVolume();
    FLinearColor ColorLightByMass() const;
    float TotalMassInKg = 0;
	float ClosedDoorPos;
	float OpenDoorPos;
    float Elapsed;
    float LightIntensity = 0;
    float MassRatio;

    FRotator DoorRotation;
    FRotator SmoothedTargetRotation;

    /// An array of actors inside the chamber (if they have overlap events enabled).
    UPROPERTY() TArray<AActor*> StuffInChamber;

    UPROPERTY(EditAnywhere) ATriggerVolume* ChamberVolume;
    UPROPERTY(EditAnywhere) ALight* ChamberLight;
    UPROPERTY(EditAnywhere) ALight* SignLightOrange;
    UPROPERTY(EditAnywhere) ALight* SignLightBlue;

    /// How often in seconds to update volume weight checks (0 will update every frame).
    UPROPERTY(EditAnywhere) float UpdateRate = 1;
	UPROPERTY(EditAnywhere) float OpenAngle = 90;

    UPROPERTY(EditAnywhere) float MassToOpenDoor = 89;
	UPROPERTY(EditAnywhere) float OpenRate = 1;
	UPROPERTY(EditAnywhere) float DoorOpenTime = 1;


};
