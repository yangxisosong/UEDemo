
#include "DLGameplayCameraShakeWave.h"

float FDLWaveOscillator::Initialize(float& OutInitialOffset) const
{
	if(InitialOffsetType == EDLInitialWaveOscillatorOffsetType::Random)
	{
		OutInitialOffset = FMath::FRand() * (2.f * PI);
	}
	else if(InitialOffsetType == EDLInitialWaveOscillatorOffsetType::ArrayRandom)
	{
		const int32 ArrayNum = RandomDots.Num();
		if(ArrayNum > 0)
		{
			const int32 RandResult = FMath::RandRange(0, ArrayNum - 1);
			OutInitialOffset = RandomDots[RandResult].Dot * PI;
		}
		else
		{
			OutInitialOffset = 0;
		}
	}
	else
	{
		OutInitialOffset = 0;
	}
	return Amplitude * FMath::Sin(OutInitialOffset);
}

float FDLWaveOscillator::Update(float DeltaTime, float AmplitudeMultiplier, float FrequencyMultiplier,
	float& InOutCurrentOffset) const
{
	const float TotalAmplitude = Amplitude * AmplitudeMultiplier;
	if (TotalAmplitude != 0.f)
	{
		InOutCurrentOffset += DeltaTime * Frequency * FrequencyMultiplier * (2.f * PI);
		return TotalAmplitude * FMath::Sin(InOutCurrentOffset);
	}
	return 0.f;
}

UDLGameplayCameraShakeWave::UDLGameplayCameraShakeWave(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
	// Default to only location shaking.
	RotationAmplitudeMultiplier = 0.f;
	FOV.Amplitude = 0.f;
}

void UDLGameplayCameraShakeWave::StartShakePatternImpl(const FCameraShakeStartParams& Params)
{
	if (!Params.bIsRestarting)
	{
		X.Initialize(InitialLocationOffset.X);
		Y.Initialize(InitialLocationOffset.Y);
		Z.Initialize(InitialLocationOffset.Z);

		CurrentLocationOffset = InitialLocationOffset;

		Pitch.Initialize(InitialRotationOffset.X);
		Yaw.Initialize(InitialRotationOffset.Y);
		Roll.Initialize(InitialRotationOffset.Z);

		CurrentRotationOffset = InitialRotationOffset;

		FOV.Initialize(InitialFOVOffset);

		CurrentFOVOffset = InitialFOVOffset;
	}
}

void UDLGameplayCameraShakeWave::UpdateShakePatternImpl(const FCameraShakeUpdateParams& Params,
	FCameraShakeUpdateResult& OutResult)
{
	UpdateOscillators(Params.DeltaTime, OutResult);
}

void UDLGameplayCameraShakeWave::ScrubShakePatternImpl(const FCameraShakeScrubParams& Params,
	FCameraShakeUpdateResult& OutResult)
{
	// Scrubbing is like going back to our initial state and updating directly to the scrub time.
	CurrentLocationOffset = InitialLocationOffset;
	CurrentRotationOffset = InitialRotationOffset;
	CurrentFOVOffset = InitialFOVOffset;

	UpdateOscillators(Params.AbsoluteTime, OutResult);
}

void UDLGameplayCameraShakeWave::GetShakePatternInfoImpl(FCameraShakeInfo& OutInfo) const
{
	if (Duration > 0.f)
	{
		OutInfo.Duration = Duration;
	}
	else
	{
		OutInfo.Duration = FCameraShakeDuration::Infinite();
	}

	OutInfo.BlendIn = BlendInTime;
	OutInfo.BlendOut = BlendOutTime;
}

void UDLGameplayCameraShakeWave::UpdateOscillators(float DeltaTime, FCameraShakeUpdateResult& OutResult)
{
	OutResult.Location.X = X.Update(DeltaTime, LocationAmplitudeMultiplier, LocationFrequencyMultiplier, CurrentLocationOffset.X);
	OutResult.Location.Y = Y.Update(DeltaTime, LocationAmplitudeMultiplier, LocationFrequencyMultiplier, CurrentLocationOffset.Y);
	OutResult.Location.Z = Z.Update(DeltaTime, LocationAmplitudeMultiplier, LocationFrequencyMultiplier, CurrentLocationOffset.Z);

	OutResult.Rotation.Pitch = Pitch.Update(DeltaTime, RotationAmplitudeMultiplier, RotationFrequencyMultiplier, CurrentRotationOffset.X);
	OutResult.Rotation.Yaw = Yaw.Update(DeltaTime, RotationAmplitudeMultiplier, RotationFrequencyMultiplier, CurrentRotationOffset.Y);
	OutResult.Rotation.Roll = Roll.Update(DeltaTime, RotationAmplitudeMultiplier, RotationFrequencyMultiplier, CurrentRotationOffset.Z);

	OutResult.FOV = FOV.Update(DeltaTime, 1.f, 1.f, CurrentFOVOffset);
}
