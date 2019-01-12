// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/CharacterBase.h"
#include "../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemComponent.h"
#include "../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/Abilities/GameplayAbility.h"
#include "../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/GameplayAbilitySpec.h"
#include "Public/AttributeSetBase.h"
#include "AIController.h"
#include "GameFramework/PlayerController.h"
#include "AIModule/Classes/BrainComponent.h"
#include "GameFramework/Actor.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AttributeSetBaseComp = CreateDefaultSubobject<UAttributeSetBase>(TEXT("AttributeSetBaseComp"));

	bIsDead = false;

	TeamID = 255;

}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	AttributeSetBaseComp->OnHealthChange.AddDynamic(this, &ACharacterBase::OnHealthChanged);
	AttributeSetBaseComp->OnManaChange.AddDynamic(this, &ACharacterBase::OnManaChanged);
	AttributeSetBaseComp->OnStrengthChange.AddDynamic(this, &ACharacterBase::OnStrengthChanged);

	AutoDetermineTeamIDbyControllerType();

	AddGameplayTag(FullHealthTag);
	
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComp;
}

void ACharacterBase::AquireAbility(TSubclassOf<UGameplayAbility> AbilityToAquire)
{
	if (AbilitySystemComp)
	{
		if (HasAuthority() && AbilityToAquire)
		{
			/* 4.19 implementation */
			//FGameplayAbilitySpecDef SpecDef = FGameplayAbilitySpecDef();
			//SpecDef.Ability = AbilityToAcquire;
			//FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(SpecDef, 1);

			// or could be
			//AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(AbilityToAquire, 1, 0));

			AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(AbilityToAquire));
		}

		AbilitySystemComp->InitAbilityActorInfo(this, this);
	}
}

void ACharacterBase::OnHealthChanged(float Health, float MaxHealth)
{
	if (Health <= 0.0f && !bIsDead)
	{
		bIsDead = true;
		Dead();
		BP_Die();
	}

	BP_OnHealthChanged(Health, MaxHealth);
}

void ACharacterBase::OnManaChanged(float Mana, float MaxMana)
{
	BP_OnManaChanged(Mana, MaxMana);
}

void ACharacterBase::OnStrengthChanged(float Strength, float MaxStrength)
{
	BP_OnStrengthChanged(Strength, MaxStrength);
}

bool ACharacterBase::IsOtherHostile(ACharacterBase* Other)
{
	return TeamID != Other->GetTeamID();
}

uint8 ACharacterBase::GetTeamID() const
{
	return TeamID;
}


void ACharacterBase::AddGameplayTag(FGameplayTag& TagToAdd)
{
	GetAbilitySystemComponent()->AddLooseGameplayTag(TagToAdd);

	// clamp the tag count to 1
	GetAbilitySystemComponent()->SetTagMapCount(TagToAdd, 1);
}

void ACharacterBase::RemoveGameplayTag(FGameplayTag& TagToRemove)
{
	GetAbilitySystemComponent()->RemoveLooseGameplayTag(TagToRemove);
}




void ACharacterBase::AutoDetermineTeamIDbyControllerType()
{
	if (GetController() && GetController()->IsPlayerController())
	{
		TeamID = 0;
	}
}

void ACharacterBase::Dead()
{
	DisableInputControl();
}

void ACharacterBase::DisableInputControl()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->DisableInput(PC);
	}

	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC)
	{
		AIC->GetBrainComponent()->StopLogic("Dead");
	}
}

void ACharacterBase::EnableInputControl()
{
	if (!bIsDead)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			PC->EnableInput(PC);
		}

		AAIController* AIC = Cast<AAIController>(GetController());
		if (AIC)
		{
			AIC->GetBrainComponent()->RestartLogic();
		}
	}
	
}

void ACharacterBase::HitStun(float StunDuration)
{
	DisableInputControl();

	GetWorldTimerManager().SetTimer(StunTimeHandle, this, &ACharacterBase::EnableInputControl, StunDuration, false);
}

