// Fill out your copyright notice in the Description page of Project Settings.

#include "GATargetActorGetAround.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameplayAbility.h"

void AGATargetActorGetAround::StartTargeting(UGameplayAbility* Ability)
{
	OwningAbility = Ability;

	MasterPC = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo()->GetInstigatorController());

}

void AGATargetActorGetAround::ConfirmTargetingAndContinue()
{
	APawn* OwningPawn = MasterPC->GetPawn();
	if (!OwningPawn)
	{
		TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
		return;
	}
	FVector ViewLocation = OwningPawn->GetActorLocation();
	

	TArray<FOverlapResult> Overlaps;
	TArray<TWeakObjectPtr<AActor>> OverlappedActors;
	bool TraceComplex = false;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bTraceComplex = TraceComplex;
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	APawn* MasterPawn = MasterPC->GetPawn();
	if (MasterPawn)
	{
		// Ignore ourselves
		CollisionQueryParams.AddIgnoredActor(MasterPawn->GetUniqueID());
	}

	bool TryOverlap = GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		ViewLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(Radius),
		CollisionQueryParams);

	if (TryOverlap)
	{
		/* Another suggested way of doing this.. cleaner...
		instead of iterating using an int, it is easier to write a ranged based loop
		for (const auto& o : Overlaps)
		{
		if (auto pawn = Cast<APawn>(o.GetActor()))
		{
		OverlappedActors.AddUnique(pawn);
		}
		}*/


		for (int32 i = 0; i < Overlaps.Num(); i++)
		{
			APawn* PawnOverlapped = Cast<APawn>(Overlaps[i].GetActor());
			if (PawnOverlapped && !OverlappedActors.Contains(PawnOverlapped))
			{
				OverlappedActors.Add(PawnOverlapped);
			}
		}
	}

	// did we find a target?
	if (OverlappedActors.Num() > 0)
	{
		// This is target data that we can now broadcast out
		FGameplayAbilityTargetDataHandle TargetData = StartLocation.MakeTargetDataHandleFromActors(OverlappedActors);
		
		TargetDataReadyDelegate.Broadcast(TargetData);
	}
	else
	{
		TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
	}
}
