// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "TPSProject.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PRINT_LOG(TEXT("%s"), TEXT("TPSPlayer Loading"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));
	if (tempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
	// 3. TPS 카메라를 붙이고 싶다.
// 3-1. SpringArm 컴포넌트 붙이기
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	springArmComp->TargetArmLength = 400;
	springArmComp->bUsePawnControlRotation = true;
	// 3-2. Camera 컴포넌트 붙이기
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	tpsCamComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;
	// 2단 점프
	JumpMaxCount = 2;
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem
			= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Locomotion();
}


// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveIA, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);
		EnhancedInputComponent->BindAction(LookUpIA, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);
		EnhancedInputComponent->BindAction(JumpIA, ETriggerEvent::Triggered, this, &ATPSPlayer::InputJump);
		EnhancedInputComponent->BindAction(FireIA, ETriggerEvent::Triggered, this, &ATPSPlayer::InputFire);
	}
}
void ATPSPlayer::Locomotion()
{
	moveDirection = FTransform(GetControlRotation()).TransformVector(moveDirection);
	AddMovementInput(moveDirection);
	moveDirection = FVector::ZeroVector; //ZeroVector; == FVector(0,0,0)
}
void ATPSPlayer::Move(const FInputActionValue& Value)
{
	const FVector _currentValue = Value.Get<FVector>();
	//PRINT_LOG(TEXT("%f %f %f"), _currentValue.X, _currentValue.Y, _currentValue.Z);
	if (Controller)
	{
		moveDirection.Y = _currentValue.X;
		moveDirection.X = _currentValue.Y;
	}
}

void ATPSPlayer::LookUp(const FInputActionValue& Value)
{
	const FVector _currentValue = Value.Get<FVector>();
	//PRINT_LOG(TEXT("%f %f %f"), _currentValue.X, _currentValue.Y, _currentValue.Z);
	AddControllerPitchInput(_currentValue.Y);
	AddControllerYawInput(_currentValue.X);
}


void ATPSPlayer::InputJump(const FInputActionValue& Value)
{
	Jump();
}

void ATPSPlayer::InputFire(const FInputActionValue& Value)
{
	//if (fireReady)
	//{
	//	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	//	if (AnimInstance)
	//	{
	//		AnimInstance->Montage_Play(attackAnimMontage);
	//	}

	//	fireReady = false;
	//}
}