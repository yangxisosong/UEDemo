#pragma once

#include "CoreMinimal.h"


class DLGAMEPLAYTYPES_API FDLGameplayDataTableHelper
{
public:

	static const struct FDTPlayerCharacterInfoRow* FindPlayerCharacterInfo(FName CharacterId, bool EnsureFind = true);
	
	static const struct FDTNPCCharacterInfoRow* FindNPCCharacterInfo(FName CharacterId, bool EnsureFind = true);

	static const struct FDTSceneInfoRow* FindSceneInfo(FName Id, bool EnsureFind = true);

	static TArray<const struct FDTNPCCharacterInfoRow*> GetAllNPCCharacterInfo();
};
