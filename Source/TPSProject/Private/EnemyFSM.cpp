// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "HPBar.h"
#include "Enemy.h"
#include "Animation/AnimMontage.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <Components/CapsuleComponent.h>
#include "EnemyAnim.h"
#include <AIController.h>
#include <NavigationSystem.h>

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> damageMontage(TEXT("/Game/Anim/MG_EnemyDamage.MG_EnemyDamage"));
	if (damageMontage.Succeeded()) {
		damageAnim = damageMontage.Object;
	}

	//


	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();
	id = FString::Printf(TEXT("%u"), this);
	int32 seed32 = 0;
	for (auto a : id) {
		seed32 += a;
	}
	UE_LOG(LogTemp, Warning, TEXT("FSM this %s seed32 %d"), *id, seed32);
	FMath::SRandInit(seed32);

	moveSpeed = FMath::RandRange(minSpeed, maxSpeed);
	// ���忡�� ATPSPlayer Ÿ�� ã�ƿ���
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	// ATPSPlayer Ÿ������ ĳ����
	target = Cast<ATPSPlayer>(actor);
	// ������ü ��������
	me = Cast<AEnemy>(GetOwner());

	// UEnemyAnim* �Ҵ�
	anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());


	// AAIController �Ҵ��ϱ�
	ai = Cast<AAIController>(me->GetController());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FString _info;

	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		_info = TEXT("Idle");
		break;
	case EEnemyState::Move:
		MoveState(DeltaTime);
		_info = TEXT("Move");
		break;
	case EEnemyState::Attack:
		AttackState();
		_info = TEXT("Attack");
		break;
	case EEnemyState::Damage:
		DamageState();
		_info = TEXT("Damage");
		break;
	case EEnemyState::Die:
		DieState();
		_info = TEXT("Die");
		break;
	}
	//me->hpBar->cInfo = FText::FromString(_info);
}

