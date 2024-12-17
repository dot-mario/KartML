// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EasyVehicle.generated.h"

UCLASS()
class KARTML_API AEasyVehicle : public ACharacter
{
	GENERATED_BODY()

public:

	AEasyVehicle(const FObjectInitializer& ObejctInitializer);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	class UEasyVehicleMovementComponent* VehicleMovementComponent;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(EditDefaultsOnly)
	bool bDrawDebugBox = false;

	TArray<FVector> LocalPositions;
	UPROPERTY(Replicated)
	TArray<FVector> ServerPositions;
	TArray<FRotator> LocalRotations;
	UPROPERTY(Replicated)
	TArray<FRotator> ServerRotations;

	float PositionUpdateInterval = 0.01;  // 0.1초마다 업데이트
	float TimeSinceLastUpdate = 0.0f;

	void DrawDebugPositions();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableDebug;

};
