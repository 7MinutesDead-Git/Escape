// Copyright 2021, Alex Gulikers, 7 Minutes Dead.
// Although these are mostly notes for reference to myself, so you are welcome to pick it apart!


#include "OpenDoor.h"
#include "Components/PrimitiveComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"

#define OUT

// -------------------------------------------------------------------------------
// Sets default values for this component's properties.
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	// You can turn these features off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

// -------------------------------------------------------------------------------
// Called when the game starts.
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Elapsed = 0;

	DoorRotation = GetOwner()->GetActorRotation();
	ClosedDoorPos = DoorRotation.Yaw;
	OpenDoorPos = DoorRotation.Yaw + OpenAngle;

    // Not setting PressurePlate can cause a crash.
	if (!ChamberVolume) {
		UE_LOG(LogTemp, Error,
		    TEXT("Pressure Plate hasn't been set for %s! Check OpenDoor component."),
		    *GetOwner()->GetName()
		    );
	}

    ChamberLight->SetBrightness(0);
    SignLightBlue->SetBrightness(0);
    SignLightOrange->SetBrightness(0);
}


// -------------------------------------------------------------------------------
// Called every frame.
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (ChamberVolume) {
		Elapsed += DeltaTime;

	    // We can save some performance by not doing all of this every single frame.
	    if (Elapsed >= UpdateRate) {
	        TotalMassInKg = GetTotalMassInVolume();
	        // Used for scaling values from 0 to 1.
	        MassRatio = TotalMassInKg / MassToOpenDoor;

	        UE_LOG(LogTemp, Warning, TEXT("Total Mass: %f kg"), TotalMassInKg)

	        LightIntensity = MassRatio * 0.3f;
	        ChamberLight->SetBrightness(LightIntensity);
	        ChamberLight->SetLightColor(ColorLightByMass());
	        SignLightOrange->SetBrightness(LightIntensity);
            // Reset timer for next ping/update.
	        Elapsed = 0;
	    }

	    if (TotalMassInKg >= MassToOpenDoor) {
	        OpenTheDoor(DeltaTime);
	        SignLightBlue->SetBrightness(0.3f);
	        SignLightOrange->SetBrightness(0);
	    }
	    else {
	        CloseTheDoor(DeltaTime);
	        SignLightBlue->SetBrightness(0);
	    }
	}
}

// -------------------------------------------------------------------------------
/// Smoothly open the door towards OpenDoorPos.
void UOpenDoor::OpenTheDoor(float DeltaTime)
{
	DoorRotation = GetOwner()->GetActorRotation();
	// FInterpTo gives us a framerate-independent Lerp for floats, by allowing us to pass in DeltaTime too.
	SmoothedTargetRotation.Yaw = FMath::FInterpTo(DoorRotation.Yaw, OpenDoorPos, DeltaTime, OpenRate);
	GetOwner()->SetActorRotation(SmoothedTargetRotation);
}

// -------------------------------------------------------------------------------
/// Smoothly close the door back towards ClosedDoorPos.
void UOpenDoor::CloseTheDoor(float DeltaTime)
{
	DoorRotation = GetOwner()->GetActorRotation();
	SmoothedTargetRotation.Yaw = FMath::FInterpTo(DoorRotation.Yaw, ClosedDoorPos, DeltaTime, OpenRate);
	GetOwner()->SetActorRotation(SmoothedTargetRotation);
}


// -------------------------------------------------------------------------------
/// Get the total mass of all actors in the trigger volume. \n\n
/// Make sure Static Mesh Components you want to contribute to this total mass have
/// "generate overlap event" ticked, or they won't contribute their mass at all.
float UOpenDoor::GetTotalMassInVolume()
{
    // Fill our array with any AActors in the trigger volume.
    ChamberVolume->GetOverlappingActors(OUT StuffInChamber);
    float TotalMass = 0;
    int Count = 0;

    // For each thing in volume:
    for (AActor* Thing : StuffInChamber) {
        // Get the mass of current thing.
        const float MassOfThing = Thing->FindComponentByClass<UStaticMeshComponent>()->GetMass();
        // Add it to the total mass.
        TotalMass += MassOfThing;
        Count += 1;
    }

    UE_LOG(LogTemp, Warning, TEXT("Objects in field: %i"), Count);     // DEBUG line.
    return TotalMass;
}

// -------------------------------------------------------------------------------
/// Return a light color that scales increasingly red and decreasingly blue
/// by mass in the trigger volume.
FLinearColor UOpenDoor::ColorLightByMass() const
{
    float Red = MassRatio;
    float Green = 0;
    float Blue = 1 - MassRatio;
    float Alpha = 1;
    // Red, Green, Blue, Alpha.  Red will increase as our mass increases.
    const FLinearColor LightColor = {Red, Green, Blue, Alpha};
    return LightColor;
}
