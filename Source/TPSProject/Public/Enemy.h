// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class TPSPROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	class USphereComponent* sphereComp;
public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	// �� AI ���� ������Ʈ Ŭ����
	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ATPSPlayer* target;

	// ��������
	UPROPERTY()
	class AEnemy* me;
	// �� AI ���� ������Ʈ Ŭ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSMComponent)
	class UEnemyFSM* fsm;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UHPBar> widgetRef;
	UPROPERTY()
	class UHPBar* hpBar;
	UPROPERTY()
	class UWidgetComponent* widgetComp;

};

