// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FeatureProcessor.generated.h"

struct FLearningData
{
	FVector LinearVel;
	FVector LinearAcc;
	FVector AngularVel;
	FVector AngularAcc;
	FVector Posdiff; // vel * delta t 
	FVector Anglediff; // angvel * delta t 
	//bool bDriftStatus;
};

struct FOutputFeatures
{
	FVector Posdiff; // velocity * delta t 
	FVector Angdiff; // AngularVel * delta t 
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KARTML_API UFeatureProcessor : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	AActor* FeatureActor;

	FLearningData Features;
	FVector PrevLinearVel;
	FVector PrevRot;
	FVector PrevAngularVel;

	const FString& FeatureFilePath = "TODO";

	bool bSaveFeatures;

public:	
	// Sets default values for this component's properties
	UFeatureProcessor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	void GetActorFeatures(float DeltaTime = 0);

	void SaveDataToCSV(const FString& FilePath, const FString& DataToWrite);
	
	FString FormatFeaturesToCSV();
};
