#ifndef CHEALTHINFO_HPP
#define CHEALTHINFO_HPP

#include <PrimeAPI.h>

class CHealthInfo
{
public:
	float health;
	float knockbackResistance;
	
	inline CHealthInfo()
		: health(0.f), knockbackResistance(0.f) {}
	
	CHealthInfo(float health, float resistance);
};

#endif // CHEALTHINFO_HPP