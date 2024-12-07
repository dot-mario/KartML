// Fill out your copyright notice in the Description page of Project Settings.


#include "FeatureProcessor.h"
#include "Misc\FileHelper.h"
#include "HAL\PlatformFileManager.h"
#include "Misc\Paths.h"

// Sets default values for this component's properties
UFeatureProcessor::UFeatureProcessor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UFeatureProcessor::BeginPlay()
{
	Super::BeginPlay();
	FeatureActor = GetOwner();
}


// Called every frame
void UFeatureProcessor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bSaveFeatures)
	{
		GetActorFeatures(DeltaTime);
		SaveDataToCSV(FeatureFilePath, FormatFeaturesToCSV());
	}
}

void UFeatureProcessor::GetActorFeatures(float DeltaTime)
{
	Features.LinearVel = FeatureActor->GetVelocity();
	Features.LinearAcc = (Features.LinearVel - PrevLinearVel) / DeltaTime;
	PrevLinearVel = Features.LinearVel;

	FRotator ActorRot = FeatureActor->GetActorRotation(); // Current rotation
	FVector CurRot = FVector(ActorRot.Roll, ActorRot.Pitch, ActorRot.Yaw);
	
	FVector CurAngularVel = (CurRot - PrevRot) / DeltaTime;
	PrevRot = CurRot;

	FVector CurAngularAcc = (CurAngularVel - PrevAngularVel) / DeltaTime;
	PrevAngularVel = CurAngularVel;

	Features.AngularVel = CurAngularVel;
	Features.AngularAcc = CurAngularAcc;

	Features.Posdiff = Features.LinearVel * DeltaTime;
	Features.Anglediff = Features.AngularVel * DeltaTime;
}

void UFeatureProcessor::SaveDataToCSV(const FString& FilePath, const FString& DataToWrite)
{
	if (!FPaths::FileExists(FilePath))
	{
		FString Header = TEXT("LinearVelocity,LinearAcceleration,AngularVelocity,AngularAccleration,PositionDiff,AngleDiff\n");
		//FFileHelper::SaveStringArrayToFile()
		FFileHelper::SaveStringToFile(Header, *FilePath);
	}

	FFileHelper::SaveStringToFile(DataToWrite, *FilePath, FFileHelper::EEncodingOptions::AutoDetect);

}

FString UFeatureProcessor::FormatFeaturesToCSV()
{
	FString DataRow = FString::Printf(TEXT("%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n"),
		Features.LinearVel.X,
		Features.LinearVel.Y,
		Features.LinearVel.Z,

		Features.LinearAcc.X,
		Features.LinearAcc.Y,
		Features.LinearAcc.Z,

		Features.AngularVel.X,
		Features.AngularVel.Y,
		Features.AngularVel.Z,

		Features.AngularAcc.X,
		Features.AngularAcc.Y,
		Features.AngularAcc.Z,

		Features.Posdiff.X,
		Features.Posdiff.Y,
		Features.Posdiff.Z,

		Features.Anglediff.X,
		Features.Anglediff.Y,
		Features.Anglediff.Z
		);

	return DataRow;
}

