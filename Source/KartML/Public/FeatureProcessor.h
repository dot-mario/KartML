// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FeatureProcessor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFileWrite);

USTRUCT(BlueprintType)
struct FLearningData
{
	GENERATED_BODY()

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bSaveFeatures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FLearningData Features;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FString FeatureFilePath;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FString FeatureFile;

	FVector PrevLinearVel;
	FVector PrevRot;
	FVector PrevAngularVel;

	int32 FileIndex;

	FTimerHandle FileWriteTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float DeltaT;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = true))
	FOnFileWrite OnFileWrite;

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
	//UFUNCTION(BlueprintCallable)
	//void SwitchSaveFeatureBool() { bSaveFeatures = !bSaveFeatures; UE_LOG(LogTemp, Warning, TEXT("bSaveFeatures: %d"), bSaveFeatures); }

	UFUNCTION(BlueprintCallable)
	void WriteFeatures(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void GetActorFeatures(float DeltaTime = 0);

	UFUNCTION(BlueprintCallable)
	void SaveDataToCSV(const FString& FilePath, const FString& DataToWrite);
	
	UFUNCTION(BlueprintCallable)
	FString FormatFeaturesToCSV();

	UFUNCTION()
	void OnFileWriteHandler();

	UFUNCTION(BlueprintCallable)
	void SwitchSaveFeatureMode();
};
