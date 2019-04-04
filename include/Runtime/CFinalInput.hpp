#ifndef CFINALINPUT_HPP
#define CFINALINPUT_HPP

#include <PrimeAPI.h>

class CFinalInput
{
public:
	PADDING(0x34);
	
    float DeltaTime();
    inline uint32 ControllerIdx() { return *GetField<uint32>(this, 0x4); };

    bool PStart() const;
    bool PR() const;
    bool PL() const;
    bool PZ() const;
    bool PY() const;
    bool PX() const;
    bool PB() const;
    bool PA() const;
    bool PDPRight() const;
    bool PDPLeft() const;
    bool PDPDown() const;
    bool PDPUp() const;
    bool PRTrigger() const;
    bool PLTrigger() const;
    bool PRARight() const;
    bool PRALeft() const;
    bool PRADown() const;
    bool PRAUp() const;
    bool PLARight() const;
    bool PLALeft() const;
    bool PLADown() const;
    bool PLAUp() const;
    bool DStart() const;
    bool DR() const;
    bool DL() const;
    bool DZ() const;
    bool DY() const;
    bool DX() const;
    bool DB() const;
    bool DA() const;
    bool DDPRight() const;
    bool DDPLeft() const;
    bool DDPDown() const;
    bool DDPUp() const;
    bool DRTrigger() const;
    bool DLTrigger() const;
    bool DRARight() const;
    bool DRALeft() const;
    bool DRADown() const;
    bool DRAUp() const;
    bool DLARight() const;
    bool DLALeft() const;
    bool DLADown() const;
    bool DLAUp() const;
    float AStart() const;
    float AR() const;
    float AL() const;
    float AZ() const;
    float AY() const;
    float AX() const;
    float AB() const;
    float AA() const;
    float ADPRight() const;
    float ADPLeft() const;
    float ADPDown() const;
    float ADPUp() const;
    float ARTrigger() const;
    float ALTrigger() const;
    float ARARight() const;
    float ARALeft() const;
    float ARADown() const;
    float ARAUp() const;
    float ALARight() const;
    float ALALeft() const;
    float ALADown() const;
    float ALAUp() const;

    inline float ALeftX() const			{ return ALARight() - ALALeft(); };
    inline float ALeftY() const 		{ return ALAUp() - ALADown(); };
    inline float ARightX() const 		{ return ARARight() - ARALeft(); };
    inline float ARightY() const 		{ return ARAUp() - ARADown(); };
    inline float ALeftTrigger() const 	{ return ALTrigger(); };
    inline float ARightTrigger() const	{ return ARTrigger(); };
};

#endif //PRIMEWATCH_PRIME_CFINALINPUT_HPP
