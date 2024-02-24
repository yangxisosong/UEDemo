#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameplayTagContainer.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"
#include "DLAbilityTargetSelector.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDLGameplayTargetSelector, Log, All);


UENUM(BlueprintType)
enum class ESelectTargetMode : uint8
{
	CoordinatePoint,
	Unit,
};


UENUM(BlueprintType)
enum class ESelectTargetRelativeObjType : uint8
{
	// 自己
	Self,

	// 锁定的角色
	LockCharacter,
};

UENUM(BlueprintType)
enum class ECoordinateConstraintShape : uint8
{
	//圆柱
	Cylinder,
	//盒子
	Box,
	//球
	Sphere
};

USTRUCT(BlueprintType)
struct FUnitSelectFilterContext
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	AActor* OriginalActor = nullptr;
};


UCLASS(MinimalAPI, Abstract, NotBlueprintType, Blueprintable, CollapseCategories, EditInlineNew)
class UDLGameplayUnitTargetSelectFilter
	: public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void FilterUnits(const FUnitSelectFilterContext& Context, const TArray<FHitResult>& InUnits,
	                 TArray<FHitResult>& OutUnits);

	virtual void FilterUnits_Implementation(const FUnitSelectFilterContext& Context, const TArray<FHitResult>& InUnits,
	                                        TArray<FHitResult>& OutUnits)
	{
	}
};


UCLASS(Meta = (Displayname = UnitFilterAnd))
class UDLGameplayUnitSelectAnd
	: public UDLGameplayUnitTargetSelectFilter
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Instanced)
	TArray<UDLGameplayUnitTargetSelectFilter*> OperateFilters;

	virtual void FilterUnits_Implementation(const FUnitSelectFilterContext& Context, const TArray<FHitResult>& InUnits,
	                                        TArray<FHitResult>& OutUnits) override
	{
		static TArray<FHitResult> OutTemp;
		static TArray<FHitResult> InTemp;

		OutTemp.Empty();
		InTemp = InUnits;

		for (const auto& Filters : OperateFilters)
		{
			if (Filters)
			{
				Filters->FilterUnits(Context, InTemp, OutTemp);
				InTemp = MoveTemp(OutTemp);
			}
		}

		OutUnits = InTemp;
	}
};


UCLASS(Meta = (Displayname = UnitFilterOr))
class UDLGameplayUnitSelectOr
	: public UDLGameplayUnitTargetSelectFilter
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Instanced)
	TArray<UDLGameplayUnitTargetSelectFilter*> OperateFilters;

	virtual void FilterUnits_Implementation(const FUnitSelectFilterContext& Context, const TArray<FHitResult>& InUnits,
	                                        TArray<FHitResult>& OutUnits) override
	{
		static TArray<FHitResult> OutTemp;

		OutTemp.Empty();

		for (const auto& Filters : OperateFilters)
		{
			if (Filters)
			{
				Filters->FilterUnits(Context, InUnits, OutTemp);

				for (const auto& Unit : OutTemp)
				{
					OutUnits.RemoveAll([Unit](const FHitResult& HitRet)
					{
						return Unit.GetActor() == HitRet.GetActor();
					});

					OutUnits.Append(OutTemp);
				}

				OutTemp.Empty();
			}
		}
	}
};


UCLASS(Abstract)
class UDLGameplayUnitSelectCampRelationship
	: public UDLGameplayUnitTargetSelectFilter
{
	GENERATED_BODY()
public:
	// 选择目标需要满足的阵营关系
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (SupportTags = "Gameplay.Unit.CampRelationship"))
	FGameplayTagContainer SelectUnitCampRelationship;
};


UENUM(BlueprintType)
enum class EAnchorSelectType
	: uint8
{
	// 基于Actor的位置信息
	ActorTransform,

	// 基于骨骼插槽
	Socket,
};


USTRUCT(BlueprintType)
struct FGameplayTargetSelectorExtendData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	bool bOverrideLocation = false;

	UPROPERTY(BlueprintReadWrite)
	FVector OverrideLocation;

	UPROPERTY(BlueprintReadWrite)
	bool bOverrideRotation = false;

	UPROPERTY(BlueprintReadWrite)
	FRotator OverrideRotation;
};

