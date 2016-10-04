#ifndef CSTATEMANAGER_H
#define CSTATEMANAGER_H

#include "PrimeAPI.h"

class CPlayerState;

class CStateManager
{
	PADDING(0x8b8);
	CPlayerState *mpPlayerState;
	
public:
	inline CPlayerState* GetPlayerState() const	{ return mpPlayerState; }
};

#endif