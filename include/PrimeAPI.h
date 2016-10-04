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

// Common defines
#ifndef NULL
#define NULL 0
#endif

#define PADDING(Amt) char padding##Amt[##Amt]

// Allocation
void* operator new(uint32 size, const char *pkFileAndLine, const char *pkType);
void* operator new[](uint32 size, const char *pkFileAndLine, const char *pkType);

#endif