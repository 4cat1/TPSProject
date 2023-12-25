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
		//Tikc함수가 호출되지 않도록 처리
		PrimaryComponentTick.bCanEverTick = false;
	};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	//사용자 입력 맵핑 처리함수
	virtual void SetupInputbinding(class UInputComponent* PlayerInputComponent){};

public:
	UPROPERTY()
	ATPSPlayer* me;

	UPROPERTY()
	UCharacterMovementComponent* moveComp;
		
};
