// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterAnimInstance.h"

#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	// 是否装备了武器
	this->bWeaponEquipped = this->BlasterCharacter->IsWeaponEquipped();
	// 是否在蹲着
	this->bIsCrouched = this->BlasterCharacter->bIsCrouched;
	// 是否在瞄准
	this->bAiming = this->BlasterCharacter->IsAiming();

	// 计算YawOffset，也就是瞄准方向和运动方向的夹角(-180,180]
	// For Strafing
	FRotator AimRotation = this->BlasterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(this->BlasterCharacter->GetVelocity());
	// UKismetMathLibrary::NormalizedDeltaRotator返回的是(-180,180]的夹角
	FRotator TargetDeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation);
	// RInterpTo插值会取最短路径旋转到达目标，比如-179度不会转358度才到179度
	// 原理是RInterpTo会单位化(Target-Current)，也就是如果Target为179，Current为-179，他们的Delta会变成-2，只有2的路程，而不是358，不会经过所有的点
	this->CurrentDeltaRotation = FMath::RInterpTo(this->CurrentDeltaRotation, TargetDeltaRotation, DeltaTime, 6.f);
	this->YawOffset = this->CurrentDeltaRotation.Yaw;
	
	// 计算Lean，用上一帧的旋转与下一帧的旋转夹角(-180,180]
	// For Leaning
	this->CharacterRotationLastFrame = this->CharacterRotation;
	this->CharacterRotation = this->BlasterCharacter->GetActorRotation();
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(this->CharacterRotation,this->CharacterRotationLastFrame);
	const float TargetYaw = DeltaRotation.Yaw / DeltaTime;
	// 插值过去
	const float Interp = FMath::FInterpTo(Lean, TargetYaw, DeltaTime,6.f);
	this->Lean = FMath::Clamp(Interp, -90.f, 90.f);
}
