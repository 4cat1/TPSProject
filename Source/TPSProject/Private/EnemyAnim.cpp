

#include "EnemyAnim.h"

void UEnemyAnim::OnEndAttackAnimation()
{
	bAttackPlay = false;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("OnEndAttackAnim Call"));
}