UCLASS(NotBlueprintable, BlueprintType, Abstract)
class DLGAMEPLAYTARGETSELECTOR_API UDLGameplayTargetSelectorBase
	: public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Base)
	ESelectTargetMode SelectTargetMode = ESelectTargetMode::CoordinatePoint;

	// 锚点选择类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Anchor)
	EAnchorSelectType AnchorSelectType;

	// 锚点 对象的骨骼插槽
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Anchor,
		Meta = (Instanced, EditCondition = "AnchorSelectType == EAnchorSelectType::Socket", EditConditionHides
			, GetOptions = "GetSockeNames"))
	FName AnchorSocketName;

#if WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, Category = Preview, meta = (AllowedClasses = "StaticMesh,SkeletalMesh"))
	TSoftObjectPtr<UStreamableRenderAsset> PreviewMesh;

	UPROPERTY(EditDefaultsOnly, Category = Preview)
	TSubclassOf<ACharacter> PreviewActor;

	UPROPERTY(EditDefaultsOnly, Category = Preview, meta = (AllowedClasses = "AnimationAsset"))
	TSoftObjectPtr<UAnimationAsset> PreviewMeshAnimation;

	UPROPERTY(EditDefaultsOnly, Category = Preview)
	bool IsLoop = false;

	UPROPERTY(EditDefaultsOnly, Category = Preview)
	bool IsPlay = false;

	//动画播放进度(0-1)
	UPROPERTY(EditDefaultsOnly, Category = Preview,
		meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 1.0f, UIMax = 1.0f))
	float Schedule = 0;

	UPROPERTY(EditDefaultsOnly, Category = Preview)
	FTransform PreviewMeshTransform = FTransform::Identity;

	// 锚点的变换
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Preview,
		Meta = (Instanced, EditCondition = "false", EditConditionHidess))
	FTransform AnchorTransform = FTransform::Identity;

	// 锚点的size
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Preview)
	float AnchorSize = 1.0f;

	//修正预览模型的位置（只适用于预览界面，实际游戏中不生效）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Preview)
	FTransform ResetPreviewTransform = FTransform::Identity;

#endif

protected:
	TWeakObjectPtr<AActor> TempOriginActor;

public:
	/**
	* @brief 计算选中的单位集合
	* @param OriginalActor 起始位置 Actor
	* @param CalculateMsg 发起计算的对象信息，用来调试输出
	* @param Succeed True 存着任意一个符合要求的选中单位，其他情况为 False
	* @param RetUnits 返回计算的结果
	* @param ExtendData 扩展数据
	*/
	UFUNCTION(BlueprintCallable)
	void CalculateTargetUnits(const AActor* OriginalActor, const FString& CalculateMsg, bool& Succeed,
	                          TArray<FHitResult>& RetUnits, FGameplayTargetSelectorExtendData ExtendData);


	/**
	 * @brief 计算选中的单位集合
	 * @param WorldContext
	 * @param OriginalTransform 起始位置的 Transform
	 * @param CalculateMsg 发起计算的对象信息，用来调试输出
	 * @param Succeed True 存着任意一个符合要求的选中单位，其他情况为 False
	 * @param RetUnits 返回计算的结果
	 */
	UFUNCTION(BlueprintCallable, Meta = (WorldContext = "WorldContext"))
	void CalculateTargetUnitsWithTransform(const UObject* WorldContext, const FTransform& OriginalTransform,
	                                       const FString& CalculateMsg, bool& Succeed, TArray<FHitResult>& RetUnits);

	/**
	* @brief 计算选中的 Transforms
	* @param OriginalActor  起始位置 Actor
	* @param CalculateMsg 发起计算的对象信息，用来调试输出
	* @param Succeed True 存着任意一个符合要求的选中单位，其他情况为 False
	* @param RetTransform 返回计算的结果  相对于世界坐标系
	* @param ExtendData 扩展数据
	*/
	UFUNCTION(BlueprintCallable)
	void CalculateTargetTransforms(const AActor* OriginalActor, const FString& CalculateMsg, bool& Succeed,
	                               TArray<FTransform>& RetTransform, FGameplayTargetSelectorExtendData ExtendData);

	UFUNCTION(BlueprintCallable)
	void CalculateTargetTransformsWithTransform(const UObject* WorldContext, const FTransform& OriginalTransform,
	                                            const FString& CalculateMsg, bool& Succeed,
	                                            TArray<FTransform>& RetTransform);

	/**
	* @brief 计算选中的 Location
	* @param OriginalActor  起始位置 Actor
	* @param CalculateMsg 发起计算的对象信息，用来调试输出
	* @param Succeed True 存着任意一个符合要求的选中单位，其他情况为 False
	* @param RetPoints 返回计算的结果  相对于世界坐标系
	* @param ExtendData 扩展数据
	*/
	UFUNCTION(BlueprintCallable)
	void CalculateTargetLocation(const AActor* OriginalActor, const FString& CalculateMsg, bool& Succeed,
	                             TArray<FVector>& RetPoints, FGameplayTargetSelectorExtendData ExtendData);


	UFUNCTION(BlueprintCallable)
	void CalculateTargetLocationWithTransform(const UObject* WorldContext, const FTransform& OriginalTransform,
	                                          const FString& CalculateMsg, bool& Succeed, TArray<FVector>& RetPoints);


	UFUNCTION(BlueprintCallable)
	const ACharacter* GetAnchorCharacter(const AActor* OriginalActor, const FString& CalculateMsg);

	UFUNCTION(BlueprintCallable)
	bool CalculateAnchorTransform(const AActor* OriginalActor, const FString& CalculateMsg, FTransform& OutTransform);


	UFUNCTION()
	TArray<FString> GetSockeNames() const;


