#ifndef CPLAYER_HPP
#define CPLAYER_HPP

#include <Runtime/World/CPhysicsActor.hpp>

class CPlayer : public CPhysicsActor
{
public:
	void Teleport(const CTransform4f& newTransform, CStateManager&, bool resetBallCam);
};

#endif // CPLAYER_HPP
