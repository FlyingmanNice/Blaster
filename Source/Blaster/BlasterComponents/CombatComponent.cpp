// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Blaster/Weapon/Weapon.h"
#include "Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	this->bAiming = bIsAiming;
	// if(!this->OwnerCharacter->HasAuthority())
	// {
	// 在Client里执行RPC，会自动在Server上执行
		this->ServerSetAiming(bIsAiming);
	//}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if(this->EquippedWeapon && this->OwnerCharacter)
	{
		//装备武器时不能跟着移动旋转, 而是根据瞄准方向旋转
		this->OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		this->OwnerCharacter->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	this->bAiming = bIsAiming;
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(this->OwnerCharacter == nullptr || WeaponToEquip == nullptr)
	{
		return;
	}

	this->EquippedWeapon = WeaponToEquip;
	this->EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	if(const USkeletalMeshSocket* HandSocket = this->OwnerCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket")))
	{
		HandSocket->AttachActor(EquippedWeapon, this->OwnerCharacter->GetMesh());
	}

	this->EquippedWeapon->SetOwner(this->OwnerCharacter);
	this->OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	this->OwnerCharacter->bUseControllerRotationYaw = true;
}