protected:
	UFUNCTION(BlueprintNativeEvent, DisplayName = CalculateTargetUnits)
	void K2_CalculateTargetUnits(const UObject* WorldContext, const FTransform& InAnchorTransform,
	                             const FString& CalculateMsg, bool& Succeed, TArray<FHitResult>& RetUnits);


	UFUNCTION(BlueprintNativeEvent, DisplayName = CalculateTargetPoints)
	void K2_CalculateTargetTransforms(const UObject* WorldContext, const FTransform& InAnchorTransform,
	                                  const FString& CalculateMsg, bool& Succeed, TArray<FTransform>& RetPoints);


protected:
	virtual void K2_CalculateTargetUnits_Implementation(const UObject* WorldContext,
	                                                    const FTransform& InAnchorTransform,
	                                                    const FString& CalculateMsg, bool& Succeed,
	                                                    TArray<FHitResult>& RetUnits)
	{
		Succeed = false;
	}

	virtual void K2_CalculateTargetTransforms_Implementation(const UObject* WorldContext,
	                                                         const FTransform& InAnchorTransform,
	                                                         const FString& CalculateMsg, bool& Succeed,
	                                                         TArray<FTransform>& RetPoints)
	{
		Succeed = false;
	}
};


UCLASS(Blueprintable, MinimalAPI, Abstract, BlueprintType, DisplayName = TransformSelectorBase)
class UDLGameplayTargetSelectorCoordinate
	: public UDLGameplayTargetSelectorBase
{
	GENERATED_BODY()
};

//单个坐标点生成器
UCLASS(DisplayName = "TransformSelector(One)")
class DLGAMEPLAYTARGETSELECTOR_API UDLGameplayTargetSelectorCoordinateSimple
	: public UDLGameplayTargetSelectorCoordinate
{
	GENERATED_BODY()
public:
	virtual void K2_CalculateTargetTransforms_Implementation(
		const UObject* WorldContext,
		const FTransform& InAnchorTransform,
		const FString& CalculateMsg,
		bool& Succeed,
		TArray<FTransform>& RetPoints) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTransform RelativeTransform;
};


/**
 * @brief 修正坐标点的数据
 */
UCLASS(MinimalAPI, Abstract, CollapseCategories, EditInlineNew)
class UDLTargetSelectorRevisedCoordinate : public UObject
{
	GENERATED_BODY()
public:
	//Revised 每次调用都会修改 RevisedOffset
	UPROPERTY()
	FTransform RevisedOffset = FTransform::Identity;
	/**
	 * @brief 修正点
	 * @param InCoordinate
	 * @return
	 */
	virtual void Revised(const FTransform& InCoordinate);
};

/**
 * @brief 根据最小距离和最大距离修正
 */
UCLASS(MinimalAPI, NotBlueprintType, CollapseCategories, EditInlineNew, DisplayName = RevisedByDistance)
class UDLTargetSelectorRevisedByDistance : public UDLTargetSelectorRevisedCoordinate
{
	GENERATED_BODY()
public:
	//距离点的最小距离
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinDistance = 0;
	//距离点的最大距离
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxDistance = 10;
public:
	/**
	 * @brief 修正点
	 * @param InCoordinate
	 * @return
	 */
	virtual void Revised(const FTransform& InCoordinate) override;
};


