// Copyright Epic Games, Inc. All Rights Reserved.

#include "KartMLWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UKartMLWheelRear::UKartMLWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}