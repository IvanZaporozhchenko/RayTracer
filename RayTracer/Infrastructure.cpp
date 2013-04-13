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

CVector3D operator * (float scalar, const CVector3D &vector)
{
	CVector3D res;
	res.x=vector.x*scalar;
	res.y=vector.y*scalar;
	res.z=vector.z*scalar;
	return res;
}

CSphere::CSphere()
{
	
}

CSphere::CSphere(float x,float y,float z,float r,RGBQUAD color)
{
	CVector3D center(x,y,z);
	this->center=center;
	this->R=r;
	this->color=color;
}

CBox::CBox()
{
	
}

CBox::CBox(float x,float y,float z,float length,float height,float width)
{
	CVector3D center(x,y,z);
	this->leftBottomPoint=center;
	this->height=height;
	this->length=length;
	this->width=width;
}

CVector3D * CBox::Points()
{
	CVector3D points[8];
	points[0]=leftBottomPoint;
	points[1]=CVector3D(leftBottomPoint.x+length,leftBottomPoint.y,leftBottomPoint.z);
	points[2]=CVector3D(leftBottomPoint.x,leftBottomPoint.y+height,leftBottomPoint.z);
	points[3]=CVector3D(leftBottomPoint.x,leftBottomPoint.y,leftBottomPoint.z+width);
	points[4]=CVector3D(leftBottomPoint.x+length,leftBottomPoint.y+height,leftBottomPoint.z);
	points[5]=CVector3D(leftBottomPoint.x+length,leftBottomPoint.y,leftBottomPoint.z+width);
	points[6]=CVector3D(leftBottomPoint.x,leftBottomPoint.y+height,leftBottomPoint.z+width);
	points[7]=CVector3D(leftBottomPoint.x+length,leftBottomPoint.y+height,leftBottomPoint.z+width);
	return points;
}

// Optimized method взято с http://www.cs.utah.edu/~awilliam/box/box.pdf
bool CBox::Intersect(CRay &r, float &tmin) 
{
	CVector3D bounds[2];
	bounds[0]=leftBottomPoint;
	bounds[1]=Points()[7];
	float tmax, tymin, tymax, tzmin, tzmax;
	tmin = (bounds[r.sign[0]].x - r.center.x) * r.inverted_direction.x;
	tmax = (bounds[1-r.sign[0]].x - r.center.x) * r.inverted_direction.x;
	tymin = (bounds[r.sign[1]].y - r.center.y) * r.inverted_direction.y;
	tymax = (bounds[1-r.sign[1]].y - r.center.y) * r.inverted_direction.y;
	if ( (tmin > tymax) || (tymin > tmax) )
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	tzmin = (bounds[r.sign[2]].z - r.center.z) * r.inverted_direction.z;
	tzmax = (bounds[1-r.sign[2]].z - r.center.z) * r.inverted_direction.z;
	if ( (tmin > tzmax) || (tzmin > tmax) )
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;	
	return ( (tmin <= tmax) && (tmax > 0) );
}

CRay::CRay(CVector3D center, CVector3D vector)
{
	this->center.x=center.x;
	this->center.y=center.y;
	this->center.z=center.z;
	this->vector.x=vector.x;
	this->vector.y=vector.y;
	this->vector.z=vector.z;
	this->inverted_direction=CVector3D(1/vector.x,1/vector.y,1/vector.z);
	sign[0] = (inverted_direction.x < 0);
	sign[1] = (inverted_direction.y < 0);
	sign[2] = (inverted_direction.z < 0);
}