USTRUCT(BlueprintType)
struct FDLTargetSelectorCoordinateInfo
{
	GENERATED_BODY()

public:
	//对现有坐标进行修正
	void SetRevisedTransform();

	//获取修正后的坐标
	FTransform GetFinalTransform() const;
public:
	//局部坐标（模型为原点）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTransform Coordinate;

	//对坐标的修正信息
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FTransform RevisedOffset;

	//是否覆盖全局修正策略（是的话会启用自己的策略）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bOverrideGlobalRevised = false;

	//单独坐标的修正策略（为空没有修正）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		meta = (EditInline, EditCondition = "bOverrideGlobalRevised", EditConditionHides))
	TSoftObjectPtr<UDLTargetSelectorRevisedCoordinate> Revised = nullptr;
};

//多点坐标生成器
UCLASS(DisplayName = "TransformSelector(Multi)(PreBuild)")
class DLGAMEPLAYTARGETSELECTOR_API UDLGameplayTargetSelectorCoordinateCollection
	: public UDLGameplayTargetSelectorCoordinate
{
	GENERATED_BODY()
public:
	virtual void K2_CalculateTargetTransforms_Implementation(const UObject* WorldContext,
	                                                         const FTransform& InAnchorTransform,
	                                                         const FString& CalculateMsg,
	                                                         bool& Succeed,
	                                                         TArray<FTransform>& RetPoints) override;

	void RevisedTransform();

	FTransform GetFinalTransformByIndex(int32 Index);

	FTransform GetRevisedOffsetByIndex(int32 Index);

	void ClearRevisedOffset();
public:
	//坐标和坐标的修正信息
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FDLTargetSelectorCoordinateInfo> RelativeTransformArray;

	//全局修正策略
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, meta = (EditInline))
	UDLTargetSelectorRevisedCoordinate* GlobalRevised;
};


/**
 * @brief 随机生成点的 选择器，会在运行时根据规则进行动态生成
 */
UCLASS(DisplayName = "TransformSelector(Multi)(DynamicBuild)")
class DLGAMEPLAYTARGETSELECTOR_API UDLGameplayTargetSelectorDynamicGeneration
	: public UDLGameplayTargetSelectorCoordinateCollection
{
	GENERATED_BODY()
public:
	TArray<FTransform> CreatRandomCoordinate();

	virtual void K2_CalculateTargetTransforms_Implementation(const UObject* WorldContext,
	                                                         const FTransform& InAnchorTransform,
	                                                         const FString& CalculateMsg,
	                                                         bool& Succeed,
	                                                         TArray<FTransform>& RetPoints) override;
private:
	TArray<FTransform> CreateRandomCoordinateByBox(int32 CreatNum);

	TArray<FTransform> CreateRandomCoordinateByCylinder(int32 CreatNum);

	TArray<FTransform> CreateCoordinateByCylinder(int32 CreatNum);

	TArray<FTransform> CreateRandomCoordinateBySphere(int32 CreatNum);

public:
	//两点间最小距离
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo)
	float MinDistanceToDot = 5;

	//约束形状
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo)
	ECoordinateConstraintShape ShapeType;

	//约束范围的最小长度(x)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo,
		meta = (EditInline, EditCondition = "ShapeType==ECoordinateConstraintShape::Box"))
	float MinLength = 10;

	//约束范围的最小宽度(y)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo)
	float MinWidth = 100;

	//约束范围的最小高度(z)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo,
		meta = (EditInline, EditCondition = "ShapeType!=ECoordinateConstraintShape::Sphere"))
	float MinHeight = 20;

	//约束范围的最小长度(x)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo,
		meta = (EditInline, EditCondition = "ShapeType==ECoordinateConstraintShape::Box"))
	float MaxLength = 20;

	//约束范围的最小宽度(y)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo)
	float MaxWidth = 150;

	//约束范围的最小高度(z)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo,
		meta = (EditInline, EditCondition = "ShapeType==ECoordinateConstraintShape::Box"))
	float MaxHeight = 20;

	//生成数量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo)
	int32 CoordinateNum = 50;

	//当前条件最大可以生成点的数量
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo)
	int32 MaxLimitNum = 0;

	//是否显示最小距离（球的半径表示）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo)
	bool IsShowMinDistance = false;

	//是否在随机位置上再增加随机偏移
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo)
	bool IsOpenRandomOffset = false;

	//增加的随机偏移（在此范围内偏移）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ConstraintInfo,
		meta = (EditInline, EditCondition = "IsOpenRandomOffset==true"))
	FVector2D RandomOffset = FVector2D::ZeroVector;
};


