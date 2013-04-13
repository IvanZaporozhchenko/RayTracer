#pragma once
#include <Windows.h>
#include "Infrastructure.h"

//************************************
// Method:    RayTracer
// FullName:  ����������� �����
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: HBITMAP bitmap - ������������� bitmap �� ������� ������������ ��������
// Parameter: CSphere S[] - ������ �� ����, ���� ��� ���
// Parameter: CRVector3D lightsArray[] - ������ �� �������� ���������� �����
//************************************
void RayTracer(HBITMAP bitmap,CSphere S[],CVector3D lights[], CBox sceneBox);