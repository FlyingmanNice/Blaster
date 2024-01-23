// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterCharacter.h"

#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/Weapon/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup CameraBoom
	// 生成Component或者Subobject时用
	this->CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	// Attach到Mesh避免蹲下修改胶囊体大小时导致弹簧臂移动
	this->CameraBoom->SetupAttachment(this->GetMesh());
	this->CameraBoom->TargetArmLength = 600.f;
	this->CameraBoom->bUsePawnControlRotation = true;

	this->FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// Attach到弹簧臂的末端，USpringArmComponent::SocketName就是指弹簧臂的末端Socket
	this->FollowCamera->SetupAttachment(this->CameraBoom, USpringArmComponent::SocketName);
	// FollowCamera跟着弹簧臂走，所以不需要随着Pawn的旋转而旋转
	this->FollowCamera->bUsePawnControlRotation = false;

	// Character不随着Controller旋转, 瞄准时再跟着旋转
	this->bUseControllerRotationYaw = false;
	// 平时跟着移动方向去旋转
	this->GetCharacterMovement()->bOrientRotationToMovement = true;

	this->OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	this->OverheadWidget->SetupAttachment(this->RootComponent);

	this->CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	this->CombatComponent->SetIsReplicated(true);

	// 确保可以蹲下
	this->GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME直接将变量Replicate给所有Client
	//DOREPLIFETIME(ABlasterCharacter, OverlappedWeapon)

	//DOREPLIFETIME_CONDITION则只会Replicate给符合条件的Client
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappedWeapon, ELifetimeCondition::COND_OwnerOnly)
}

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ThisClass::Jump);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ThisClass::LookUp);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ThisClass::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch",IE_Pressed, this, &ThisClass::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim",IE_Pressed, this, &ThisClass::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim",IE_Released, this, &ThisClass::AimButtonReleased);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(this->CombatComponent)
	{
		this->CombatComponent->OwnerCharacter = this;
	}
}

void ABlasterCharacter::MoveForward(float Value)
{
	if(this->Controller!=nullptr&&Value!=0.f)
	{
		// 之所以不用 this->GetActorForwardVector() 来取方向
		// 是因为这个方法取出来的是RootComponent的Forward方向，也就是胶囊体的
		// 但是我们要取的是 Controller的方向，Controller的方向跟RootComponent的方向是不同
		const FRotator YawRotation(0.f, this->GetControlRotation().Yaw, 0.f);
		
		// 用目前Yaw的旋转矩阵来取得 Forward的方向
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		this->AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::MoveRight(float Value)
{
	if(this->Controller!=nullptr&&Value!=0.f)
	{
		const FRotator YawRotation(0.f, this->GetControlRotation().Yaw, 0.f);
		
		// 用目前Yaw的旋转矩阵来取得 Right的方向
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		this->AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::Turn(float Value)
{
	this->AddControllerYawInput(Value);
}

void ABlasterCharacter::LookUp(float Value)
{
	this->AddControllerPitchInput(Value);
}

void ABlasterCharacter::EquipButtonPressed()
{
	// 只有Server可以装备武器
	if(this->CombatComponent)
	{
		if(this->HasAuthority())
		{
			this->CombatComponent->EquipWeapon(this->OverlappedWeapon);
		}
		else
		{
			this->ServerEquipButtonPressed();
		}
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if(this->bIsCrouched)
	{
		this->UnCrouch();
	}
	else
	{
		this->Crouch();
	}
}

void ABlasterCharacter::AimButtonPressed()
{
	if(this->CombatComponent)
	{
		this->CombatComponent->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if(this->CombatComponent)
	{
		this->CombatComponent->SetAiming(false);
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverlappedWeapon)
	{
		OverlappedWeapon->ShowPickupWidget(true);
	}

	if(LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

// RPC的Funciton定义需要加后缀_Implementation，因为得由Server调用而不是普通的Fucntion直接调用
void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if(this->CombatComponent)
	{
		this->CombatComponent->EquipWeapon(this->OverlappedWeapon);
	}
}

void ABlasterCharacter::SetOverlappedWeapon(AWeapon* Weapon)
{
	// 因为只有Server才会绑定Overlap方法，所以能进来的肯定是在Server
	// 所以再判断是不是本地在操作就可以知道是不是Server的本地玩家Overlap了
	if(IsLocallyControlled())
	{
		if(OverlappedWeapon)
		{
			OverlappedWeapon->ShowPickupWidget(false);
		}

		if(Weapon)
		{
			Weapon->ShowPickupWidget(true);
		}
	}

	this->OverlappedWeapon = Weapon;
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return this->CombatComponent && this->CombatComponent->EquippedWeapon != nullptr;
}

bool ABlasterCharacter::IsAiming()
{
	return this->CombatComponent && this->CombatComponent->bAiming;
}





