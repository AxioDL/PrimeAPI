#ifndef CGAMESTATE_HPP
#define CGAMESTATE_HPP

class CWorldState;

class CGameState
{
public:
	void SetCurrentWorldId(uint assetId);
	CWorldState* StateForWorld(uint assetId);
	CWorldState* CurrentWorldState();
};
extern CGameState* gpGameState;

#endif // CGAMESTATE_HPP