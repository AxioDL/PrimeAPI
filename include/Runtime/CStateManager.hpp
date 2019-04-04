#ifndef CSTATEMANAGER_HPP
#define CSTATEMANAGER_HPP

#include <PrimeAPI.h>
#include <rstl/rc_ptr.h>

#define gpStateManager ((CStateManager *) 0x8045A1A8)

class CPlayer;
class CPlayerState;

class CStateManager
{
public:
	inline CPlayer* GetPlayer() const 				{ return **GetField<CPlayer**>(this, 0x84c); }
	inline CPlayerState* GetPlayerState() const		{ return **GetField<CPlayerState**>(this, 0x8b8); }
};

#endif