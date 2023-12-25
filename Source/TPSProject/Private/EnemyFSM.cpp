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
	// 월드에서 ATPSPlayer 타겟 찾아오기
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	// ATPSPlayer 타입으로 캐스팅
	target = Cast<ATPSPlayer>(actor);
	// 소유객체 가져오기
	me = Cast<AEnemy>(GetOwner());

	// UEnemyAnim* 할당
	anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());


	// AAIController 할당하기
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

// 대기상태
void UEnemyFSM::IdleState()
{
	//1.시간이 흘렀으니까
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2.만약 경과시간이 대기시간을 초과했다면
	if (currentTime > idleDelayTime)
	{
		//3.이동 상태로 전환하고 싶다.
		mState = EEnemyState::Move;
		// 경과시간초기화 
		currentTime = 0;

		// 애니메이션 상태 동기화
		anim->animState = mState;
		 //최초 랜덤한 위치 정해주기
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

// 이동상태
void UEnemyFSM::MoveState(float DeltaTime)
{
	// 1.타겟 목적지가 필요하다.
	FVector destination = target->GetActorLocation();
	// 2.방향이 필요하다.
	FVector dir = destination - me->GetActorLocation();
	// 3.방향으로 이동하고 싶다. AI  dldyd
	//ai->MoveToLocation(destination);

	//float _speed = DeltaTime * moveSpeed;
	//3. 방향으로 이동하고 싶다. 벡터이용
	//FRotator _dirRot = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), target->GetActorLocation());

	//me->SetActorRotation(FRotator(0, _dirRot.Yaw, 0));
	////dir = FVector(dir.X, dir.Y, 0).GetSafeNormal();
	////float _speed = dir.Length() * moveSpeed * 0.1;
	//me->AddMovementInput(me->GetActorForwardVector() * moveSpeed * DeltaTime); //* _speed

	// NavigationSystem 객체 얻어오기
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	// 목적지 길찾기 경로 데이터 검색
	FPathFindingQuery query;
	FAIMoveRequest req;
	// 목적지에서 인지 할수 있는 범위
	req.SetAcceptanceRadius(3);
	req.SetGoalLocation(destination);
	// 길찾기를 위한 쿼리 생성
	ai->BuildPathfindingQuery(req, query);
	// 길찾기 결과 가져오기
	FPathFindingResult r = ns->FindPathSync(query);

	 //목적지 까지의 길찾기 성공여부 확인
	if (r.Result == ENavigationQueryResult::Success)
	{
		// 타겟쪽으로 이동
		ai->MoveToLocation(destination);
	}
	else
	{
		// 랜덤위치로 이동
		auto result = ai->MoveToLocation(randomPos);
		// 목적지에 도착하면
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			// 새로운 랜덤 위치 가져오기
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
		}
	}

	// 타겟과 가까워지면 공격상태로 전환하고 싶다.
	// 1. 만약 거리가 공격 범위안에 들어오면
	if (dir.Size() < attackRange)
	{
		// 길찾기 기능 정지
			ai->StopMovement();

		// 2. 공격상태로 전환하고 싶다.
		mState = EEnemyState::Attack;
		// 애니메이션 상태 동기화
		anim->animState = mState;
		// 공격 애니메이션 재생 활성화
		anim->bAttackPlay = true;
		// 공격상태 전환 시 대기 시간이 바로 끝나도록 처리 
		currentTime = attackDelayTime;
		//me->hpBar->cMP -= 9;

	}
	//1.시간이 흘렀으니까
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2.만약 경과시간이 대기시간을 초과했다면
	if (currentTime > idleDelayTime)
	{
		currentTime = 0;
		me->hpBar->cMP -= 1;
	}
	if (me->hpBar->cMP <= 0) {
		me->hpBar->cMaxMP = maxMp;
		me->hpBar->cHP--;
		if (me->hpBar->cHP <= 0) {
			// 상태를 죽음으로 전환
			mState = EEnemyState::Die;
			// 캡슐 충돌체 비활성화 죽었을때 바닥으로 사라지게하는 효과
			me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			 //죽음 애니메이션 재생 ABP_Enemy blend bool에서 처리하고 있음.
			anim->PlayDamageAnim(TEXT("Die"));
		}
		// 애니메이션 상태 동기화
		anim->animState = mState;
	}
}

// 공격상태
void UEnemyFSM::AttackState()
{
	//목표: 일정 시간에 한번씩 공격하고 싶다.
	//1. 시간이 흘러야 한다.
	currentTime += GetWorld()->DeltaTimeSeconds;
	////2. 공격시간이 됐으니까
	if (currentTime > attackDelayTime)
	{
		//3. 공격하고 싶다.
		//PRINT_LOG(TEXT("Attack!!!!!"));
		// 경과시간초기화
		currentTime = 0;
		anim->bAttackPlay = true;
	}

	//목표: 타겟이 공격범위를 벗어나면 상태를 이동으로 전환하고 싶다.
	// 1. 타겟과의 거리가 필요하다.
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	//// 2. 타겟과의 거리가 공격범위를 벗어났으니까
	if (distance > attackRange)
	{
		// 3. 상태를 이동으로 전환하고 싶다.
		mState = EEnemyState::Move;
		// 애니메이션 상태 동기화
		anim->animState = mState;
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

// 피격상태
void UEnemyFSM::DamageState()
{
	//1.시간이 흘렀으니까
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2.만약 경과시간이 대기시간을 초과했다면
	if (currentTime > damageDelayTime)
	{
		//3.대기 상태로 전환하고 싶다.
		mState = EEnemyState::Idle;
		// 경과시간초기화 
		currentTime = 0;
		// 애니메이션 상태 동기화
		anim->animState = mState;
	}
}

// 죽음상태
void UEnemyFSM::DieState()
{
	// 아직 죽음 애니메이션이 끝나지 않았다면
	// 바닥내려가지 않도록 처리
	if (anim->bDieDone == false)
	{
		return;
	}

	//// 계속 아래로 내려가고 싶다.
	// 등속운동 공식 P = P0 + vt
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;
	me->SetActorLocation(P);

	//// 1. 만약 2미터 이상 내려왔다면
	if (P.Z < -200.0f)
	{
		// 2. 제거시킨다.
		me->Destroy();
	}
}

void UEnemyFSM::SetMoveSpeed(float value)
{
	moveSpeed = value;
}

// 피격 알림 이벤트 함수
void UEnemyFSM::OnDamageProcess()
{
	// 체력감소
	hp--;
	// 만약 체력이 남아있다면
	if (hp > 0 )
	{
		
		// 상태를 피격으로 전환
		mState = EEnemyState::Damage;
		me->hpBar->cHP = hp;
		currentTime = 0;

		// 피격 애니메이션 재생
		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), index);
		anim->PlayDamageAnim(FName(*sectionName));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, *FString::Printf(TEXT("OnDamage%d"), index));
		//section 지정 플레이는 나중에 구현
		//if (!anim->Montage_IsPlaying(damageAnim)) {
		//	anim->Montage_Play(damageAnim);
		//	//나중에 콤보샷 구현할 생각
		//	anim->Montage_JumpToSection(TEXT("damage1"), damageAnim);
		//}
	}
	// 그렇지 않다면
	else
	{
		// 상태를 죽음으로 전환
		mState = EEnemyState::Die;
		// 캡슐 충돌체 비활성화 죽었을때 바닥으로 사라지게하는 효과
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 죽음 애니메이션 재생 ABP_Enemy blend bool에서 처리하고 있음.
		anim->PlayDamageAnim(TEXT("Die"));
	}
	// 애니메이션 상태 동기화
	anim->animState = mState;

	ai->StopMovement();
}

// 랜덤위치 가져오기
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

