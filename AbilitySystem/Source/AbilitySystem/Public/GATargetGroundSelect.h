// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GATargetGroundSelect.generated.h"

class UDecalComponent;
class USceneComponent;

/**
 * The tutorial called their class 'GATargetActorGroundSelect'. I left out the 'Actor' in the name for mine.
 */
UCLASS()
class ABILITYSYSTEM_API AGATargetGroundSelect : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:

	AGATargetGroundSelect();

	/** Initialize and begin targeting logic  */
	virtual void StartTargeting(UGameplayAbility* Ability) override;

	/** Requesting targeting data, but not necessarily stopping/destroying the task. Useful for external target data requests. */
	virtual void ConfirmTargetingAndContinue() override;

	UFUNCTION(BlueprintCallable, Category = "GroundSelect")
	bool GetPlayerLookingPoint(FVector& OutViewPoint);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "GroundSelect")
	float Radius;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GroundBlast")
	UDecalComponent* Decal;

	USceneComponent* RootComp;
};
