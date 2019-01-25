// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageExecutionCalculation.h"
#include "AttributeSetBase.h"
#include "GameplayEffectTypes.h"
#include "UnrealMathUtility.h"

struct DamageStatics
{
	// Using this macro is the same as the code we did in the code below - see the code at the bottom of GameplayEffectExecutionCalculation.h
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)

	DamageStatics()
	{
		// DEFINE_ATTRIBUTE_CAPTUREDEF(S, P, T, B)
		// S = class, P = Property, T = target, B = bool InSnapshot parameter of FGameplayEffectAttributeCaptureDefinition
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetBase, AttackDamage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetBase, Armor, Target, true);
	}
};

// create a static function to get a copy of the struct (DamageStatics)
// Then use in the call below
static DamageStatics& GetDamageStatics()
{
	static DamageStatics DamageStaticsVar;
	return DamageStaticsVar;
}

UDamageExecutionCalculation::UDamageExecutionCalculation()
{
	HealthProperty = FindFieldChecked<UProperty>(UAttributeSetBase::StaticClass(), GET_MEMBER_NAME_CHECKED(UAttributeSetBase, Health));
	HealthDef = FGameplayEffectAttributeCaptureDefinition(HealthProperty, EGameplayEffectAttributeCaptureSource::Target, true);

	// Can be done this way
	RelevantAttributesToCapture.Add(HealthDef);
	// Or this way with the static struct
	RelevantAttributesToCapture.Add(GetDamageStatics().AttackDamageDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().ArmorDef);
}

void UDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	float AttackDamageMagnitude = 0.0f;
	float ArmorMagnitude = 0.0f;

	// The sources attack damage value with be assigned to AttackDamageMagnitude after this executes
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().AttackDamageDef, FAggregatorEvaluateParameters(), AttackDamageMagnitude);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().ArmorDef, FAggregatorEvaluateParameters(), ArmorMagnitude);

	// Now we can do calculations
	// Based on defaults in the AttributeSetBase ctr we will get 20 - 5 = 15 damage.
	float FinalDamage = FMath::Clamp(AttackDamageMagnitude - ArmorMagnitude, 0.0f, AttackDamageMagnitude - ArmorMagnitude);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(HealthProperty, EGameplayModOp::Additive, -FinalDamage));
}
