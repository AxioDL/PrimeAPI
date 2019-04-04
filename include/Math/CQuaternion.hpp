#ifndef CQUATERNION_HPP
#define CQUATERNION_HPP

class CInputStream;
class CMatrix3f;
class CRelAngle;
class CUnitVector3f;

class CQuaternion
{
public:
    float W;
    float X;
    float Y;
    float Z;
	
	inline CQuaternion() {}
    inline CQuaternion(float inW, float inX, float inY, float inZ)
		: W(inW), X(inX), Y(inY), Z(inZ) {}
	CQuaternion(CInputStream&);
	
	CVector3f Transform(const CVector3f&) const;
	CQuaternion BuildNormalized() const;
	CQuaternion BuildEquivalent() const;
	CRelAngle AngleFrom(const CQuaternion&) const;
	CTransform4f BuildTransform4f() const;
	CTransform4f BuildTransform4f(const CVector3f&) const;
	CMatrix3f BuildTransform() const;
	
	CQuaternion operator*(const CQuaternion&) const;
	
	static CQuaternion AxisAngle(const CUnitVector3f&, const CRelAngle&);
	static CQuaternion XRotation(const CRelAngle&);
	static CQuaternion YRotation(const CRelAngle&);
	static CQuaternion ZRotation(const CRelAngle&);
	static CQuaternion SlerpLocal(const CQuaternion&, const CQuaternion&, float);
	static CQuaternion LookAt(const CUnitVector3f&, const CUnitVector3f&, const CRelAngle&);
	static CQuaternion ShortestRotationArc(const CVector3f&, const CVector3f&);
	static CQuaternion Slerp(const CQuaternion&, const CQuaternion&, float);
	static CQuaternion FromMatrix(const CTransform4f&);
	static CQuaternion FromMatrix(const CMatrix3f&);
	static CQuaternion FromMatrixRows(const CVector3f&, const CVector3f&, const CVector3f&);
	
	static CQuaternion sNoRotation;
};

#endif // CQUATERNION_HPP