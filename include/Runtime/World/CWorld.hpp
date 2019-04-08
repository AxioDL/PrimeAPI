#ifndef CWORLD_HPP
#define CWORLD_HPP

#include <PrimeAPI.h>

class CWorld
{
public:
	TAreaId IGetAreaId(uint areaAssetId) const;
	TAreaId GetAreaId(uint areaAssetId) const;
};

#endif // CWORLD_HPP