#ifndef RSTL_VECTOR_H
#define RSTL_VECTOR_H

#include "rstl/rstl.h"

RSTL_BEGIN

template<typename T>
class vector
{
    uint32 mFirst;
    uint32 mSize;
    uint32 mMax;
    T* mPtr;

public:
	inline uint32 size() const	{ return mSize; }
	inline uint32 capacity() const { return mMax; }
	inline T& operator[](int i) const { return mPtr[i]; }
	inline T* data() const { return mPtr; }
};

template<typename T>
class reserved_vector
{
public:
    int32 len;
    T* first;
    T* end;
    T* last;
};

RSTL_END


#endif // RSTL_VECTOR_H