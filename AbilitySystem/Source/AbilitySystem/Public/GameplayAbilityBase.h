// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbility.h"
#include "AbilityTypes.h"
#include "GameplayAbilityBase.generated.h"

class UMaterialInstance;
//struct FGameplayAbilityInfo;

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityBase")
	UMaterialInstance* UIMaterial;

	UFUNCTION(BlueprintCallable, Category = "AbilityBase")
	FGameplayAbilityInfo GetAbilityInfo();
	
};
