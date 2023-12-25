// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include <Components/CapsuleComponent.h>
#include <Components/SphereComponent.h>
#include <Components/WidgetComponent.h>
#include "Components/widget.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "HPBar.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. ���̷�Ż�޽� ������ �ε�
	//
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/Enemy/Model/vampire_a_lusth.vampire_a_lusth'"));
	// 1-1. ������ �ε� �����ϸ�
	if (tempMesh.Succeeded())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Enemy Skeletal Mesh Loading"));
		// 1-2. ������ �Ҵ�
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		// 1-3. �޽� ��ġ �� ȸ�� ����
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
		// 1-4. �޽� ũ�� ����
		GetMesh()->SetRelativeScale3D(FVector(0.84f));
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	//sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collosion"));
	//sphereComp->SetRelativeScale3D(FVector(3, 3, 3));
	//sphereComp->SetCollisionProfileName(TEXT("EnemyHand"));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	// EnemyFSM ������Ʈ �߰�
	fsm = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));
	if (fsm) {
		UE_LOG(LogTemp, Warning, TEXT("fsm"));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf( TEXT("FSM: ") );// , fsm->GetFName()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,  TEXT("FSM:") );
	// �ִϸ��̼� �������Ʈ �Ҵ��ϱ�
	}
	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Game/Anim/ABP_Enemy.ABP_Enemy_C"));
	if (tempClass.Succeeded())
	{
		//UE_LOG(LogTemp, Warning, TEXT("ABP_Enemy Loading"));
		//GetMesh()->SetAnim
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}
	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPWidget"));
	widgetComp->SetupAttachment(GetMesh());
	widgetComp->SetRelativeLocation(FVector(0, 0, 200.0f));
	UClass* WidgetCompClass = LoadClass<UUserWidget>(NULL, TEXT("/Game/UI/WBP_HPEnemy.WBP_HPEnemy_C"));
	if (WidgetCompClass) {
		widgetComp->SetWidgetClass(WidgetCompClass);
		widgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		widgetComp->SetDrawSize(FVector2D(230, 75));
		widgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	// ATPSPlayer Ÿ������ ĳ����
	target = Cast<ATPSPlayer>(actor);
	// ������ü ��������
	//me = Cast<ANPC>(GetOwner());
	me = this;
	widgetComp->InitWidget();
	hpBar = Cast<UHPBar>(widgetComp->GetUserWidgetObject());
	if (IsValid(hpBar)) {
		//FText idText = FText::FromString(FString::FromInt(12));
		FVector _pos = target->GetActorLocation();
		hpBar->ctextId = GetClass()->GetDisplayNameText();
		hpBar->cInfo = FText::FromString(FString::Printf(TEXT("IDLE")));
		//hpBar->ctextPos = FText::FromString(FString::Printf(TEXT("%f %f %f"), _pos.X, _pos.Y, _pos.Z));
		hpBar->cHP = me->fsm->maxHp;
		hpBar->cMaxHP = me->fsm->maxHp;
		hpBar->cMP = me->fsm->maxMp;
		hpBar->cMaxMP = me->fsm->maxMp;

		/*widget->SetTextID(GetClass()->GetDisplayNameText());
		FVector _pos = target->GetActorLocation();
		widget->SetTextPosition(FText::FromString(FString::Printf(TEXT("%f %f %f"), _pos.X, _pos.Y, _pos.Z)));
		*/
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

