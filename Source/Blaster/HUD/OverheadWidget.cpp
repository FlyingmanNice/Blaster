// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"

#include "Components/TextBlock.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if(DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	// GetlocalRole()时，Client显示的是Autonomous Proxy和Simulated Proxy，Server显示的都是Authority
	// GetRemoteRole()时则相反，Client显示的是Authority，Server显示的是Autonomous Proxy和Simulated Proxy
	ENetRole LocalRole = InPawn->GetLocalRole();
	FString Role;
	switch (LocalRole)
	{
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	default:
		Role = FString("None");
		break;
	}

	FString LocalRoleString = FString::Printf(TEXT("Local Role: %s"), *Role);
	this->SetDisplayText(LocalRoleString);
}

void UOverheadWidget::NativeDestruct()
{
	this->RemoveFromParent();
	Super::NativeDestruct();
}
