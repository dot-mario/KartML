// Copyright Epic Games, Inc. All Rights Reserved.

#include "KartMLWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UKartMLWheelFront::UKartMLWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}