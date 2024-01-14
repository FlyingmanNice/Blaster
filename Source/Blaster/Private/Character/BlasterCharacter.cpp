// Fill out your copyright notice in the Description page of Project Settings.


#include "Blaster/Public/Character/BlasterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
}

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
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

// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



