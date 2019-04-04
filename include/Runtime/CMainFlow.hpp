#ifndef CMAINFLOW_HPP
#define CMAINFLOW_HPP

#include <Runtime/CIOWin.hpp>

class CArchitectureMessage;
class CArchitectureQueue;

enum EClientFlowStates
{
	kCFS_Unspecified = -1,
	kCFS_PreFrontEnd = 7,
	kCFS_FrontEnd = 8,
	kCFS_Game = 14,
	kCFS_GameExit = 15
};

class CMainFlow
{
public:
	DECLARE_FIELD(EClientFlowStates, GameState, 0x14);
	
    CIOWin::EMessageReturn OnMessage(const CArchitectureMessage& msg, CArchitectureQueue& queue);
	void AdvanceGameState(CArchitectureQueue& queue);
	void SetGameState(EClientFlowStates state, CArchitectureQueue& queue);
};

#endif // CMAINFLOW_HPP
