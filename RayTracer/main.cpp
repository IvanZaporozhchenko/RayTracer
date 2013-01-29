#include <windows.h>
#include "Infrastructure.h"
#include "RayTracer.h"

LRESULT _stdcall WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

int _stdcall WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Данные для подсчета фпс
	DWORD FrameCnt=0;    // кол-во кадров
	float TimeElapsed=0; // промежуток времени
	float FPS=0;         // наш фпс
	float oldTime;
	float newTime;
	float deltatime;
	char * buf=new char[20];

	//Инициализация окна
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
		WS_OVERLAPPEDWINDOW,100,100,600,600,
		NULL,NULL,hInstance,NULL);

	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	MSG msg;
	DWORD bits[500][500];
	HBITMAP hBitmap;
	RGBQUAD color;
	float x,y,z,r;
	//Инициализация сцены
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
	
	CVector3D lightArray[LIGHT_COUNT];

	for (int i=0; i<LIGHT_COUNT; i++)
	{
		lightArray[i].x=rand()%5;
		lightArray[i].y=rand()%5;
		lightArray[i].z=rand()%5;
	}

	
	hBitmap=CreateBitmap(500,500,1,32,bits);
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
			oldTime = GetTickCount(); // записываем в олдтайм время перед началом всех операций
			
			//Рендеринг
			
			
			RayTracer(hBitmap,SArray,lightArray);

			HDC hdc = GetDC(hWnd); // Get the DC from that HWND
			HDC hdcMem = CreateCompatibleDC(hdc);   
			SelectObject(hdcMem, hBitmap);
			BitBlt(hdc, 0, 0, 500, 500, hdcMem, 0, 0, SRCCOPY);
			DeleteDC(hdcMem);
			DeleteDC(hdc); // Delete the DC
			//Подсчет FPS
			FrameCnt++; // c каждым кадром увеличивается на 1, т.е. это кол-во кадров кот. мы разделим на промежуток времени
			newTime = GetTickCount(); // записываем в ньютайм время после всех выполненных операциях
			deltatime = newTime - oldTime; // узнаем сколько времени проходит от начала до конца
			TimeElapsed += deltatime; // и прибавляем эту разницу к таймэлапседу

			if(TimeElapsed >= 1000.0) // если в таймэлапседе накопилось 0.5 сек, то
			{
				FPS = 1000*(float)FrameCnt / TimeElapsed; // делим кол-во кадров прошедшее за полсекунды на таймэлапсед(равный примерно 0.5 сек)
				TimeElapsed = 0.0f; // обнуляем таймэлапсед, для следующего подсчета
				FrameCnt = 0; // и обнуляем кол-во кадров
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