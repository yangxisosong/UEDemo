#include "DLGameplayPredictTaskTypes.h"

#include <vector>

using FCaller = TFunction<FDLGameplayPredictTaskContextBase* (FDLGameplayPredictTaskContextTypeId Id)>;


struct FData
{
	FDLGameplayPredictTaskContextTypeId Id;
	FCaller Caller;
};


static std::vector<FData> PredictTaskContextFactorData;

void RegisterPredictTaskContextFactor(
	FDLGameplayPredictTaskContextTypeId Id,
	TFunction<FDLGameplayPredictTaskContextBase* (FDLGameplayPredictTaskContextTypeId Id)> Factor)
{
	//int32* p = nullptr;
	//*p = 222;
	PredictTaskContextFactorData.push_back({Id, Factor});
}


FDLGameplayPredictTaskContextBase* CreatePredictTaskContext(const FDLGameplayPredictTaskContextTypeId TypeId)
{
	for (const auto& v : PredictTaskContextFactorData)
	{
		if (v.Id == TypeId)
		{
			return v.Caller(TypeId);
		}
	}
	return nullptr;
}