// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Components/SphereComponent.h"

// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	// bReplicates为true，则这个武器会复制到服务器上
	// Pawn会自动设置bReplicates为true
	this->bReplicates = true;

	this->WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	this->WeaponMesh->SetupAttachment(this->RootComponent);
	this->SetRootComponent(this->WeaponMesh);

	// Block所有的Channel
	this->WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	// 然后Ignore Pawn
	this->WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// 一开始没有碰撞
	this->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 设置拾取的碰撞框，初始为关闭，如果是Server才打开
	this->AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	this->AreaSphere->SetupAttachment(this->RootComponent);
	this->AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//判定是否在服务器
	// this->HasAuthority() 等于 this->GetLocalRole() == ENetRole::ROLE_Authority
	if(this->HasAuthority())
	{
		this->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		this->AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

