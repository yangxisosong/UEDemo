#pragma once

#include "Camera/CameraShakeBase.h"
#include "DLGameplayCameraShakeWave.generated.h"


/** Shake start offset parameter. */
UENUM()
enum class EDLInitialWaveOscillatorOffsetType : uint8
{
	//在 0-2π上随机选择一个偏移
	Random,
	//无偏移
	Zero,
	//在RandomDots 数组中随机一个值乘上π作为偏移
	ArrayRandom
};

USTRUCT(BlueprintType)
struct FClampFloat
{
	GENERATED_BODY()

	//该数据会和PI 相乘 取值限定在（0,2）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 2.0f, UIMax = 2.0f))
		float Dot;
};

/** A wave oscillator for a single number. */
USTRUCT(BlueprintType)
struct FDLWaveOscillator
{
	GENERATED_BODY()

		/** Amplitude of the sinusoidal oscillation. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Oscillation)
		float Amplitude;

	/** Frequency of the sinusoidal oscillation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Oscillation)
		float Frequency;

	//初值偏移类型 选择ArrayRandom 后会在该数组中随机取值与PI相乘 数组为空 则初始偏移为0
	//数组中只包含一个元素 偏移就取该元素和PI相乘的结果
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Oscillation)
		TArray<FClampFloat> RandomDots;

	//随机类型
	//Zero 无偏移

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Oscillation)
		EDLInitialWaveOscillatorOffsetType InitialOffsetType;

	/** Creates a new wave oscillator. */
	FDLWaveOscillator()
		: Amplitude(1.f)
		, Frequency(1.f)
		, InitialOffsetType(EDLInitialWaveOscillatorOffsetType::Random)
	{}

	/** Sets the initial offset and returns the initial value of the oscillator */
	float Initialize(float& OutInitialOffset) const;

	/** Advances the oscillation time and returns the current value */
	float Update(float DeltaTime, float AmplitudeMultiplier, float FrequencyMultiplier, float& InOutCurrentOffset) const;
};
/**
 * Like UCameraShakeBase but with a perlin noise shake pattern by default, for convenience.
 */
UCLASS()
class UDLGameplayCameraShakeWave : public UCameraShakePattern
{
	GENERATED_BODY()

public:

	UDLGameplayCameraShakeWave(const FObjectInitializer& ObjInit);

public:
	/** Amplitude multiplier for all location oscillation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
		float LocationAmplitudeMultiplier = 1.f;

	/** Frequency multiplier for all location oscillation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
		float LocationFrequencyMultiplier = 1.f;

	/** Oscillation in the X axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
		FDLWaveOscillator X;

	/** Oscillation in the Y axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
		FDLWaveOscillator Y;

	/** Oscillation in the Z axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
		FDLWaveOscillator Z;

	/** Amplitude multiplier for all rotation oscillation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotation)
		float RotationAmplitudeMultiplier = 1.f;

	/** Frequency multiplier for all rotation oscillation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotation)
		float RotationFrequencyMultiplier = 1.f;

	/** Pitch oscillation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotation)
		FDLWaveOscillator Pitch;

	/** Yaw oscillation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotation)
		FDLWaveOscillator Yaw;

	/** Roll oscillation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotation)
		FDLWaveOscillator Roll;

	/** FOV oscillation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FOV)
		FDLWaveOscillator FOV;
	/** Duration in seconds of this shake. Zero or less means infinite. */
	UPROPERTY(EditAnywhere, Category = Timing)
		float Duration = 1.f;

	/** Blend-in time for this shake. Zero or less means no blend-in. */
	UPROPERTY(EditAnywhere, Category = Timing)
		float BlendInTime = 0.2f;

	/** Blend-out time for this shake. Zero or less means no blend-out. */
	UPROPERTY(EditAnywhere, Category = Timing)
		float BlendOutTime = 0.2f;

public:
	// UCameraShakePattern interface
	virtual void StartShakePatternImpl(const FCameraShakeStartParams& Params) override;
	virtual void UpdateShakePatternImpl(const FCameraShakeUpdateParams& Params, FCameraShakeUpdateResult& OutResult) override;
	virtual void ScrubShakePatternImpl(const FCameraShakeScrubParams& Params, FCameraShakeUpdateResult& OutResult) override;


	virtual void GetShakePatternInfoImpl(FCameraShakeInfo& OutInfo) const override;

	void UpdateOscillators(float DeltaTime, FCameraShakeUpdateResult& OutResult);

private:

	/** Initial sinusoidal offset for location oscillation. */
	FVector InitialLocationOffset;
	/** Current sinusoidal offset for location oscillation. */
	FVector CurrentLocationOffset;

	/** Initial sinusoidal offset for rotation oscillation. */
	FVector InitialRotationOffset;
	/** Current sinusoidal offset for rotation oscillation. */
	FVector CurrentRotationOffset;

	/** Initial sinusoidal offset for FOV oscillation */
	float InitialFOVOffset;
	/** Current sinusoidal offset for FOV oscillation */
	float CurrentFOVOffset;
};

