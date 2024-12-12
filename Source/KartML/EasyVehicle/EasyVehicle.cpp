#include "EasyVehicle.h"
#include "Net/UnrealNetwork.h"
#include "EasyVehicleMovementComponent.h"
#include "DrawDebugHelpers.h"

AEasyVehicle::AEasyVehicle(const FObjectInitializer& ObejctInitializer)
    : Super(ObejctInitializer.SetDefaultSubobjectClass<UEasyVehicleMovementComponent>(ACharacter::CharacterMovementComponentName))
{    // Set this character to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    VehicleMovementComponent = Cast<UEasyVehicleMovementComponent>(GetCharacterMovement());
}


// Called when the game starts or when spawned
void AEasyVehicle::BeginPlay()
{
    Super::BeginPlay();
}

// Replication properties
void AEasyVehicle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AEasyVehicle, ServerPositions);
    DOREPLIFETIME(AEasyVehicle, ServerRotations);
}


// Called every frame
void AEasyVehicle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bDrawDebugBox) return;

    // Update local positions and rotations
    TimeSinceLastUpdate += DeltaTime;
    if (TimeSinceLastUpdate >= PositionUpdateInterval)
    {
        // Save local position and rotation
        LocalPositions.Add(GetActorLocation());
        LocalRotations.Add(GetActorRotation());

        // Predict the next position locally (considering movement or input)
        FVector PredictedLocation = GetActorLocation();  // Assuming you already move based on input
        FRotator PredictedRotation = GetActorRotation();

        // Add predicted values to arrays (for debugging)
        LocalPositions.Add(PredictedLocation);
        LocalRotations.Add(PredictedRotation);

        if (HasAuthority())
        {
            // Save server position and rotation (replicated)
            FVector ServerLocation = GetActorLocation(); // Replicated location
            FRotator ServerRotation = GetActorRotation(); // Replicated rotation

            ServerPositions.Add(ServerLocation);
            ServerRotations.Add(ServerRotation);
        }


        // Reset update time
        TimeSinceLastUpdate = 0.0f;

        // Compare local predicted vs. server replicated position and rotation
       // float PositionDifference = FVector::Dist(PredictedLocation, ServerLocation);
       // float RotationDifference = FRotator::NormalizeAxis(PredictedRotation.Yaw - ServerRotation.Yaw);

        // Log differences (optional for debugging)
        //UE_LOG(LogTemp, Warning, TEXT("Position Difference: %f"), PositionDifference);
        //UE_LOG(LogTemp, Warning, TEXT("Rotation Difference: %f"), RotationDifference);
    }

    // Visualize positions for debugging
    DrawDebugPositions();
}

// Called to bind functionality to input
void AEasyVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Draw debug positions and rotations
void AEasyVehicle::DrawDebugPositions()
{
    // Draw local positions and rotations in green
    for (int i = 0; i < LocalPositions.Num(); ++i)
    {
        FVector LocalPos = LocalPositions[i];
        FRotator LocalRot = LocalRotations[i];
        DrawDebugBox(GetWorld(), LocalPos, FVector(30.0f, 30.0f, 30.0f), FQuat(LocalRot), FColor::Green, false, -1, 0, 2.0f);
    }

    // Draw server positions and rotations in red
    for (int i = 0; i < ServerPositions.Num(); ++i)
    {
        FVector ServerPos = ServerPositions[i];
        FRotator ServerRot = ServerRotations[i];
        DrawDebugBox(GetWorld(), ServerPos, FVector(30.0f, 30.0f, 30.0f), FQuat(ServerRot), FColor::Red, false, -1, 0, 2.0f);
    }
}