// ������
void UEnemyFSM::IdleState()
{
	//1.�ð��� �귶���ϱ�
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2.���� ����ð��� ���ð��� �ʰ��ߴٸ�
	if (currentTime > idleDelayTime)
	{
		//3.�̵� ���·� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Move;
		// ����ð��ʱ�ȭ 
		currentTime = 0;

		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;
		 //���� ������ ��ġ �����ֱ�
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

// �̵�����
void UEnemyFSM::MoveState(float DeltaTime)
{
	// 1.Ÿ�� �������� �ʿ��ϴ�.
	FVector destination = target->GetActorLocation();
	// 2.������ �ʿ��ϴ�.
	FVector dir = destination - me->GetActorLocation();
	// 3.�������� �̵��ϰ� �ʹ�. AI  dldyd
	//ai->MoveToLocation(destination);

	//float _speed = DeltaTime * moveSpeed;
	//3. �������� �̵��ϰ� �ʹ�. �����̿�
	//FRotator _dirRot = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), target->GetActorLocation());

	//me->SetActorRotation(FRotator(0, _dirRot.Yaw, 0));
	////dir = FVector(dir.X, dir.Y, 0).GetSafeNormal();
	////float _speed = dir.Length() * moveSpeed * 0.1;
	//me->AddMovementInput(me->GetActorForwardVector() * moveSpeed * DeltaTime); //* _speed

	// NavigationSystem ��ü ������
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	// ������ ��ã�� ��� ������ �˻�
	FPathFindingQuery query;
	FAIMoveRequest req;
	// ���������� ���� �Ҽ� �ִ� ����
	req.SetAcceptanceRadius(3);
	req.SetGoalLocation(destination);
	// ��ã�⸦ ���� ���� ����
	ai->BuildPathfindingQuery(req, query);
	// ��ã�� ��� ��������
	FPathFindingResult r = ns->FindPathSync(query);

	 //������ ������ ��ã�� �������� Ȯ��
	if (r.Result == ENavigationQueryResult::Success)
	{
		// Ÿ�������� �̵�
		ai->MoveToLocation(destination);
	}
	else
	{
		// ������ġ�� �̵�
		auto result = ai->MoveToLocation(randomPos);
		// �������� �����ϸ�
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			// ���ο� ���� ��ġ ��������
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
		}
	}

	// Ÿ�ٰ� ��������� ���ݻ��·� ��ȯ�ϰ� �ʹ�.
	// 1. ���� �Ÿ��� ���� �����ȿ� ������
	if (dir.Size() < attackRange)
	{
		// ��ã�� ��� ����
			ai->StopMovement();

		// 2. ���ݻ��·� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Attack;
		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;
		// ���� �ִϸ��̼� ��� Ȱ��ȭ
		anim->bAttackPlay = true;
		// ���ݻ��� ��ȯ �� ��� �ð��� �ٷ� �������� ó�� 
		currentTime = attackDelayTime;
		//me->hpBar->cMP -= 9;

	}
	//1.�ð��� �귶���ϱ�
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2.���� ����ð��� ���ð��� �ʰ��ߴٸ�
	if (currentTime > idleDelayTime)
	{
		currentTime = 0;
		me->hpBar->cMP -= 1;
	}
	if (me->hpBar->cMP <= 0) {
		me->hpBar->cMaxMP = maxMp;
		me->hpBar->cHP--;
		if (me->hpBar->cHP <= 0) {
			// ���¸� �������� ��ȯ
			mState = EEnemyState::Die;
			// ĸ�� �浹ü ��Ȱ��ȭ �׾����� �ٴ����� ��������ϴ� ȿ��
			me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			 //���� �ִϸ��̼� ��� ABP_Enemy blend bool���� ó���ϰ� ����.
			anim->PlayDamageAnim(TEXT("Die"));
		}
		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;
	}
}

// ���ݻ���
void UEnemyFSM::AttackState()
{
	//��ǥ: ���� �ð��� �ѹ��� �����ϰ� �ʹ�.
	//1. �ð��� �귯�� �Ѵ�.
	currentTime += GetWorld()->DeltaTimeSeconds;
	////2. ���ݽð��� �����ϱ�
	if (currentTime > attackDelayTime)
	{
		//3. �����ϰ� �ʹ�.
		//PRINT_LOG(TEXT("Attack!!!!!"));
		// ����ð��ʱ�ȭ
		currentTime = 0;
		anim->bAttackPlay = true;
	}

	//��ǥ: Ÿ���� ���ݹ����� ����� ���¸� �̵����� ��ȯ�ϰ� �ʹ�.
	// 1. Ÿ�ٰ��� �Ÿ��� �ʿ��ϴ�.
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	//// 2. Ÿ�ٰ��� �Ÿ��� ���ݹ����� ������ϱ�
	if (distance > attackRange)
	{
		// 3. ���¸� �̵����� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Move;
		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

// �ǰݻ���
void UEnemyFSM::DamageState()
{
	//1.�ð��� �귶���ϱ�
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2.���� ����ð��� ���ð��� �ʰ��ߴٸ�
	if (currentTime > damageDelayTime)
	{
		//3.��� ���·� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Idle;
		// ����ð��ʱ�ȭ 
		currentTime = 0;
		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;
	}
}

// ��������
void UEnemyFSM::DieState()
{
	// ���� ���� �ִϸ��̼��� ������ �ʾҴٸ�
	// �ٴڳ������� �ʵ��� ó��
	if (anim->bDieDone == false)
	{
		return;
	}

	//// ��� �Ʒ��� �������� �ʹ�.
	// ��ӿ ���� P = P0 + vt
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;
	me->SetActorLocation(P);

	//// 1. ���� 2���� �̻� �����Դٸ�
	if (P.Z < -200.0f)
	{
		// 2. ���Ž�Ų��.
		me->Destroy();
	}
}

void UEnemyFSM::SetMoveSpeed(float value)
{
	moveSpeed = value;
}

// �ǰ� �˸� �̺�Ʈ �Լ�
void UEnemyFSM::OnDamageProcess()
{
	// ü�°���
	hp--;
	// ���� ü���� �����ִٸ�
	if (hp > 0 )
	{
		
		// ���¸� �ǰ����� ��ȯ
		mState = EEnemyState::Damage;
		me->hpBar->cHP = hp;
		currentTime = 0;

		// �ǰ� �ִϸ��̼� ���
		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), index);
		anim->PlayDamageAnim(FName(*sectionName));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, *FString::Printf(TEXT("OnDamage%d"), index));
		//section ���� �÷��̴� ���߿� ����
		//if (!anim->Montage_IsPlaying(damageAnim)) {
		//	anim->Montage_Play(damageAnim);
		//	//���߿� �޺��� ������ ����
		//	anim->Montage_JumpToSection(TEXT("damage1"), damageAnim);
		//}
	}
	// �׷��� �ʴٸ�
	else
	{
		// ���¸� �������� ��ȯ
		mState = EEnemyState::Die;
		// ĸ�� �浹ü ��Ȱ��ȭ �׾����� �ٴ����� ��������ϴ� ȿ��
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// ���� �ִϸ��̼� ��� ABP_Enemy blend bool���� ó���ϰ� ����.
		anim->PlayDamageAnim(TEXT("Die"));
	}
	// �ִϸ��̼� ���� ����ȭ
	anim->animState = mState;

	ai->StopMovement();
}

// ������ġ ��������
bool UEnemyFSM::GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation loc;

	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);
	dest = loc.Location;
	FString _resultStr = (result) ? TEXT("True") : TEXT("Fail");
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, *FString::Printf(TEXT("GetRandomPositionNav Result %s"), *_resultStr));
	return result;
}

