#ifndef CARCHITECTUREMESSAGE_HPP
#define CARCHITECTUREMESSAGE_HPP

#include <rstl/rc_ptr.h>
#include <Runtime/CFinalInput.hpp>

class CIOWin;

enum EArchMsgTarget
{
    kArchMsgTarget_IOWinManager = 0,
    kArchMsgTarget_Game = 1
};

enum EArchMsgType
{
    kArchMsgType_RemoveIOWin = 0,
    kArchMsgType_CreateIOWin = 1,
    kArchMsgType_ChangeIOWinPriority = 2,
    kArchMsgType_RemoveAllIOWins = 3,
    kArchMsgType_TimerTick = 4,
    kArchMsgType_UserInput = 5,
    kArchMsgType_SetGameState = 6,
    kArchMsgType_ControllerStatus = 7,
    kArchMsgType_QuitGameplay = 8,
    kArchMsgType_FrameBegin = 10,
    kArchMsgType_FrameEnd = 11,
};

struct IArchMsgParm
{
    uint32 vtable;
};

struct CArchMsgParmInt32 : IArchMsgParm
{
    uint32 x4_parm;
};

struct CArchMsgParmVoidPtr : IArchMsgParm
{
    void* x4_parm1;
};

struct CArchMsgParmInt32Int32VoidPtr : IArchMsgParm
{
    uint32 x4_parm1;
    uint32 x8_parm2;
    void* xc_parm3;
};

struct CArchMsgParmInt32Int32IOWin : IArchMsgParm
{
    uint32 x4_parm1;
    uint32 x8_parm2;
    rstl::rc_ptr<CIOWin> xc_parm3;
};

struct CArchMsgParmNull : IArchMsgParm
{
};

struct CArchMsgParmReal32 : IArchMsgParm
{
    float x4_parm;
};

struct CArchMsgParmUserInput : IArchMsgParm
{
    CFinalInput x4_parm;
};

struct CArchMsgParmControllerStatus : IArchMsgParm
{
    uint16 x4_parm1;
    bool x6_parm2;
};

class CArchitectureMessage
{
public:
    EArchMsgTarget x0_target;
    EArchMsgType x4_type;
    rstl::rc_ptr<IArchMsgParm> x8_parm;
};

#endif //PRIME_PRACTICE_CARCHITECTUREMESSAGE_HPP
