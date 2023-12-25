// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBar.generated.h"

/**
 *
 */
UCLASS()
class TPSPROJECT_API UHPBar : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HPBar")
	FText ctextId;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HPBar")
	FText cInfo;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HPBar")
	int cHP;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HPBar")
	int cMaxHP;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HPBar")
	int cMP;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HPBar")
	int cMaxMP;
};
