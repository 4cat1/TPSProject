// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Components/ActorComponent.h"
#include "PlayerBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPROJECT_API UPlayerBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerBaseComponent() {
		//Tikc�Լ��� ȣ����� �ʵ��� ó��
		PrimaryComponentTick.bCanEverTick = false;
	};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	//����� �Է� ���� ó���Լ�
	virtual void SetupInputbinding(class UInputComponent* PlayerInputComponent){};

public:
	UPROPERTY()
	ATPSPlayer* me;

	UPROPERTY()
	UCharacterMovementComponent* moveComp;
		
};
