// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "TPSPlayer.generated.h"

UCLASS()
class TPSPROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* tpsCamComp;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveIA;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookUpIA;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpIA;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* FireIA;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_1;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_2;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Aim;
public:
	void Move(const FInputActionValue& Value);
	void LookUp(const FInputActionValue& Value);
	//void Turn(const FInputActionValue& Value);
	void InputJump(const FInputActionValue& Value);
	void InputFire(const FInputActionValue& Value); 	// �Ѿ� �߻� ó�� �Լ�

	// �� ���̷�Ż�޽�
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

	// �Ѿ� ����
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	// �������۰� ����ƽ�޽� �߰�
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class UStaticMeshComponent* sniperGunComp;

	// ��ź�� ��� ������ ����
	bool bUsingGrenadeGun = true;
	// ��ź������ ����
	void ChangeToGrenadeGun();
	// �������۰����� ����
	void ChangeToSniperGun();

	// �������� ����
	void SniperAim();
	// �������� ���� ������ ����
	bool bSniperAim = false;

	// �������� UI ���� ����
	UPROPERTY(EditDefaultsOnly, Category = Widget)
	TSubclassOf<class UMyUserWidget> sniperUIFactory;
	// �������� UI ���� �ν��Ͻ�
	class UMyUserWidget* _sniperUI;
	// �Ϲ� ���� ũ�ν����UI ����
	UPROPERTY(EditDefaultsOnly, Category = Widget)
	TSubclassOf<class UMyUserWidget> crosshairUIFactory;
	// ũ�ν���� �ν��Ͻ�
	class UMyUserWidget* _crosshairUI;

	// �Ѿ� ���� ȿ�� ����
	UPROPERTY(EditAnywhere, Category = BulletEffect)
	class UParticleSystem* bulletEffectFactory;


private:
	void Locomotion();

	FVector moveDirection;
	bool fireReady;
	float fireTimerTime;
};
