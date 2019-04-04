#ifndef CMAIN_HPP
#define CMAIN_HPP

#include <PrimeAPI.h>

enum EFlowState
{
    kFlowState_None,
    kFlowState_WinBad,
    kFlowState_WinGood,
    kFlowState_WinBest,
    kFlowState_LoseGame,
    kFlowState_Default,
    kFlowState_StateSetter,
};

class CMain
{
public:
	DECLARE_FIELD(EFlowState, FlowState, 0x12C);
};

#endif // CMAIN_HPP
