// Fill out your copyright notice in the Description page of Project Settings.

#include "GATargetGroundSelect.h"
#include "GameplayAbilityTargetActor.h"
#include "GameplayAbility.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"

AGATargetGroundSelect::AGATargetGroundSelect()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);
	Decal->SetupAttachment(RootComp);
	Radius = 200.0f;
	Decal->DecalSize = FVector(Radius);
}

void AGATargetGroundSelect::StartTargeting(UGameplayAbility* Ability)
{
	OwningAbility = Ability;

	MasterPC = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo()->GetInstigatorController());

	Decal->DecalSize = FVector(Radius);
}


void AGATargetGroundSelect::ConfirmTargetingAndContinue()
{
	FVector ViewLocation;
	GetPlayerLookingPoint(ViewLocation);

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

	FGameplayAbilityTargetData_LocationInfo* CenterLocation = new FGameplayAbilityTargetData_LocationInfo();
	if (Decal)
	{
		CenterLocation->TargetLocation.LiteralTransform = Decal->GetComponentTransform();
		CenterLocation->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	}
	

	// did we find a target?
	if (OverlappedActors.Num() > 0)
	{
		// This is target data that we can now broadcast out
		FGameplayAbilityTargetDataHandle TargetData = StartLocation.MakeTargetDataHandleFromActors(OverlappedActors);
		TargetData.Add(CenterLocation);
		TargetDataReadyDelegate.Broadcast(TargetData);
	}
	else
	{
		TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle(CenterLocation));
	}
}

bool AGATargetGroundSelect::GetPlayerLookingPoint(FVector& OutViewPoint)
{
	FVector ViewPoint;
	FRotator ViewRotation;
	MasterPC->GetPlayerViewPoint(ViewPoint, ViewRotation);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;

	// Our pawn
	APawn* MasterPawn = MasterPC->GetPawn();
	if (MasterPawn)
	{
		// Ignore ourselves
		QueryParams.AddIgnoredActor(MasterPawn->GetUniqueID());
	}

	
	// do a linetrace
	bool TryTrace = GetWorld()->LineTraceSingleByChannel(HitResult, ViewPoint, ViewPoint + ViewRotation.Vector() * 10000.0f, ECC_Visibility, QueryParams);
	if (TryTrace)
	{
		// Exactly where in the world the player is looking
		OutViewPoint = HitResult.ImpactPoint;
	}
	else
	{
		OutViewPoint = FVector();
	}

	return TryTrace;

}

void AGATargetGroundSelect::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector LookingPoint;
	GetPlayerLookingPoint(LookingPoint);

	Decal->SetWorldLocation(LookingPoint);


}
