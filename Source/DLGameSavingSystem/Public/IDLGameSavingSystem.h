#pragma once


#include "CoreMinimal.h"


class IDLNPCSavingAccessor;
class IDLPlayerSavingAccessor;

class DLGAMESAVINGSYSTEM_API IDLGameSavingSystem
{
public:
	virtual ~IDLGameSavingSystem() = default;

	static IDLGameSavingSystem* Get(const UObject* WorldContext);


public:

	virtual IDLNPCSavingAccessor* GetNPCSavingAccessor() = 0;

	virtual IDLPlayerSavingAccessor* GetPlayerSavingAccessor() = 0;

};
