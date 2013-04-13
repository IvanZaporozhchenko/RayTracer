#include "RayTracer.h"
#include "Infrastructure.h"
#include <windows.h>
#include <math.h>
#include <float.h>

#define BACKGROUND_COLOR RGB(20,20,20)

COLORREF buffer[YRES][XRES]; //Буфер пикселей изображения

void NextVoxel(int x, int y, int z, CBox & sceneBox)
{
	CVector3D t, t2;
	t.x = (float)x/(float)CELL_NUMBER;
	t.y = (float)y/(float)CELL_NUMBER;
	t.z = (float)z/(float)CELL_NUMBER;

	t2.x = (float)(x+1)/(float)CELL_NUMBER;
	t2.y = (float)(y+1)/(float)CELL_NUMBER;
	t2.z = (float)(z+1)/(float)CELL_NUMBER;

	CBox smallBox;
	smallBox.leftBottomPoint.x = sceneBox.leftBottomPoint.x + t.x*sceneBox.length;
	smallBox.leftBottomPoint.y = sceneBox.leftBottomPoint.y + t.y*sceneBox.height;
	smallBox.leftBottomPoint.z = sceneBox.leftBottomPoint.z + t.z*sceneBox.width;

	smallBox.length = t2.x*sceneBox.length;
	smallBox.height = t2.y*sceneBox.height;
	smallBox.width = t2.z*sceneBox.width;
}

