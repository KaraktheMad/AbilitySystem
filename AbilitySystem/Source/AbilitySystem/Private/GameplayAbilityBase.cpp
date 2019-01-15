// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/GameplayAbilityBase.h"
#include "../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/Abilities/GameplayAbility.h"
#include "../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/GameplayEffect.h"
#include "Public/AbilityTypes.h"
#include "UObjectBase.h"

FGameplayAbilityInfo UGameplayAbilityBase::GetAbilityInfo()
{
	UGameplayEffect* CoolDownEffect = GetCooldownGameplayEffect();
	UGameplayEffect* CostEffect = GetCostGameplayEffect();

	
	
	if (CoolDownEffect && CostEffect)
	{
		float CoolDownDuration = 0;
		CoolDownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CoolDownDuration);

		float Cost = 0;
		EAbilityCostType CostType; 

		// the CostEffect is stored in an array
		if (CostEffect->Modifiers.Num() > 0)
		{
			// For now we just get the first element in the array
			FGameplayModifierInfo EffectInfo = CostEffect->Modifiers[0];
			EffectInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);
			
			// Get the attribute - Health, Mana, Strength
			FGameplayAttribute CostAttr = EffectInfo.Attribute;

			// The string name: E.g. "Health, "Mana" etc.
			FString AttributeName = CostAttr.AttributeName;

			// What type are we using for our ability cost
			// Any more than this and I would use a switch statement here.
			// I don't really like the idea of checking the string name to be sure I have the right type. I think I would put this into some consts.
			if (AttributeName == "Health")
			{
				CostType = EAbilityCostType::Health;
			}
			else if (AttributeName == "Mana")
			{
				CostType = EAbilityCostType::Mana;
			}
			else if (AttributeName == "Strength")
			{
				CostType = EAbilityCostType::Strength;
			}

			return FGameplayAbilityInfo(CoolDownDuration, Cost, CostType, UIMaterial, GetClass());
			
		}
	}

	return FGameplayAbilityInfo();
}
