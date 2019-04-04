#ifndef CPHYSICSACTOR_HPP
#define CPHYSICSACTOR_HPP

#include <Runtime/World/CActor.hpp>
#include <Runtime/World/CPhysicsState.hpp>
#include <Math/CVector3f.hpp>

class CPhysicsActor : public CActor 
{
public:
    CPhysicsState GetPhysicsState() const;
	
	DECLARE_FIELD(CVector3f, Velocity, 0x138);
	DECLARE_FIELD(CVector3f, AngularVelocity, 0x144);
};

#endif // CPHYSICSACTOR_HPP
