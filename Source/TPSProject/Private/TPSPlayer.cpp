// Fill out your copyright notice in the Description page of Project Settings.
#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Bullet.h"
#include "MyUserWidget.h"
//#include "Sound/SoundBase.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "PlayerAnim.h"
#include "EnemyFSM.h"
//#include "PlayerMove.h"
#include "TPSProject.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 1. 스켈레탈메시 데이터를 불러오고 싶다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin"));
	if (tempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		// 2. Mesh 컴포넌트의 위치와 회전 값을 설정하고 싶다.
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
	// 4. 총 스켈레탈메시 컴포넌트 등록
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	// 4-1. 부모 컴포넌트를 Mesh 컴포넌트로 설정
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	// 4-2. 스켈레탈메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	// 4-3. 데이터로드가 성공했다면
	if (TempGunMesh.Succeeded())
	{
		// 4-4. 스켈레탈메시 데이터 할당
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		// 4-5. 위치 조정하기
		gunMeshComp->SetRelativeLocation(FVector(-17, 10, -3));
		// 회전 조정하기
		gunMeshComp->SetRelativeRotation(FRotator(0, 90, 0));
	}

	// 5. 스나이퍼건 컴포넌트 등록
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	// 5-1. 부모 컴포넌트를 Mesh 컴포넌트로 설정
	sniperGunComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	// 5-2. 스태틱메시 데이터 로드
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	// 5-3. 데이터로드가 성공했다면
	if (TempSniperMesh.Succeeded())
	{
		// 5-4. 스태틱메시 데이터 할당
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		// 5-5. 위치 조정하기
		sniperGunComp->SetRelativeLocation(FVector(-42, 7, 1));
		// 회전 조정하기
		sniperGunComp->SetRelativeRotation(FRotator(0, 90, 0));
		// 5-6. 크기 조정하기
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Game/SniperGun/Rifle.Rifle"));
	if (tempSound.Succeeded()) {
		bulletSound = tempSound.Object; 
	}
	//playerMove = CreateDefaultSubobject<UPlayerMove>(TEXT("PlayerMove"));
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	//초기 속도를 걷기로 설정
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem
			= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}

	// 3. 일반 조준 UI 등록
	_sniperUI->AddToViewport();
	_sniperUI->SetVisibility(ESlateVisibility::Hidden);
	_crosshairUI->AddToViewport();
	_crosshairUI->SetVisibility(ESlateVisibility::Hidden);


	// 기본으로 스나이퍼건을 사용하도록 설정
	ChangeToSniperGun();
}

void ATPSPlayer::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	// 1. 스나이퍼 UI 위젯 인스턴스 생성, UMyWidget으로 캐스팅해줘야함
	_sniperUI = Cast< UMyUserWidget>(CreateWidget(GetWorld(), sniperUIFactory));
	// 2. 일반 조준 UI 크로스헤어 인스턴스 생성,  UMyWidget으로 캐스팅해줘야함
	_crosshairUI = Cast< UMyUserWidget>(CreateWidget(GetWorld(), crosshairUIFactory));
}

void ATPSPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Locomotion();
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
		EnhancedInputComponent->BindAction(RunIA, ETriggerEvent::Started, this, &ATPSPlayer::InputRun);
		EnhancedInputComponent->BindAction(RunIA, ETriggerEvent::Completed, this, &ATPSPlayer::InputRun);
		EnhancedInputComponent->BindAction(FireIA, ETriggerEvent::Triggered, this, &ATPSPlayer::InputFire);
		EnhancedInputComponent->BindAction(IA_1, ETriggerEvent::Triggered, this, &ATPSPlayer::ChangeToGrenadeGun);
		EnhancedInputComponent->BindAction(IA_2, ETriggerEvent::Triggered, this, &ATPSPlayer::ChangeToSniperGun);
		EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Started, this, &ATPSPlayer::SniperAim);
		EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Completed, this, &ATPSPlayer::SniperAim);
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
		//From Locomotion
		moveDirection = FTransform(GetControlRotation()).TransformVector(moveDirection);
		AddMovementInput(moveDirection);
		moveDirection = FVector::ZeroVector; //ZeroVector; == FVector(0,0,0)
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

	auto anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if (anim->isFire) {
		return;
	}
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);
	//카메라셰이크 재생
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);
	//공격 애니메이션 재생
	anim->PlayAttackAnim();  // 공격 Montage를 플레이한다.
	if (bUsingGrenadeGun)
	{
		// 총알 발사 처리
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	// 스나이퍼건 사용 시
	else
	{
		// LineTrace 의 시작 위치
		FVector startPos = tpsCamComp->GetComponentLocation();
		// LineTrace 의 종료 위치
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;
		// LineTrace 의 충돌 정보를 담을 변수
		FHitResult hitInfo;
		// 충돌 옵션 설정 변수
		FCollisionQueryParams params;
		// 자기 자신(플레이어) 는 충돌에서 제외
		params.AddIgnoredActor(this);
		// Channel 필터를 이용한 LineTrace 충돌 검출(충돌 정보, 시작 위치, 종료 위치, 검출 채널, 충돌 옵션)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos,
			ECC_Visibility, params);
		// LineTrace가 부딪혔을 때
		if (bHit)
		{
			// 충돌 처리 -> 총알 파편 효과 재생
			// 총알 파편 효과 트랜스폼

			FTransform bulletTrans;
			// 부딪힌 위치 할당
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			// 총알 파편 효과 인스턴스 생성
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			auto hitComp = hitInfo.GetComponent();
			// 1. 만약 컴포넌트에 물리가 적용되어 있다면
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Hit"));
				// 2. 날려버릴 힘과 방향이 필요
				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 5000;
				// 3. 그 방향으로 날려버리고 싶다.
				hitComp->AddForce(force);
			}

			// 부딪힌 대상이 적인지 판단하기
			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy)
			{
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamageProcess();
			}
		}
	}
}

// 유탄총으로 변경
void ATPSPlayer::ChangeToGrenadeGun()
{
	// 유탄총 사용 중으로 체크
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}
// 스나이퍼건으로 변경
void ATPSPlayer::ChangeToSniperGun()
{
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}

void ATPSPlayer::SniperAim()
{
	// 스나이퍼건 모드가 아니라면 처리하지 않는다.
	if (bUsingGrenadeGun)
	{
		return;
	}
	// Pressed 입력 처리
	if (bSniperAim == false)
	{
		// 1. 스나이퍼 조준 모드 활성화
		bSniperAim = true;
		// 4. 일반 조준 UI 제거
		//_crosshairUI->RemoveFromParent();
		_crosshairUI->SetVisibility(ESlateVisibility::Hidden);
		// 2. 스나이퍼조준 UI 등록
		_sniperUI->SetVisibility(ESlateVisibility::Visible);
		//_sniperUI->AddToViewport();
		// 3. 카메라의 시야각 Field Of View 설정
		tpsCamComp->SetFieldOfView(45.0f);

	}
	// Released 입력 처리
	else
	{
		// 1. 스나이퍼 조준 모드 비활성화
		bSniperAim = false;
		// 2. 스나이퍼 조준 UI 화면에서 제거
		//_sniperUI->RemoveFromParent();
		_sniperUI->SetVisibility(ESlateVisibility::Hidden);
		// 3. 카메라 시야각 원래대로 복원
		tpsCamComp->SetFieldOfView(90.0f);
		// 4. 일반 조준 UI 등록
		_crosshairUI->SetVisibility(ESlateVisibility::Visible);
		//_crosshairUI->AddToViewport();
	}
}

void ATPSPlayer::InputRun()
{
	auto movement = GetCharacterMovement();
	if (movement->MaxWalkSpeed > walkSpeed) {
		movement->MaxWalkSpeed = walkSpeed;
	}
	else {
		movement->MaxWalkSpeed = runSpeed;
	}
}
