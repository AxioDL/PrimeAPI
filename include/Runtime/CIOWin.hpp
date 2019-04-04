#ifndef CIOWIN_HPP
#define CIOWIN_HPP

class CIOWin
{
public:
    enum EMessageReturn
	{
        kMessageReturn_Normal = 0,
        kMessageReturn_Exit = 1,
        kMessageReturn_RemoveIOWinAndExit = 2,
        kMessageReturn_RemoveIOWin = 3
    };
};

#endif // CIOWIN_HPP