UCLASS(Blueprintable, MinimalAPI, Abstract, BlueprintType, DisplayName = UnitSelectorBase)
class UDLGameplayTargetSelectorShapeTraceBase
	: public UDLGameplayTargetSelectorBase
{
	GENERATED_BODY()

public:
	UDLGameplayTargetSelectorShapeTraceBase();

public:
	/**
	 * @brief Debug 绘制 方式
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TraceDebug)
	TEnumAsByte<EDrawDebugTrace::Type> DebugTraceType = EDrawDebugTrace::ForDuration;

	/**
	 * @brief 跟踪的距离
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Trace)
	float TraceLength = 0.f;

	// TODO 这个参数未实现 需要策划给 Unit 的排序规则 ， 存在 Filter > SelectCount 的情况
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UnitSelect)
	int32 SelectCount = 1;

	/**
	 * @brief 单位过滤器  过滤选中的单位
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = UnitSelect)
	UDLGameplayUnitTargetSelectFilter* UnitTargetSelectFilter;


	/**
	 * @brief 是否排除自己
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UnitSelect)
	bool bExcludeSelf = true;


public:
	/**
	 * @brief 检测的对象类型
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay, Category = RuntimeTrace)
	TArray<TEnumAsByte<EObjectTypeQuery>> TargetObjectType;

	/**
	* @brief Trace 的起点
	*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = RuntimeTrace)
	FVector StartPosRelativeAnchor;


	/**
	* @brief Trace 的终点
	*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = RuntimeTrace)
	FVector EndPosRelativeAnchor;

	// 碰撞盒子的变换
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Preview)
	FTransform CollisionTransform = FTransform::Identity;
};


// 球形选择器
UCLASS(Blueprintable, MinimalAPI, DisplayName = UnitSelectorSphere)
class UDLGameplayTargetSelectorSphereTrace
	: public UDLGameplayTargetSelectorShapeTraceBase
{
	GENERATED_BODY()
public:
	/**
	 * @brief 球的半径
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Trace)
	float Radius = 10.0f;

protected:
	virtual void K2_CalculateTargetUnits_Implementation(const UObject* WorldContext,
	                                                    const FTransform& InAnchorTransform,
	                                                    const FString& CalculateMsg, bool& Succeed,
	                                                    TArray<FHitResult>& RetUnits) override;
};


// 盒子形状的选择器
UCLASS(Blueprintable, MinimalAPI, DisplayName = UnitSelectorBox)
class UDLGameplayTargetSelectorBoxTrace
	: public UDLGameplayTargetSelectorShapeTraceBase
{
	GENERATED_BODY()
public:
	/**
	 * @brief Box 的 长宽高的 半径
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Trace)
	FVector HalfSize = FVector(10, 10, 10);
protected:
	virtual void K2_CalculateTargetUnits_Implementation(const UObject* WorldContext,
	                                                    const FTransform& InAnchorTransform,
	                                                    const FString& CalculateMsg, bool& Succeed,
	                                                    TArray<FHitResult>& RetUnits) override;
};

// 胶囊体选择器
UCLASS(Blueprintable, MinimalAPI, DisplayName = UnitSelectorCapsule)
class UDLGameplayTargetSelectorCapsuleTrace
	: public UDLGameplayTargetSelectorShapeTraceBase
{
	GENERATED_BODY()
public:
	/**
	 * @brief 胶囊体的半径
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Trace)
	float CapsuleRadius = 10.0f;

	/**
	 * @brief 胶囊体的高度的半径
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Trace)
	float CapsuleHalfHeight = 20.0f;
protected:
	virtual void K2_CalculateTargetUnits_Implementation(const UObject* WorldContext,
	                                                    const FTransform& InAnchorTransform,
	                                                    const FString& CalculateMsg, bool& Succeed,
	                                                    TArray<FHitResult>& RetUnits) override;
};

// 直线选择器
UCLASS(Blueprintable, MinimalAPI, DisplayName = UnitSelectorLine)
class UDLGameplayTargetSelectorLineTrace
	: public UDLGameplayTargetSelectorBase
{
	GENERATED_BODY()
};
