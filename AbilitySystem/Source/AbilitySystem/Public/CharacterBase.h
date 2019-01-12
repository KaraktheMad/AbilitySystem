// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
//#include "../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemComponent.h"
//#include "GameplayAbilities/Public/AbilitySystemComponent.h"
#include "CharacterBase.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UAttributeSetBase;
struct FGameplayTag;

UCLASS()
class ABILITYSYSTEM_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	UAbilitySystemComponent* AbilitySystemComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	UAttributeSetBase* AttributeSetBaseComp;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void AquireAbility(TSubclassOf<UGameplayAbility> AbilityToAquire);


	// Health
	UFUNCTION()
	void OnHealthChanged(float Health, float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase", meta = (DisplayName = "OnHealthChanged"))
	void BP_OnHealthChanged(float Health, float MaxHealth);


	// Mana
	UFUNCTION()
	void OnManaChanged(float Mana, float MaxMana);

	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase", meta = (DisplayName = "OnManaChanged"))
	void BP_OnManaChanged(float Mana, float MaxMana);


	// Strength
	UFUNCTION()
	void OnStrengthChanged(float Strength, float MaxStrength);

	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase", meta = (DisplayName = "OnStrengthChanged"))
	void BP_OnStrengthChanged(float Strength, float MaxStrength);


	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase", meta = (DisplayName = "Die"))
	void BP_Die();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	bool IsOtherHostile(ACharacterBase* Other);

	uint8 GetTeamID() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void AddGameplayTag(FGameplayTag& TagToAdd);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void RemoveGameplayTag(FGameplayTag& TagToRemove);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	FGameplayTag FullHealthTag;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void HitStun(float StunDuration);

protected:

	bool bIsDead;

	uint8 TeamID;

	void AutoDetermineTeamIDbyControllerType();

	void Dead();

	void DisableInputControl();

	void EnableInputControl();

	FTimerHandle StunTimeHandle;

};
