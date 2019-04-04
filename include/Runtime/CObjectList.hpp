#ifndef COBJECTLIST_HPP
#define COBJECTLIST_HPP

#include <PrimeAPI.h>

class CEntity;

class SObjectListEntry
{
public:
    CEntity* pEntity;
    uint16 next;
    uint16 prev;
};

class CObjectList
{
public:
    uint32 vtable;
    SObjectListEntry mEntries[1024];
    uint32 mType;
    uint16 mFirst;
    uint16 mCount;
};

#endif //PRIME_PRACTICE_NATIVE_COBJECTLIST_HPP
