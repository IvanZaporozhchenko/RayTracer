#pragma once
#include <Windows.h>
#include "Infrastructure.h"

//************************************
// Method:    RayTracer
// FullName:  Трассировка лучей
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: HBITMAP bitmap - идентификатор bitmap на который отрендерится картинка
// Parameter: CSphere S[] - массив из сфер, пока что так
// Parameter: CRVector3D lightsArray[] - массив из точечных источников света
//************************************
void RayTracer(HBITMAP bitmap,CSphere S[],CVector3D lights[], CBox sceneBox);