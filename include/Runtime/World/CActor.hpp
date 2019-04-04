#ifndef CACTOR_HPP
#define CACTOR_HPP

#include <PrimeAPI.h>
#include <Runtime/World/CEntity.hpp>
#include <Math/CTransform4f.hpp>

class CActor : public CEntity
{
public:
	DECLARE_FIELD(CTransform4f, Transform, 0x34);
};

#endif // CACTOR_HPP
