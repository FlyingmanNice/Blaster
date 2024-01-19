// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

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

	this->PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	this->PickupWidget->SetupAttachment(this->RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// 判定是否在服务器
	// this->HasAuthority() 等于 this->GetLocalRole() == ENetRole::ROLE_Authority
	if(this->HasAuthority())
	{
		this->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		this->AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		// 绑定Overlap事件
		this->AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);
		this->AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	}

	if(this->PickupWidget)
	{
		this->PickupWidget->SetVisibility(false);
	}
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter)
	{
		BlasterCharacter->SetOverlappedWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter)
	{
		BlasterCharacter->SetOverlappedWeapon(nullptr);
	}
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if(this->PickupWidget)
	{
		this->PickupWidget->SetVisibility(bShowWidget);
	}
}

