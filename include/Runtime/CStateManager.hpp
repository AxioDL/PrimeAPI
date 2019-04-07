#ifndef CSTATEMANAGER_HPP
#define CSTATEMANAGER_HPP

#include <PrimeAPI.h>
#include <rstl/rc_ptr.h>

/** note - avoid using this macro - not portable between game versions */
#define gpStateManager ((CStateManager *) 0x8045A1A8)

class CPlayer;
class CPlayerState;

class CStateManager
{
public:
	enum EInitPhase
	{
		kInit_LoadWorld = 0,
		kInit_LoadFirstArea = 1,
		kInit_Done = 2
	};
	
	void InitializeState(uint WorldAssetId, TAreaId AreaId, uint AreaAssetId);
	
	inline CPlayer* GetPlayer() const 				{ return *GetField<CPlayer*>(this, 0x84C); }
	inline CPlayerState* GetPlayerState() const		{ return **GetField<CPlayerState**>(this, 0x8b8); }
	inline EInitPhase GetInitPhase() const			{ return *GetField<EInitPhase>(this, 0xB3C); }
};

#endif