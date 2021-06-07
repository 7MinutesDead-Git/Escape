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
	// Functions. ----------------------------------------------------------------------------------
	void OpenTheDoor(float DeltaTime);
	void CloseTheDoor(float DeltaTime);
    float GetTotalMassInVolume();
    FLinearColor ColorLightByMass() const;
    void InitializeLights();
    void AdjustLights();
    void AdjustLightsOpened();
    void AdjustLightsShut();

    // Variables. ----------------------------------------------------------------------------------
    float TotalMassInKg = 0;
	float ClosedDoorPos;
	float OpenDoorPos;
    float Elapsed;
    float LightIntensity = 0;
    float MassRatio;
    float TotalMass;
    int ObjectCount;

    FRotator DoorRotation;
    FRotator SmoothedTargetRotation;
    FLinearColor White = {1, 1, 1, 1};

    /// An array of actors inside the chamber (if they have overlap events enabled).
    UPROPERTY() TArray<AActor*> StuffInChamber;
    UPROPERTY() TArray<ALight*> ImportantLights;

    // Pointers ------------------------------------------------------------------------------------
    UPROPERTY(EditAnywhere) ATriggerVolume* ChamberVolume;
    UPROPERTY(EditAnywhere) ALight* ChamberLight;
    UPROPERTY(EditAnywhere) ALight* SignLightOrange;
    UPROPERTY(EditAnywhere) ALight* SignLightBlue;

    // Configurables -------------------------------------------------------------------------------
    /// How often in seconds to update volume weight checks (0 will update every frame).
    UPROPERTY(EditAnywhere) float UpdateRate = 1;
	UPROPERTY(EditAnywhere) float OpenAngle = 90;
    UPROPERTY(EditAnywhere) float MassToOpenDoor = 89;
	UPROPERTY(EditAnywhere) float OpenRate = 1;
	UPROPERTY(EditAnywhere) float DoorOpenTime = 1;

    // Debug ---------------------------------------------------------------------------------------
    UPROPERTY(EditAnywhere) bool EnableDebugMessages = false;
    /// Tripped if any crash-causing errors are found (ie trying to access null pointers).
    bool Ready = true;
    /// Tripped once user has been warned of potentially fatal errors (prevents log spam).
    bool Warned = false;
    /// Prevents log spam from light color update attempts when EnableDebugMessages is true.
    bool DebugColorWarning = false;


};
