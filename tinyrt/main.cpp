#include "definition.h"
#include "intersect.h"
#include "shade.h"

#include "bmp.h"
#include "rtmath.h"

#include <math.h>
#include <float.h>

#include <iostream>

//Point light = { 0, 5, 0 };		// ���Դ��λ��
                         // ��������ϵ

Lights lights; // ��Դ����



void initLights()
{
    // ���Ϸ��ĵ��Դ
    PointLight pointLight = { 0, 5, 0,  255, 0,0};
    add_PointLight(&lights, pointLight);

    DirectionLight dLight = { -1, 0, 0,    30, 30, 30}; // ����ɫ
    add_DirectiontLight(&lights, dLight);

    int i = 23;
}

bool destroyLights()
{
    return false;
}


double distance =  1.5; //  �۾����Ӵ��ľ���
const static int width = 640  ;  // 640 pixel    0.64m
const static int height = 480  ;

// view plane  ��ָ����ʽ�� eye space�� ��z ��ֱͨ��view plane
// view plane �Ŀ����eye space y �ᣨup��ƽ��
// x,y ����������, һ������0.1cm����view plane ��64cm�� ������25����Ļ
EyeSpace eyeSpace;

bool initEyeSpace()
{
    eyeSpace.eye = { 0, 10, 30 };
    Vector up = { 0, 1, -1 };  // ͷ���ķ���, �ֲ�����ϵy��    // ��������ϵ 
    Vector lookAt = { 0, 0, 0 };  //�۾���ǰ�ķ���  
    //up = {0, 0, -1};   // ����温��
    //lookAt = {0, -10, 0};

    Vector tmpW = pointDifference(&eyeSpace.eye, &lookAt);
    eyeSpace.w = normalize(&tmpW);
    Vector tmpU = crossVector(&up, &eyeSpace.w);
    eyeSpace.u = normalize(&tmpU);
    Vector tmpV = crossVector(&eyeSpace.w, &eyeSpace.u);
    eyeSpace.v = normalize(&tmpV);

    //Vector rup = localToWorld(&up, &u, &v, &r.origin); //

    Vector  rw = { -eyeSpace.w.x, -eyeSpace.w.y, -eyeSpace.w.z };
    eyeSpace.eye2Center = scalarVector(&rw, distance); //eye �� view plane ���ĵ�����
    eyeSpace.height = ((double)height) / 1000;
    eyeSpace.width = ((double)width) / 1000;

    return true;
}

Ray getRay(int x, int y ) {  
 
    Ray r;
    r.origin = eyeSpace.eye;         // �趨�۾���λ��

    Vector b = scalarVector(&eyeSpace.v,  eyeSpace.height / 2 - ((double)y)/1000);
    Vector c = scalarVector(&eyeSpace.u, -eyeSpace.width / 2  + ((double)x)/1000);

    Vector bc = vectorAdd(&b,  &c);
    r.direction = vectorAdd(&eyeSpace.eye2Center, &bc);   //
    r.viewPlanePos = pointAdd(&r.origin, &r.direction);
#ifdef DEBUG
    r.px = x; r.py = y; // for debug
    r.type = EYE_RAY;
#endif
    return r;
}







