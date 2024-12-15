// Fill out your copyright notice in the Description page of Project Settings.

#include "FeatureProcessor.h"
#include "Misc\FileHelper.h"
#include "HAL\PlatformFileManager.h"
#include "Misc\Paths.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UFeatureProcessor::UFeatureProcessor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	FeatureFilePath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Features"));
	FeatureFilePath = FPaths::Combine(FeatureFilePath, TEXT("data0.csv"));

	FileWriteDelay = 0.1f;
}

// Called when the game starts
void UFeatureProcessor::BeginPlay()
{
	Super::BeginPlay();
	FeatureActor = GetOwner();

	// Set Filewrite timer
	GetWorld()->GetTimerManager().SetTimer(FileWriteTimer, [this]()
	{
		WriteFeatures(FileWriteDelay);
	}, FileWriteDelay, true);

	GetWorld()->GetTimerManager().PauseTimer(FileWriteTimer);

	OnFileWrite.AddDynamic(this, &UFeatureProcessor::OnFileWriteHandler);
}

// Called every frame
void UFeatureProcessor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//if (bSaveFeatures)
	//{
	//	WriteFeatures(DeltaTime);
	//}
}

void UFeatureProcessor::WriteFeatures(float DeltaTime)
{
	GetActorFeatures(DeltaTime);
	//SaveDataToCSV(FeatureFilePath, FormatFeaturesToCSV());
}

void UFeatureProcessor::GetActorFeatures(float DeltaTime)
{
	// Euler rotation calc
	//FRotator ActorRot = FeatureActor->GetActorRotation(); // Current rotation
	//FVector CurRot = FVector(ActorRot.Roll, ActorRot.Pitch, ActorRot.Yaw);
	//FVector CurAngularVel = (CurRot - PrevRot) / DeltaTime; // TODO ? 
	//FVector CurAngularAcc = (CurAngularVel - PrevAngularVel) / DeltaTime;

	// Quaternion rotation calc
	FQuat CurQuat = FeatureActor->GetActorQuat();
	FQuat DeltaQuat = CurQuat * PrevQuat.Inverse();
	
	FVector Axis;
	float Angle;
	DeltaQuat.ToAxisAndAngle(Axis, Angle);

	FVector CurAngularVel = Axis * Angle / DeltaTime;
	FVector CurAngularAcc = (CurAngularVel - PrevAngularVel) / DeltaTime;

	FVector CurPos = FeatureActor->GetActorLocation();
	FVector CurLinearVel = FeatureActor->GetVelocity() * 0.01f;

	// TODO: Ground truth i+1 frame에서 계산 후, i frame 데이터 이전으로 가서 write. 
	++DataIteration;
	if (DataIteration >= 1)
	{
		Features.GTruthPosDiff = CurPos - PrevPos;
		Features.GTruthRotDiff = Axis * Angle;

		SaveDataToCSV(FeatureFilePath, FormatFeaturesToCSV());
	}

	// Linear
	Features.LinearVel = CurLinearVel;
	Features.LinearAcc = (Features.LinearVel - PrevLinearVel) / DeltaTime;

	// Rotation
	Features.AngularVel = CurAngularVel;
	Features.AngularAcc = CurAngularAcc;

	// Delta
	Features.VelmulDelta = Features.LinearVel * DeltaTime;
	Features.AngvelmulDelta = Features.AngularVel * DeltaTime;

	// Value update
	PrevPos = CurPos;
	PrevLinearVel = CurLinearVel;
	//PrevRot = CurRot;
	PrevQuat = CurQuat;
	PrevAngularVel = CurAngularVel;
}

void UFeatureProcessor::SaveDataToCSV(const FString& FilePath, const FString& DataToWrite)
{
	if (!FPaths::FileExists(FilePath))
	{
		FString Header = TEXT("LinearVel.X,LinearVel.Y,LinearAcc.X,LinearAcc.Y,AngularVel.Z,AngularAcc.Z,VelMulDelta.X,VelMulDelta.Y, Angvelmuldelta.Z,GTruthPosDiff.X,GTruthPosDiff.Y,GTruthRotDiff.Z,\n");
		//FFileHelper::SaveStringArrayToFile()
		FFileHelper::SaveStringToFile(Header, *FilePath);
	}

	bool Result = FFileHelper::SaveStringToFile(DataToWrite, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	if (Result)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *DataToWrite);
	}
}

FString UFeatureProcessor::FormatFeaturesToCSV()
{
	FString DataRow = FString::Printf(TEXT("%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,\n"),
		Features.LinearVel.X,
		Features.LinearVel.Y,

		Features.LinearAcc.X,	
		Features.LinearAcc.Y,

		Features.AngularVel.Z,

		Features.AngularAcc.Z,

		Features.VelmulDelta.X,
		Features.VelmulDelta.Y,

		Features.AngvelmulDelta.Z,

		Features.GTruthPosDiff.X,
		Features.GTruthPosDiff.Y,

		Features.GTruthRotDiff.Z
		);

	//FString DataRow = FString::Printf(TEXT("%f,%f,%f\n"),
	//	Features.AngularAcc.X,
	//	Features.AngularAcc.Y,
	//	Features.AngularAcc.Z
	//);

	return DataRow;
}

void UFeatureProcessor::OnFileWriteHandler()
{
	bSaveFeatures = !bSaveFeatures;

	if (bSaveFeatures)
		GetWorld()->GetTimerManager().UnPauseTimer(FileWriteTimer);
	else
	{
		GetWorld()->GetTimerManager().PauseTimer(FileWriteTimer);
		DataIteration = -1;
	}

	UE_LOG(LogTemp, Warning, TEXT("FILEWRITE MODE: %d"), bSaveFeatures);
}

void UFeatureProcessor::SwitchSaveFeatureMode()
{
	if(OnFileWrite.IsBound())
		OnFileWrite.Broadcast();
}

