#ifndef CHEALTHINFO_HPP
#define CHEALTHINFO_HPP

#include <PrimeAPI.h>

class CHealthInfo
{
public:
	float mHealth;
	float mKnockbackResistance;
	
	inline CHealthInfo()
		: mHealth(0.f), mKnockbackResistance(0.f) {}
	
	CHealthInfo(float health, float resistance);
};

#endif // CHEALTHINFO_HPP