// Fill out your copyright notice in the Description page of Project Settings.


#include "EasyVehicleMovementComponent.h"
#include "EngineStats.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

UEasyVehicleMovementComponent::UEasyVehicleMovementComponent()
{
}

void UEasyVehicleMovementComponent::SmoothClientPosition(float DeltaSeconds)
{
	//SCOPE_CYCLE_COUNTER(STAT_CharacterMovementSmoothClientPosition_Interp);
	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	USkeletalMeshComponent* Mesh = CharacterOwner->GetMesh();

	if (ClientData)
	{
		if (bUseExtrapolationForPredict == false)
		{
			Super::SmoothClientPosition(DeltaSeconds);
		}
		else
		{
			if (!HasValidData() || NetworkSmoothingMode == ENetworkSmoothingMode::Disabled)
			{
				return;
			}

			// We shouldn't be running this on a server that is not a listen server.
			checkSlow(GetNetMode() != NM_DedicatedServer);
			checkSlow(GetNetMode() != NM_Standalone);

			// Only client proxies or remote clients on a listen server should run this code.
			const bool bIsSimulatedProxy = (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy);
			const bool bIsRemoteAutoProxy = (CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy);
			if (!ensure(bIsSimulatedProxy || bIsRemoteAutoProxy))
			{
				return;
			}

			SmoothClientPosition_InterpolateAndExtrapolate(DeltaSeconds);
			SmoothClientPosition_UpdateVisuals_InterpolateAndExtrapolate();

		}
	}
	// UE_LOG(LogTemp, Warning, TEXT("Override SmoothClientPosition"));
}
// 등속 적용 O, X, 지연시간에 따른 Data 및 Data 플롯
void UEasyVehicleMovementComponent::SmoothClientPosition_InterpolateAndExtrapolate(float DeltaSeconds)
{
	// SCOPE_CYCLE_COUNTER(STAT_CharacterMovementSmoothClientPosition_Interp);
	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	if (ClientData)
	{
		FVector LocalPos = ClientData->MeshTranslationOffset;
		FQuat LocalRot = ClientData->MeshRotationOffset;
		if (GetWorld())
		{
			DrawDebugBox(GetWorld(), LocalPos + GetActorLocation() , FVector(30.0f, 30.0f, 30.0f), LocalRot, FColor::Red, true, 10.f, 0, 2.0f);
		}


		//UE_LOG(LogTemp, Warning, TEXT("DeltaSeconds = %f"), DeltaSeconds);
		//UE_LOG(LogTemp, Warning, TEXT("LastCorrectionDelta = %f"), ClientData->LastCorrectionDelta);
		//UE_LOG(LogTemp, Warning, TEXT("NetworkLatency = %f"), NetworkLatency);
		// 외삽 위치 계산
		FVector ExtrapolatedTranslation =  (Velocity * NetworkLatency);
		UE_LOG(LogTemp, Warning, TEXT("PrePOsition = %f, %f, %f"), LocalPos.X, LocalPos.Y, LocalPos.Z);
		//UE_LOG(LogTemp, Warning, TEXT("ExtrapolatedTranslation = %f, %f, %f"), ExtrapolatedTranslation.X, ExtrapolatedTranslation.Y, ExtrapolatedTranslation.Z);
		UE_LOG(LogTemp, Warning, TEXT("OriginalMeshTranslationOffset = %f, %f, %f"), ClientData->OriginalMeshTranslationOffset.X, ClientData->OriginalMeshTranslationOffset.Y, ClientData->OriginalMeshTranslationOffset.Z);


		/*
		// 외삽 회전 계산 (회전 속도가 있을 경우)
		FQuat ExtrapolatedRotation = ClientData->MeshRotationOffset;
		if (RotationVelocity.SizeSquared() > UE_SMALL_NUMBER)
		{
			ExtrapolatedRotation = ClientData->MeshRotationOffset * FQuat::MakeFromEuler(RotationVelocity * ExtrapolationTime);
		}
		*/
		
		const float SmoothLocationTime = Velocity.IsZero() ? 0.5f * ClientData->SmoothNetUpdateTime : ClientData->SmoothNetUpdateTime;
		if (DeltaSeconds < SmoothLocationTime)
		{
			// Slowly decay translation offset
			ClientData->MeshTranslationOffset = (ExtrapolatedTranslation); // * (1.f - DeltaSeconds / SmoothLocationTime));
		}
		else
		{
			ClientData->MeshTranslationOffset = FVector::ZeroVector;
		}

		// Smooth rotation
		const FQuat MeshRotationTarget = ClientData->MeshRotationTarget;
		if (DeltaSeconds < ClientData->SmoothNetUpdateRotationTime)
		{
			// Slowly decay rotation offset
			ClientData->MeshRotationOffset = FQuat::FastLerp(ClientData->MeshRotationOffset, MeshRotationTarget, DeltaSeconds / ClientData->SmoothNetUpdateRotationTime).GetNormalized();
		}
		else
		{
			ClientData->MeshRotationOffset = MeshRotationTarget;
		}

		// Check if lerp is complete
		if (ClientData->MeshTranslationOffset.IsNearlyZero(1e-2f) && ClientData->MeshRotationOffset.Equals(MeshRotationTarget, 1e-5f))
		{
			bNetworkSmoothingComplete = true;
			// Make sure to snap exactly to target values.
			ClientData->MeshTranslationOffset = FVector::ZeroVector;
			ClientData->MeshRotationOffset = MeshRotationTarget;
		}
		else
		{
			bNetworkSmoothingComplete = true;
		}


		LocalPos = ClientData->MeshTranslationOffset;
		LocalRot = ClientData->MeshRotationOffset;

		if (GetWorld())
		{
			DrawDebugBox(GetWorld(), LocalPos + GetActorLocation(), FVector(30.0f, 30.0f, 30.0f), LocalRot, FColor::Green, true, 10.f, 0, 2.0f);
		}
		UE_LOG(LogTemp, Warning, TEXT("AfterPosition = %f, %f, %f"), LocalPos.X, LocalPos.Y, LocalPos.Z);
		UE_LOG(LogTemp, Warning, TEXT("MeshTranslationOffset = %f, %f, %f"), ClientData->MeshTranslationOffset.X, ClientData->MeshTranslationOffset.Y, ClientData->MeshTranslationOffset.Z);
	}
}


