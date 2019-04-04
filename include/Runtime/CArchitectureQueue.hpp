#ifndef CARCHITECTUREQUEUE_HPP
#define CARCHITECTUREQUEUE_HPP

#include <Runtime/CArchitectureMessage.hpp>

class CArchitectureQueue
{
public:
    PADDING(8);
	
	void InsertMsg(const CArchitectureMessage&);
};

#endif // CARCHITECTUREQUEUE_HPP
