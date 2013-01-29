#pragma once

#include <windows.h>
#include <xmmintrin.h>

#define DIST 1
#define XRES 500
#define YRES 500
#define SPHERE_COUNT 10
#define LIGHT_COUNT 1

class CVector2D 
{
public:
	float x,y;
	float GetDistanceToVector(CVector2D b); //Расстояние до вектора
	CVector2D operator + (const CVector2D& right);
	CVector2D operator - (const CVector2D& right);
};

class CVector3D 
{
public:
	float x,y,z;
	
	CVector3D();
	CVector3D(float x, float y, float z);
	void Get2DCoord(CVector2D *vector2d); //Получить 2хмерные координаты точки
	void NormalizeVector(); //Нормализовать вектор
	float GetDistanceToVector(CVector3D b); //Расстояние до вектора
	CVector3D operator + (const CVector3D& right);
	CVector3D operator - (const CVector3D& right);
	float operator * (const CVector3D& right); //Скалярное произведение
	
};

class CSphere
{ 
public:
	CVector3D center;
	float R;
	RGBQUAD color;
	CSphere::CSphere();
	CSphere::CSphere(float x,float y,float z,float r,RGBQUAD color);
};

class CRay
{
public:
	CVector3D center;
	CVector3D vector;

	CRay(CVector3D center, CVector3D vector);
};

CVector3D operator * (float scalar, const CVector3D& vector);

float sqrtSSE(float x); //Корень квадратный на ассемблере