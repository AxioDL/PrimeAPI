#ifndef CVECTOR3F_HPP
#define CVECTOR3F_HPP

class CInputStream;
class CRelAngle;
class CInputStream;
class COutputStream;

struct CVector3f
{
public:
    float X, Y, Z;
	
	inline CVector3f() {}
    inline CVector3f(float inX, float inY, float inZ)
		: X(inX), Y(inY), Z(inZ) {}
	CVector3f(CInputStream&);
		
	bool IsEqu(const CVector3f&, float threshold) const;
	bool CanBeNormalized() const;
	CVector3f AsNormalized() const;
	float Magnitude() const;
	void Normalize();
	void PutTo(COutputStream&) const;
	
	static float GetAngleDiff(const CVector3f&, const CVector3f&);
	static CVector3f Slerp(const CVector3f&, const CVector3f&, const CRelAngle&);

	static CVector3f sZeroVector;
	static CVector3f sUpVector;
	static CVector3f sDownVector;
	static CVector3f sLeftVector;
	static CVector3f sRightVector;
	static CVector3f sForwardVector;
	static CVector3f sBackVector;
	
	// These functions are inlined in Metroid Prime
	inline float Dot(const CVector3f& rhs) const
	{
		return (X * rhs.X) + (Y * rhs.Y) + (Z * rhs.Z);
	}
	
	inline CVector3f Cross(const CVector3f& rhs) const
	{
		return CVector3f(
			Y * rhs.Z - Z * rhs.Y,
			Z * rhs.X - X * rhs.Z,
			X * rhs.Y - Y * rhs.X
		);
	}
};

#endif // CVECTOR3F_HPP
