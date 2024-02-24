// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TargetSelectorPreviewObject.generated.h"

/**
 *
 */
UCLASS()
class UTargetSelectorPreviewObject
	: public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "AuxiliaryLineStyle")
	//描绘碰撞物体边框的颜色
	FColor CollisionLineColor = FColor::Orange;

	UPROPERTY(EditDefaultsOnly, Category = "AuxiliaryLineStyle")
	//描绘锚点的颜色
	FColor AnchorLineColor = FColor::Red;

	UPROPERTY(EditAnywhere, Category = "AuxiliaryLineStyle")
	//描绘碰撞区域的颜色
	FColor CollisionAreaLineColor = FColor::Red;
	UPROPERTY(EditAnywhere, Category = "AuxiliaryLineStyle")
	//描绘碰撞区域线的宽度
	int32 CollisionAreaLineWidth = 1;

	UPROPERTY(EditAnywhere, Category = "AuxiliaryLineStyle")
	//指向锚点的颜色
	FColor ToAnchorLineColor = FColor::Red;
	UPROPERTY(EditAnywhere, Category = "AuxiliaryLineStyle")
	//指向锚点线的宽度
	int32 ToAnchorLineWidth = 1;

	UPROPERTY(EditAnywhere, Category = "AuxiliaryLineStyle")
	//指向碰撞体起点和终点的颜色
	FColor ToCollisionLineColor = FColor::Blue;

	UPROPERTY(EditAnywhere, Category = "AuxiliaryLineStyle")
	//指向碰撞体起点和终点连线的宽度
	int32 ToCollisionLineWidth = 1;

	UPROPERTY(EditAnywhere, Category = "AuxiliaryLineStyle")
	//方向箭头的缩放
	FColor ArrowColor = FColor::Red;
	UPROPERTY(EditAnywhere, Category = "AuxiliaryLineStyle")
	//方向箭头的缩放
	float ArrowScale = 1;
};
