// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EasyVehicleMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class KARTML_API UEasyVehicleMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:

	UEasyVehicleMovementComponent();

protected:




	UPROPERTY(EditDefaultsOnly)
	bool bUseExtrapolationForPredict = true;

	UPROPERTY(EditDefaultsOnly)
	float NetworkLatency = 0.1f; // Time threshold for enabling extrapolation

	virtual void SmoothClientPosition(float DeltaSeconds) override;

private:

	void SmoothClientPosition_InterpolateAndExtrapolate(float DeltaSeconds);
	void SmoothClientPosition_UpdateVisuals_InterpolateAndExtrapolate();
};