// ���ÿһ�����ߣ�����Ҫ�ж��״����ĸ������ཻ�� �жϽ��㵽�۾��ľ��뼴��
Color traceRay(Ray* ray, Object *objs[], int num)
{
    Color color = { 0, 0, 0 };  //Ĭ�Ϻ�ɫ����

    for (int i = 0; i < num; i++)
    {
        Intersect intersection = getFirstIntersection(ray, objs, num);
        if (-1 == intersection.objectId) {
            // no intersect
           // std::cout << "sss" << std::endl;
        }
        else {
            Object* obj = objs[intersection.objectId];
            // ��ʼ��ɫ
            switch (obj->type)    
            {
            case SPHERE:
                color = shade_Sphere(ray, (Sphere*)obj->o, &intersection, &lights);
                break;
            case PLANE:
                color = shade_Plane(ray, (Plane*)obj->o, &intersection, &lights, objs, num);
                break;
            case CUBOID:
                color = shade_Cuboid(ray, (Cuboid*)obj->o, &intersection, &lights);
                break;
            case  RECTANGLE:
                color = shade_Rectange(ray, (Rectangle*)obj->o, &intersection, &lights);
                break;
            case  TRIANGLE:
                color = shade_Triangle(ray, (Triangle*)obj->o, &intersection, &lights);
                break;
            default:
                break;
            }
        }
    }

    return color;
}
 

 
#include <chrono>


    
extern   int pointInRectangle_counter ;
int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();

    initEyeSpace();
    initLights();

	Plane basePlane = { 0, 1, 0,   0,0,0,  255, 0, 0 };    // xoz ƽ��
    Sphere sphere = {0, 0.5, 0,      1 };
    
    Cuboid cuboid = { -3, 0, 0,  1, 0, -1,    1, 0, 1,  1.414213562373 };   // obj3
    Point p = cuboid.p;
    Vector tmpYDir = crossVector(&cuboid.hVector, &cuboid.wVector);
    Vector normaledY = normalize(&tmpYDir);
    Vector yVec = scalarVector(&normaledY, cuboid.yLenth);
    Vector whVec = vectorAdd(&cuboid.wVector, &cuboid.hVector);
    Vector tmpVec = vectorAdd(&whVec, &yVec);   // �Խ�����
    Point pp = pointAdd(&p, &tmpVec);
    Vector wVec = cuboid.wVector;
    Vector hVec = cuboid.hVector;
    // ȡ�Խǵ㣬 ������Զ�Ӧ������0,3,5,     1,2,4
    Vector rwVec = { -wVec.x, -wVec.y, -wVec.z };
    Vector rhVec = { -hVec.x, -hVec.y, -hVec.z };
    Vector ryVec = { -yVec.x, -yVec.y, -yVec.z };

    // ��ʱ�뷽�򣬼����ж�
    cuboid.rectangles[0] = { p,  wVec,  hVec };  // 
    cuboid.rectangles[1] = { p,  yVec,  wVec, };
    cuboid.rectangles[2] = { pp, rhVec, rwVec, };
    cuboid.rectangles[3] = { pp, rwVec, ryVec, };
    cuboid.rectangles[4] = { pp, ryVec, rhVec, };
    cuboid.rectangles[5] = { p,  hVec,  yVec, };

    Rectangle rect = { -3, 0.5, 1,  2, 0, 0,      0, 2, -2, };
        rect = { 1, 1, 1, 2, 0, 0, 0, 1, -1, };  // for debug Rectangle
    Triangle triangle = { 3, 1.5, 1,    2, 0, 0,  0, 1, -2, };
       

    Object objPlane = { PLANE, (void*)&basePlane };
    Object objSphere = { SPHERE, (void*)& sphere };
    Object objCuboid = { CUBOID, (void*)& cuboid };
    Object objRect = { RECTANGLE, (void*)& rect };
    Object objTriangle = { TRIANGLE, (void*)& triangle };

    Object *objs[] = { &objPlane,  &objSphere,   &objCuboid,  &objRect, &objTriangle,};  //
    //
    

	char* rgb = (char*)malloc(3 * width * height * sizeof(char));
 
    //Ray r;
    // x, y һ��Ҫ�ŵ����������壬����ʹ���      //Color c;    private(c)  private(r) 
#define USE_OMPm
#ifdef USE_OMP
#pragma omp parallel for schedule(dynamic, 8) 
#endif
	for (int x = 0; x<width; x++) {     // ��ɨ��
		for (int y = 0; y<height; y++) {
            Ray r = getRay(x, y);

            int objSize = sizeof(objs) / sizeof(Object*);
            Color c = traceRay(&r, objs, objSize);

            int ipos = 3 * (width * y + x);

			rgb[ipos + 2] = (unsigned char)c.z;
			rgb[ipos + 1] = (unsigned char)c.y;
			rgb[ipos] = (unsigned char)c.x;
		}
	}
	write_bmp("test.bmp", width, height, rgb);
	free(rgb);
     
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    std::cout << pointInRectangle_counter << std::endl;

    system("pause");
	return 0;
}