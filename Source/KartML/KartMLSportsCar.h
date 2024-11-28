// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KartMLPawn.h"
#include "KartMLSportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class KARTML_API AKartMLSportsCar : public AKartMLPawn
{
	GENERATED_BODY()
	
public:

	AKartMLSportsCar();
};
