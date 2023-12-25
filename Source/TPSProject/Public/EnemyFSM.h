#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

// ����� ���� ����
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle, Move, Attack, Damage, Die,
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TPSPROJECT_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyFSM();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// ���º���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FSM)
	EEnemyState mState = EEnemyState::Idle;

	// ������
	void IdleState();
	// �̵�����
	void MoveState(float DeltaTime);
	// ���ݻ���
	void AttackState();
	// �ǰݻ���
	void DamageState();
	// ��������
	void DieState();

	// ���ð�
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float idleDelayTime = 2;
	// ����ð�
	float currentTime = 0;

	// Ÿ��
	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ATPSPlayer* target;

	// ��������
	UPROPERTY()
	class AEnemy* me;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
	FString id;
	//�̵��ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
	float moveSpeed = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
	float minSpeed = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
	float maxSpeed = 10.f;
	UFUNCTION()
	void SetMoveSpeed(float value);
	// ���ݹ���
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 150.0f;

	// ���ݴ��ð�
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackDelayTime = 3.0f;

	// �ǰ� �˸� �̺�Ʈ �Լ�
	void OnDamageProcess();

	// ü��
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	int32 maxHp = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	int32 hp = maxHp;
	// �ǰݴ��ð�
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	int32 maxMp = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	int32 mp = maxHp;
	// �ǰݴ��ð�
	UPROPERTY(EditAnywhere, Category = FSM)
	float damageDelayTime = 2.0f;
	UPROPERTY(EditAnywhere, Category = FSM)
	float mpDelayTime = 2.0f;
	// �Ʒ��� ������� �ӵ�
	UPROPERTY(EditAnywhere, Category = FSM)
	float dieSpeed = 50.0f;

	// ������� �ִϸ��̼� �������Ʈ
	UPROPERTY()
	class UEnemyAnim* anim;

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* damageAnim;

	// Enemy �� �����ϰ� �ִ� AIController
	UPROPERTY()
	class AAIController* ai;

	// ��ã�� ����� ������ġ
	FVector randomPos;
	// ������ġ ��������
	bool GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest);
};
