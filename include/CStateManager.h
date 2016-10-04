#ifndef CSTATEMANAGER_H
#define CSTATEMANAGER_H

#include "PrimeAPI.h"
#include "rstl/rc_ptr.h"

class CPlayerState;

class CStateManager
{
	PADDING(0x8b8);
	rstl::rc_ptr<CPlayerState> mpPlayerState;
	
public:
	inline CPlayerState* GetPlayerState() const	{ return mpPlayerState.RawPointer(); }
};

#endif