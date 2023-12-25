// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSplayer.h"
#include <GameFramework/CharacterMovementComponent.h>

void UPlayerAnim::PlayAttackAnim()
{
	isFire = true;
	Montage_Play(attackAnimMontage);
}

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	auto ownerPawn = TryGetPawnOwner();
	auto player = Cast<ATPSPlayer>(ownerPawn);

	if (player) {
		FVector velocity = player->GetVelocity();
		FVector forwardVector = player->GetActorForwardVector();
		speed = FVector::DotProduct(forwardVector, velocity);
		FVector rightVector = player->GetActorRightVector();
		direction = FVector::DotProduct(rightVector, velocity);
		auto movement = player->GetCharacterMovement();
		isInAir = movement->IsFalling();
	}
}
