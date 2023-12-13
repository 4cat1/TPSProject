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
	void InputFire(const FInputActionValue& Value); 	// 총알 발사 처리 함수

	// 총 스켈레탈메시
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

	// 총알 공장
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	// 스나이퍼건 스태틱메시 추가
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class UStaticMeshComponent* sniperGunComp;

	// 유탄총 사용 중인지 여부
	bool bUsingGrenadeGun = true;
	// 유탄총으로 변경
	void ChangeToGrenadeGun();
	// 스나이퍼건으로 변경
	void ChangeToSniperGun();

	// 스나이퍼 조준
	void SniperAim();
	// 스나이퍼 조준 중인지 여부
	bool bSniperAim = false;

	// 스나이퍼 UI 위젯 공장
	UPROPERTY(EditDefaultsOnly, Category = Widget)
	TSubclassOf<class UMyUserWidget> sniperUIFactory;
	// 스나이퍼 UI 위젯 인스턴스
	class UMyUserWidget* _sniperUI;
	// 일반 조준 크로스헤어UI 위젯
	UPROPERTY(EditDefaultsOnly, Category = Widget)
	TSubclassOf<class UMyUserWidget> crosshairUIFactory;
	// 크로스헤어 인스턴스
	class UMyUserWidget* _crosshairUI;

	// 총알 파편 효과 공장
	UPROPERTY(EditAnywhere, Category = BulletEffect)
	class UParticleSystem* bulletEffectFactory;


private:
	void Locomotion();

	FVector moveDirection;
	bool fireReady;
	float fireTimerTime;
};
