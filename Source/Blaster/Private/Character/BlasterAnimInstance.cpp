// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterAnimInstance.h"

#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 获取BlasterCharacter
	this->BlasterCharacter = Cast<ABlasterCharacter>(this->TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if(this->BlasterCharacter == nullptr)
	{
		this->BlasterCharacter = Cast<ABlasterCharacter>(this->TryGetPawnOwner());
	}

	if(this->BlasterCharacter == nullptr)
	{
		return;
	}

	// 获取速度
	FVector Velocity = this->BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	this->Speed = Velocity.Size();

	UCharacterMovementComponent* CharacterMovementComponent = this->BlasterCharacter->GetCharacterMovement();
	// 是否在空中
	this->bIsInAir = CharacterMovementComponent->IsFalling();
	// 是否在加速
	this->bIsAccelerating = CharacterMovementComponent->GetCurrentAcceleration().Size() > 0;
}