bool * FujimotoTraverse3D(CRay ray, CBox sceneBox, CSphere SArray[])
{
	//будущий результат обнуляем
	bool result[SPHERE_COUNT];
	for (int i=0;i<SPHERE_COUNT;i++)
	{
		result[i]=false;
	}
	float  t_min=0;

	if(!sceneBox.Intersect(ray, t_min))
		return result;

	if(t_min < 0)
		t_min = 0;

	float startX = ray.center.x + t_min*ray.vector.x;
	float startY = ray.center.y + t_min*ray.vector.y;
	float startZ = ray.center.z + t_min*ray.vector.z;

	int cells=CELL_NUMBER;
	int x = (int)((startX - sceneBox.leftBottomPoint.x)/(sceneBox.length)*cells);
	int y = (int)((startY - sceneBox.leftBottomPoint.y)/(sceneBox.height)*cells);
	int z = (int)((startZ - sceneBox.leftBottomPoint.z)/(sceneBox.width)*cells);

	if(x == cells) x--;
	if(y == cells) y--;
	if(z == cells) z--;

	float tVoxelX, tVoxelY,tVoxelZ; 
	int stepX, stepY,stepZ;
  
	if(ray.vector.x > 0)
	{
		tVoxelX  = (float)(x+1)/(float)CELL_NUMBER;
		stepX = 1;
	}
	else
	{
		tVoxelX  = (float)x/(float)CELL_NUMBER;
		stepX = -1;
	}

	if(ray.vector.y > 0)
	{
		tVoxelY = (float)(y+1)/(float)CELL_NUMBER;
		stepY = 1;
	}
	else
	{
		tVoxelY = (float)y/(float)CELL_NUMBER;
		stepY = -1;
	}

	if(ray.vector.z > 0)
	{
		tVoxelZ = (float)(z+1)/(float)CELL_NUMBER;
		stepZ = 1;
	}
	else
	{
		tVoxelZ = (float)z/(float)CELL_NUMBER;
		stepZ = -1;
	}

	float voxelMaxX = sceneBox.leftBottomPoint.x + tVoxelX*sceneBox.length;
	float voxelMaxY = sceneBox.leftBottomPoint.y + tVoxelY*sceneBox.height;
	float voxelMaxZ = sceneBox.leftBottomPoint.z + tVoxelZ*sceneBox.width;

	float tMaxX = t_min + (voxelMaxX - startX)/ray.vector.x;
	float tMaxY = t_min + (voxelMaxY - startY)/ray.vector.y;
	float tMaxZ = t_min + (voxelMaxZ - startZ)/ray.vector.z;

	const float voxelSize = (sceneBox.length)/CELL_NUMBER;
	const float tDeltaX = voxelSize/fabs(ray.vector.x);
	const float tDeltaY = voxelSize/fabs(ray.vector.y);
	const float tDeltaZ = voxelSize/fabs(ray.vector.z);
	//NextVoxel(x, y, box);

	while(x < cells && x >= 0 && 
		y < cells && y >= 0 && z<cells && z>=0)
	{
		//Определяем какие сферы в данном боксе
		for (int i=0;i<SPHERE_COUNT;i++)
		{
			if (x>=SArray[i].boxIndexes.minX && 
				x<=SArray[i].boxIndexes.maxX &&
				y>=SArray[i].boxIndexes.minY &&
				y<=SArray[i].boxIndexes.maxY &&
				z>=SArray[i].boxIndexes.minZ &&
				z<=SArray[i].boxIndexes.maxZ)
			{
				result[i]=true;
			}
		}
    
		if(tMaxX <= tMaxY && tMaxX <= tMaxZ)
		{
			tMaxX += tDeltaX;
			x += stepX;
		}
		else if(tMaxY <= tMaxZ && tMaxY <= tMaxX)
		{
			tMaxY += tDeltaY;
			y += stepY;
		}
		else
		{
			tMaxZ += tDeltaZ;
			z += stepZ;
		}
	}
	return result;
}


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
bool checkIntersection(CRay ray, CSphere sphereArray[], CVector3D * hit, int * n, CBox sceneBox)
{
	float a,b,c,D,t,tmin;
	bool intersectionFinded=false;
	tmin=30000;

	bool * fujimotoSpheres =FujimotoTraverse3D(ray,sceneBox,sphereArray);
	//Проверка на столкновение
	//Формулы сзяты с данного сайта http://www.cs.huji.ac.il/course/2006/cg/slides/raytracing.pdf
	//Решаем квадратное уравнение 
	for (int i=0; i<SPHERE_COUNT; i++)
	{	
		if (fujimotoSpheres[i]==false)
		{
			continue;
		}
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

RGBQUAD TraceOneRay(CRay ray, CSphere S[], CVector3D lightsArray[],int level, CBox sceneBox)
{
	int n;
	RGBQUAD currentColor;
	currentColor.rgbBlue=20;
	currentColor.rgbGreen=20;
	currentColor.rgbRed=20;
	CVector3D hit,light,lightdir,snormal;

	if(checkIntersection(ray,S,&hit,&n,sceneBox)) 
	{	
		currentColor.rgbBlue=0;
		currentColor.rgbGreen=0;
		currentColor.rgbRed=0;
		//Вычисляем нормаль
		snormal.x=(hit.x-S[n].center.x);
		snormal.y=(hit.y-S[n].center.y);
		snormal.z=(hit.z-S[n].center.z);
		snormal.NormalizeVector();		
		//Проверяем каждый источник света
		for (int lightIndex=0; lightIndex<LIGHT_COUNT; lightIndex++)
		{
			light=lightsArray[lightIndex];
			lightdir.x=light.x-hit.x;
			lightdir.y=light.y-hit.y;
			lightdir.z=light.z-hit.z;
			lightdir.NormalizeVector();

			bool inShadow=false;
			//Находим коэффициент освещения
			float coefLight=snormal*lightdir;
			//Если коэффициент больше 0 то свет попадает
			if(coefLight>0){
				//луч от источника света к месту пересечения с объектом
				CRay lightRay(hit,lightdir);
				//Проверяем не затенен ли данный объект другим объектом
				for (int index=0; index<SPHERE_COUNT; index++)
				{
					
					/*if(index==n) 
						continue;*/
					CVector3D lightHit;
					int num;
					if (checkIntersection(lightRay,S,&lightHit,&num,sceneBox))
					{
						inShadow=true;
						break;
					}
				}
				if(!inShadow)
				{		
					//Создаем отражающий луч
					CVector3D reflectionVector=(ray.vector-2*(((ray.vector*snormal))*snormal));
					CRay reflectionRay(hit,reflectionVector);
					RGBQUAD reflectionColor;
					float coef=1;
					for (int i=0; i<level; i++)
					{
						//coef*=reflectionVector*ray.vector;
						coef*=0.5;
					}
					reflectionColor.rgbBlue=0;
					reflectionColor.rgbGreen=0;
					reflectionColor.rgbRed=0;
					if(level<7)
					{
						reflectionColor=TraceOneRay(reflectionRay,S,lightsArray,++level,sceneBox);
					}
					
					//Свет по Ламберту
					if (reflectionColor.rgbRed==20 && reflectionColor.rgbGreen==20 && reflectionColor.rgbBlue==20)
					{
						reflectionColor.rgbBlue=0;
						reflectionColor.rgbGreen=0;
						reflectionColor.rgbRed=0;
					}
					else
					{
						int k=1;
					}
					if ((int)currentColor.rgbRed+coefLight*S[n].color.rgbRed*0.5+reflectionColor.rgbRed*coef>=255)
					{
						currentColor.rgbRed=255;
					}
					else
					{
						currentColor.rgbRed+=coefLight*S[n].color.rgbRed*0.5+reflectionColor.rgbRed*coef;
					}					
					if ((int)currentColor.rgbGreen+coefLight*S[n].color.rgbGreen*0.5+reflectionColor.rgbGreen*coef>=255)
					{
						currentColor.rgbGreen=255;
					}
					else
					{
						currentColor.rgbGreen+=coefLight*S[n].color.rgbGreen*0.5+reflectionColor.rgbGreen*coef;
					}
					if ((int)currentColor.rgbBlue+coefLight*S[n].color.rgbBlue*0.5+reflectionColor.rgbBlue*coef>=255)
					{
						currentColor.rgbBlue=255;
					}
					else
					{
						currentColor.rgbBlue+=coefLight*S[n].color.rgbBlue*0.5+reflectionColor.rgbBlue*coef;
					}
					
					//Блинн-Фонг взято с http://www.codermind.com/articles/Raytracer-in-C++-Part-II-Specularity-post-processing.html
					float fViewProjection=ray.vector*snormal;
					CVector3D blinnVector=lightRay.vector-ray.vector;
					float temp=blinnVector*blinnVector;
					if (temp!=0)
					{
						float blinn=1/sqrtSSE(temp) * max(coefLight-fViewProjection,0.0f);
						blinn=coef*powf(blinn,20);
						if ((int)currentColor.rgbRed+blinn*S[n].color.rgbRed>=255)
						{
							currentColor.rgbRed=255;
						}
						else
						{
							currentColor.rgbRed+=blinn*S[n].color.rgbRed;
						}
						if ((int)currentColor.rgbGreen+blinn*S[n].color.rgbGreen>=255)
						{
							currentColor.rgbGreen=255;
						}
						else
						{
							currentColor.rgbGreen+=blinn*S[n].color.rgbGreen;
						}
						if ((int)currentColor.rgbBlue+blinn*S[n].color.rgbBlue>=255)
						{
							currentColor.rgbBlue=255;
						}
						else
						{
							currentColor.rgbBlue+=blinn*S[n].color.rgbBlue;
						}						
					}
				}
			}
		}		
	}	
	return currentColor;
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
void RayTracer(HBITMAP bitmap,CSphere S[], CVector3D lightsArray[], CBox sceneBox)
{
	CVector2D tt;
	int i,j;
	float a,b,c,D,t,coefLight;
	CVector3D rayVector,snormal,lightdir,light;
	//Выбрасываем каждый луч
	for(i=-YRES/2;i<YRES/2;i++)
	{
		for(j=-XRES/2;j<XRES/2;j++)
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
			RGBQUAD currentColor=TraceOneRay(ray,S,lightsArray,1,sceneBox);
			buffer[i+YRES/2][j+XRES/2]= RGB(currentColor.rgbRed,currentColor.rgbGreen,currentColor.rgbBlue);
			
		}
	}
	SetBitmapBits(bitmap,XRES*YRES*sizeof(buffer[0][0]),buffer);
	memset(buffer,25,XRES*YRES*sizeof(buffer[0][0]));
}