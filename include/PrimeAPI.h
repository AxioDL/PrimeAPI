#ifndef PRIMEAPI_H
#define PRIMEAPI_H

// Add this macro at the beginning of _prolog
#define MODULE_INIT \
	extern void ApplyCodePatches(); \
	ApplyCodePatches();

// Patches all references to originalSymbol in the base game to point to newSymbol instead
#define PATCH_SYMBOL(originalSymbol, newSymbol)

// Common typedefs
typedef signed char			int8;
typedef signed short		int16;
typedef signed long			int32;
typedef signed long long	int64;
typedef unsigned int		uint;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned long		uint32;
typedef unsigned long long	uint64;

// Other common types
struct TAreaId {
	int32 Id;
	inline operator int32() const { return Id; }
};

// Common defines
#ifndef NULL
#define NULL 0
#endif

#define PADDING(Amt) char padding##Amt[##Amt]
#define ALIGN(V, Alignment) (((V)+(Alignment)) & ~((Alignment)-1))

// Retrieves a field from an incomplete class declaration
template<typename T>
inline T* GetField(const void* inThis, uint offset)
{
	return reinterpret_cast<T*>( ((char*) inThis) + offset );
};

// Defines a field within an incomplete class declaration by declaring the following member functions:
//	[FieldType] Get[FieldName]() const;
//	Set[FieldName](const [FieldType]&);
#define DECLARE_FIELD(FieldType, FieldName, FieldOffset) \
	FieldType Get##FieldName() const 			{ return *GetField<FieldType>(this, FieldOffset); } \
	void Set##FieldName(const FieldType& in) 	{ *GetField<FieldType>(this, FieldOffset) = in; }

// new operators from Metroid Prime
void* operator new  (uint32 size, const char* fileAndLine, const char* type);
void* operator new[](uint32 size, const char* fileAndLine, const char* type);

// new operator implementation without extra parameters
inline void* operator new  (uint32 size) { return ::operator new  (size, "??(??)", NULL); }
inline void* operator new[](uint32 size) { return ::operator new[](size, "??(??)", NULL); }

// delete operator implementation (the operators are inlined in Metroid Prime)
#include <Runtime/CMemory.hpp>
inline void operator delete  (void* data) { CMemory::Free(data); }
inline void operator delete[](void* data) { CMemory::Free(data); }

// Forward declare of some functions to allow usage without STL headers
extern "C" {
	void memcpy(void* dst, void* src, int len);
}

#endif