#ifndef CRELANGLE_HPP
#define CRELANGLE_HPP

class CRelAngle
{
public:
	float mAngle;
	
	// All functions inlined in Metroid Prime
	inline CRelAngle () {}
	inline CRelAngle(float inAngle) : mAngle(inAngle) {}
	
	inline CRelAngle& operator=(float inAngle)
	{
		mAngle = inAngle;
		return *this;
	}
};

#endif // CRELANGLE_HPP