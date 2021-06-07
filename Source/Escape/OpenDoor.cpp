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

    InitializeLights();

    // Not setting ChamberVolume can cause a crash.
	if (!ChamberVolume) {
	    Ready = false;
		UE_LOG(LogTemp, Error,
		    TEXT("OpenDoor component on %s is missing a Trigger Volume! Assign in editor."),
		    *GetOwner()->GetName()
		    );
	}
}


// -------------------------------------------------------------------------------
// Called every frame.
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // If no errors have occurred, then let's go.
	if (Ready) {
		Elapsed += DeltaTime;
	    // We can save some performance by not doing all of this every single frame.
	    if (Elapsed >= UpdateRate) {
	        TotalMassInKg = GetTotalMassInVolume();
	        // Used for scaling values from 0 to 1.
	        MassRatio = TotalMassInKg / MassToOpenDoor;
	        // Light adjustments.
	        LightIntensity = MassRatio * 0.3f;
	        ChamberLight->SetBrightness(LightIntensity);
	        ChamberLight->SetLightColor(ColorLightByMass());
	        SignLightOrange->SetBrightness(LightIntensity);
	        // Debug check.
	        if (EnableDebugMessages) {
	            UE_LOG(LogTemp, Warning,
                    TEXT("%s color set to %s"),
                    *ChamberLight->GetName(),
                    *ChamberLight->GetLightColor().ToString());

	            UE_LOG(LogTemp, Warning,
                    TEXT("Total Mass: %f kg"),
                    TotalMassInKg);
	        }
	    }
        // If we have enough stuff in the chamber, we can open.
	    if (TotalMassInKg >= MassToOpenDoor) {
	        OpenTheDoor(DeltaTime);
	        SignLightBlue->SetBrightness(0.3f);
	        SignLightOrange->SetBrightness(0);
	        ChamberLight->SetBrightness(10);
	        ChamberLight->SetLightColor(White);
	    }
	    // Otherwise keep the door closed. We can also close if items leave the chamber.
	    else {
	        CloseTheDoor(DeltaTime);
	        SignLightBlue->SetBrightness(0);
	    }

	}
    // Otherwise we've encountered errors during setup,
    // so we avoid above loop to prevent editor crashes.
    else if (!Warned) {
        UE_LOG(LogTemp, Error, TEXT("Most functionality has been disabled until errors are fixed. Please check log."));
        Warned = true;
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

    if (EnableDebugMessages) {
        UE_LOG(LogTemp, Warning, TEXT("Objects in field: %i"), Count);
    }

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

// -------------------------------------------------------------------------------
/// Start our logical lights at 0 brightness. Trip not ready flag if lights haven't been set in the editor.
void UOpenDoor::InitializeLights()
{
    ImportantLights.Add(ChamberLight);
    ImportantLights.Add(SignLightBlue);
    ImportantLights.Add(SignLightOrange);

    for (ALight* Light : ImportantLights) {
        if (!Light) {
            Ready = false;

            UE_LOG(LogTemp, Error,
                TEXT("OpenDoor component on %s is missing a light! Assign in editor."),
                *GetOwner()->GetName());
        }
        else {
            Light->SetBrightness(0);
        }
    }
}
