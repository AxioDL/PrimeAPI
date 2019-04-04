#ifndef CAXISANGLE_HPP
#define CAXISANGLE_HPP

struct CAxisAngle : CVector3f
{
    inline CAxisAngle(float x, float y, float z) : CVector3f(x, y, z) {}
};

#endif // CAXISANGLE_HPP