#ifndef CTRANSFORM4F_H
#define CTRANSFORM4F_H

#include <Math/CVector3f.hpp>

class CInputStream;
class CMatrix3f;
class CRelAngle;
class CUnitVector3f;

class CTransform4f
{
public:
    float matrix[12];
	
	// Functions
	inline CTransform4f() {}
	CTransform4f(const CVector3f&, const CVector3f&, const CVector3f&, const CVector3f&);
	CTransform4f(const CMatrix3f&, const CVector3f&);
	CTransform4f(CInputStream&);
	
	CTransform4f GetInverse() const;
	CVector3f TransposeRotate(const CVector3f&) const;
	CVector3f Rotate(const CVector3f&) const;
	void SetRotation(const CTransform4f&);
	void SetRotation(const CMatrix3f&);
	CTransform4f GetRotation() const;
	CTransform4f GetQuickInverse() const;
	void ScaleBy(float);
	void MultiplyIgnoreTranslation(const CTransform4f&) const;
	CMatrix3f BuildMatrix3f() const;
	void Translate(const CVector3f&);
	void Translate(float, float, float);
	void Scale(const CVector3f&);
	void Scale(float, float, float);
	void Scale(float);
	void Orthonormalize();
	void RotateLocalX(const CRelAngle&);
	void RotateLocalY(const CRelAngle&);
	void RotateLocalZ(const CRelAngle&);
	void RotateX(const CRelAngle&);
	void RotateY(const CRelAngle&);
	void RotateZ(const CRelAngle&);
	
	CTransform4f operator*(const CTransform4f&) const;
	CVector3f operator*(const CVector3f&) const;
	CTransform4f& operator=(const CTransform4f&);
	
	static CTransform4f FromColumns(const CVector3f&, const CVector3f&, const CVector3f&, const CVector3f&);
	static CTransform4f MakeRotationsBasedOnY(const CUnitVector3f&);
	static CTransform4f LookAt(const CVector3f& origin, const CVector3f& targetLocation, const CVector3f& upAxis);
	
	static CTransform4f sIdentity;
	
	// This function is inlined in Metroid Prime
    inline CVector3f GetTranslation() const
	{
		return CVector3f(matrix[3], matrix[7], matrix[11]);
	}
};

#endif // CTRANSFORM4F_H