void UEasyVehicleMovementComponent::SmoothClientPosition_UpdateVisuals_InterpolateAndExtrapolate()
{
	//SCOPE_CYCLE_COUNTER(STAT_CharacterMovementSmoothClientPosition_Visual);
	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	USkeletalMeshComponent* Mesh = CharacterOwner->GetMesh();
	if (ClientData && Mesh && !Mesh->IsSimulatingPhysics())
	{
		const USceneComponent* MeshParent = Mesh->GetAttachParent();

		FVector MeshParentScale = MeshParent != nullptr ? MeshParent->GetComponentScale() : FVector(1.0f, 1.0f, 1.0f);

		MeshParentScale.X = FMath::IsNearlyZero(MeshParentScale.X) ? 1.0f : MeshParentScale.X;
		MeshParentScale.Y = FMath::IsNearlyZero(MeshParentScale.Y) ? 1.0f : MeshParentScale.Y;
		MeshParentScale.Z = FMath::IsNearlyZero(MeshParentScale.Z) ? 1.0f : MeshParentScale.Z;

		// Adjust extrapolated mesh location and rotation
		const FVector NewRelTranslation = (UpdatedComponent->GetComponentToWorld().InverseTransformVectorNoScale(ClientData->MeshTranslationOffset) / MeshParentScale) + CharacterOwner->GetBaseTranslationOffset();
		const FQuat NewRelRotation = ClientData->MeshRotationOffset * CharacterOwner->GetBaseRotationOffset();
		Mesh->SetRelativeLocationAndRotation(NewRelTranslation, NewRelRotation, false, nullptr, GetTeleportType());	
	}
}
