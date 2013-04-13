#include <windows.h>
#include <float.h>
#include "Infrastructure.h"
#include "RayTracer.h"

LRESULT _stdcall WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

CBox boxForSphere(CSphere sphere)
{
	return CBox(
			sphere.center.x-sphere.R,
			sphere.center.y-sphere.R,
			sphere.center.z-sphere.R,
			sphere.R*2,
			sphere.R*2,
			sphere.R*2);
}

CBox buildHierarhicalGrid(CSphere sphereArray[])
{
	CVector3D allPoints[SPHERE_COUNT*8];
	float xMin=FLT_MAX,
		xMax=FLT_MIN,
		yMin=FLT_MAX,
		yMax=FLT_MIN,
		zMin=FLT_MAX,
		zMax=FLT_MIN;
	for (int sphereIndex=0;sphereIndex<SPHERE_COUNT;sphereIndex++)
	{
		CBox boxForCurrentSphere=boxForSphere(sphereArray[sphereIndex]);
		for (int i=0;i<8;i++)
		{
			allPoints[sphereIndex*8+i]=boxForCurrentSphere.Points()[i];
			if (allPoints[sphereIndex*8+i].x<xMin)
			{
				xMin=allPoints[sphereIndex*8+i].x;
			}
			if (allPoints[sphereIndex*8+i].y<yMin)
			{
				yMin=allPoints[sphereIndex*8+i].y;
			}
			if (allPoints[sphereIndex*8+i].z<zMin)
			{
				zMin=allPoints[sphereIndex*8+i].z;
			}
			if (allPoints[sphereIndex*8+i].x>xMax)
			{
				xMax=allPoints[sphereIndex*8+i].x;
			}
			if (allPoints[sphereIndex*8+i].y>yMax)
			{
				yMax=allPoints[sphereIndex*8+i].y;
			}
			if (allPoints[sphereIndex*8+i].z>zMax)
			{
				zMax=allPoints[sphereIndex*8+i].z;
			}
		}
	}
	CBox bigSceneBox(xMin,yMin,zMin,xMax-xMin,yMax-yMin,zMax-zMin);
	return bigSceneBox;
}


int _stdcall WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//������ ��� �������� ���
	DWORD FrameCnt=0;    // ���-�� ������
	float TimeElapsed=0; // ���������� �������
	float FPS=0;         // ��� ���
	float oldTime;
	float newTime;
	float deltatime;
	char * buf=new char[20];

	//������������� ����
	WNDCLASS wc;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(6);
	wc.lpszMenuName = 0;
	wc.lpszClassName = "class";

	RegisterClass(&wc);
	HWND hWnd = CreateWindow(
		"class",
		"RayTracer",
		WS_OVERLAPPEDWINDOW,0,0,800,800,
		NULL,NULL,hInstance,NULL);

	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	MSG msg;
	DWORD bits[XRES][YRES];
	HBITMAP hBitmap;
	RGBQUAD color;
	float x,y,z,r;
	//������������� �����
	CSphere SArray[SPHERE_COUNT];
	for (int i=0; i<SPHERE_COUNT; i++)
	{
		color.rgbRed=rand()%255;
		color.rgbBlue=rand()%255;
		color.rgbGreen=rand()%255;
		x=rand()%10-5;
		y=rand()%10-5;
		z=rand()%5+5;
		r=(rand()%5+5)/10.0f;
		SArray[i]=CSphere(x,y,z,r,color);	
	}
	//������� �������������� ���� ��� �����
	CBox sceneBox =	buildHierarhicalGrid(SArray);

	//������� ����� � ������� ��������� ����� � ���������� ��
	for (int i=0; i<SPHERE_COUNT; i++)
	{
		int cells=CELL_NUMBER;
		CBox boxForCurrentSphere = boxForSphere(SArray[i]); 
		SArray[i].boxIndexes.minX = (int)((boxForCurrentSphere.leftBottomPoint.x - sceneBox.leftBottomPoint.x)/(sceneBox.length)*cells);
		SArray[i].boxIndexes.maxX = (int)((boxForCurrentSphere.Points()[7].x - sceneBox.leftBottomPoint.x)/(sceneBox.length)*cells);
		SArray[i].boxIndexes.minY = (int)((boxForCurrentSphere.leftBottomPoint.y - sceneBox.leftBottomPoint.y)/(sceneBox.height)*cells);
		SArray[i].boxIndexes.maxY = (int)((boxForCurrentSphere.Points()[7].y - sceneBox.leftBottomPoint.y)/(sceneBox.height)*cells);
		SArray[i].boxIndexes.minZ = (int)((boxForCurrentSphere.leftBottomPoint.z - sceneBox.leftBottomPoint.z)/(sceneBox.width)*cells);
		SArray[i].boxIndexes.maxZ = (int)((boxForCurrentSphere.Points()[7].z - sceneBox.leftBottomPoint.z)/(sceneBox.width)*cells);
	}
	
	CVector3D lightArray[LIGHT_COUNT];

	for (int i=0; i<LIGHT_COUNT; i++)
	{
		lightArray[i].x=rand()%5;
		lightArray[i].y=rand()%5;
		lightArray[i].z=rand()%5;
	}

	
	hBitmap=CreateBitmap(XRES,YRES,1,32,bits);
	while(true)
	{
		if(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;  
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			oldTime = GetTickCount(); // ���������� � ������� ����� ����� ������� ���� ��������
			
			//���������
			
			
			RayTracer(hBitmap,SArray,lightArray,sceneBox);

			HDC hdc = GetDC(hWnd); // Get the DC from that HWND
			HDC hdcMem = CreateCompatibleDC(hdc);   
			SelectObject(hdcMem, hBitmap);
			BitBlt(hdc, 0, 0, XRES, YRES, hdcMem, 0, 0, SRCCOPY);
			DeleteDC(hdcMem);
			DeleteDC(hdc); // Delete the DC
			//������� FPS
			FrameCnt++; // c ������ ������ ������������� �� 1, �.�. ��� ���-�� ������ ���. �� �������� �� ���������� �������
			newTime = GetTickCount(); // ���������� � ������� ����� ����� ���� ����������� ���������
			deltatime = newTime - oldTime; // ������ ������� ������� �������� �� ������ �� �����
			TimeElapsed += deltatime; // � ���������� ��� ������� � ������������

			if(TimeElapsed >= 1000.0) // ���� � ������������ ���������� 0.5 ���, ��
			{
				FPS = 1000*(float)FrameCnt / TimeElapsed; // ����� ���-�� ������ ��������� �� ���������� �� �����������(������ �������� 0.5 ���)
				TimeElapsed = 0.0f; // �������� �����������, ��� ���������� ��������
				FrameCnt = 0; // � �������� ���-�� ������
			}
			_gcvt(FPS,19,buf);
			SetWindowText(hWnd,buf);
		}
	}
	DeleteObject(hBitmap);
	return 0;
}

LRESULT _stdcall WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd,msg,wParam,lParam);
	}
}