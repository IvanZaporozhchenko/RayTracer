#include <windows.h>
#include <math.h>
#include "Infrastructure.h"

//
__forceinline float sqrtSSE(float x)
{
	__asm
	{
		movss	xmm0, x
		sqrtss	xmm0, xmm0
		movss	x, xmm0
	}

	return x;
}

CVector2D CVector2D::operator +(const CVector2D& right)
{
	CVector2D vector;
	vector.x=this->x+right.x;
	vector.y=this->y+right.y;
	return vector;
}

CVector2D CVector2D::operator -(const CVector2D& right)
{
	CVector2D vector;
	vector.x=this->x-right.x;
	vector.y=this->y-right.y;
	return vector;
}

float CVector2D::GetDistanceToVector(CVector2D b)
{
	return sqrtSSE((this->x-b.x)*(this->x-b.x)+(this->y-b.y)*(this->y-b.y));
}

CVector3D::CVector3D()
{
	this->x=0;
	this->y=0;
	this->z=0;
}

CVector3D::CVector3D(float x, float y, float z)
{
	this->x=x;
	this->y=y;
	this->z=z;
}

CVector3D CVector3D::operator +(const CVector3D& right)
{
	CVector3D vector;
	vector.x=this->x+right.x;
	vector.y=this->y+right.y;
	vector.z=this->z+right.z;
	return vector;
}

CVector3D CVector3D::operator -(const CVector3D& right)
{
	CVector3D vector;
	vector.x=this->x-right.x;
	vector.y=this->y-right.y;
	vector.z=this->z-right.z;
	return vector;
}

float CVector3D::operator *(const CVector3D& right)
{
	float res=this->x*right.x+this->y*right.y+this->z*right.z;	
	return res;
}

void CVector3D::Get2DCoord(CVector2D *vector2d)
{
	vector2d->x=XRES/2+this->x*DIST/(this->z+DIST);
	vector2d->y=YRES/2+this->y*DIST/(this->z+DIST);
}

float CVector3D::GetDistanceToVector(CVector3D b)
{
	return sqrtSSE((this->x-b.x)*(this->x-b.x)+(this->y-b.y)*(this->y-b.y)+(this->z-b.z)*(this->z-b.z));
}

void CVector3D::NormalizeVector()
{
	float mod;
	mod=sqrtSSE(this->x*this->x+this->y*this->y+this->z*this->z);
	this->x/=mod;
	this->y/=mod;
	this->z/=mod;
}

CSphere::CSphere(float x,float y,float z,float r,RGBQUAD color)
{
	CVector3D center(x,y,z);
	this->center=center;
	this->R=r;
	this->color=color;
}

CRay::CRay(CVector3D center, CVector3D vector)
{
	this->center.x=center.x;
	this->center.y=center.y;
	this->center.z=center.z;
	this->vector.x=vector.x;
	this->vector.y=vector.y;
	this->vector.z=vector.z;
}


