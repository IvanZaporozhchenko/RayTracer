#include "RayTracer.h"
#include "Infrastructure.h"
#include <windows.h>

#define BACKGROUND_COLOR RGB(20,20,20)

COLORREF buffer[YRES][XRES];

//************************************
// Method:    checkIntersection
// FullName:  Проверить пересечение сферы и луча
// Access:    public 
// Returns:   bool true если пересекает, иначе false
// Qualifier:
// Parameter: CRay ray - луч
// Parameter: CSphere S - сфера
// Parameter: CRVector3D * hit - точка пересечения
// Parameter: int * n - номер объекта с которым находится ближайшее пересечение
//************************************
bool checkIntersection(CRay ray, CSphere sphereArray[], CVector3D * hit, int * n)
{
	float a,b,c,D,t,tmin;
	bool intersectionFinded=false;
	tmin=30000;
	//Проверка на столкновение
	//Формулы сзяты с данного сайта http://www.cs.huji.ac.il/course/2006/cg/slides/raytracing.pdf
	//Решаем квадратное уравнение 
	for (int i=0; i<2; i++)
	{	
		CSphere S=sphereArray[i];
		a = 1;
		b=2*((ray.center-S.center)*ray.vector);           
		c=((ray.center-S.center)*(ray.center-S.center))-S.R*S.R;

		D=b*b - 4*c; 

		//Если дискриминант больше 0 значит пересечение есть
		if(D>0) 
		{	
			t=-1;
			//находим положительный корень
			float t1=(-b-sqrtSSE(D))/(2);
			float t2=(-b-sqrtSSE(D))/(2);

			if (t1>=0.1f)
			{
				t=t1;
			}
			if (t2>=0.1f)
			{
				t=t2;
			}
			//Если нет положительного корня
			if (t<0)
			{
				continue;
			}
			//tmin - минимальное расстояние до пересечение
			if (tmin>t)
			{
				tmin=t;
				*n=i;
				//Находим пересечение
				hit->x = ray.center.x+t*ray.vector.x;
				hit->y = ray.center.y+t*ray.vector.y; 
				hit->z = ray.center.z+t*ray.vector.z;
				intersectionFinded=true;
			}
		}
	}
	return intersectionFinded;
}

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
void RayTracer(HBITMAP bitmap,CSphere S[], CVector3D lightsArray[])
{
	CVector2D tt;
	int i,j;
	float a,b,c,D,t,coefLight;
	CVector3D rayVector,snormal,lightdir,light;
	//Выбрасываем каждый луч
	for(i=-500/2;i<500/2;i++)
	{
		for(j=-500/2;j<500/2;j++)
		{
			//Изначально задаем цвет фона
			buffer[i+YRES/2][j+XRES/2]=BACKGROUND_COLOR;
			//Вычиляем вектор для луча
			rayVector.x=(j)*2.0f/XRES;		
			rayVector.y=(i)*2.0f/YRES;		
			rayVector.z=DIST;	
			//Нормализуем вектор
			rayVector.NormalizeVector();
			CVector3D hit;
			CVector3D eye(0,0,0);
			CRay ray(eye,rayVector);
			int n;
			if(checkIntersection(ray,S,&hit,&n)) 
			{	
				//Вычисляем нормаль
				snormal.x=(hit.x-S[n].center.x);
				snormal.y=(hit.y-S[n].center.y);
				snormal.z=(hit.z-S[n].center.z);
				snormal.NormalizeVector();
				RGBQUAD currentColor;
				currentColor.rgbBlue=0;
				currentColor.rgbGreen=0;
				currentColor.rgbRed=0;
				//Проверяем каждый источник света
				for (int lightIndex=0; lightIndex<2; lightIndex++)
				{
					light=lightsArray[lightIndex];
					lightdir.x=light.x-hit.x;
					lightdir.y=light.y-hit.y;
					lightdir.z=light.z-hit.z;
					lightdir.NormalizeVector();
						
						
					bool inShadow=false;
					//Находим коэффициент освещения
					coefLight=snormal*lightdir;
					//Если коэффициент больше 0 то свет попадает
					if(coefLight>0){
						//Проверяем не затенен ли данный объект другим объектом
						for (int index=0; index<2; index++)
						{
							CRay lightRay(hit,lightdir);
							if(index==n) 
								continue;
							CVector3D lightHit;
							if (checkIntersection(lightRay,S,&lightHit,NULL))
							{
								inShadow=true;
								break;
							}
						}
						if(!inShadow)
						{												
							//Если значение RGB > чем текущее то заменяем его
							currentColor.rgbRed+=coefLight*S[n].color.rgbRed*0.5;
							currentColor.rgbGreen+=coefLight*S[n].color.rgbGreen*0.5;
							currentColor.rgbBlue+=coefLight*S[n].color.rgbBlue*0.5;
						}
					}
				}		
				//Записываем в буфер конечный цвет
				buffer[i+YRES/2][j+XRES/2]= RGB(currentColor.rgbRed,currentColor.rgbGreen,currentColor.rgbBlue);
			}	//(kd*factor*R, kd*factor*G, kd*factor*B).+ (ka*R, ka*G, Ka*B).			
		}
	}
	SetBitmapBits(bitmap,XRES*YRES*sizeof(buffer[0][0]),buffer);
	memset(buffer,25,XRES*YRES*sizeof(buffer[0][0]));
}