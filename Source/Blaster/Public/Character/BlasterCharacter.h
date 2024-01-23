// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// 继承自AActor，用于定义需要Replicate的Properties
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override; 
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	//EditAnywhere是指在Instance也可以Edit
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget; 

	// GetLifetimeReplicatedProps和标签定义了之后，只要Server里这个变量有变化，就会Replicate给所有的Client
	// UPROPERTY(Replicated)
	// class AWeapon* OverlappedWeapon;
	
	// 定义了变量发生Replicate时的回调（Client的回调）
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappedWeapon;

	// 回调只能有一个参数，是修改前的变量
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* CombatComponent;

	// Server表示这是个RPC Function
	// Reliable表示这是个可靠的RPC，如果Server没有回应会一直发，所以避免在Tick里使用
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	
public:
	void SetOverlappedWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
};
