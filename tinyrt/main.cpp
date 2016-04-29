#include "definition.h"

#include "bmp.h"
#include "rtmath.h"

#include <math.h>
#include <float.h>

#include <iostream>

Point light = { 0, 5, 0 };		// ���Դ��λ��
Point eye = {0, 10, 20};                          // ��������ϵ



double distance =  1.5; //  �۾����Ӵ��ľ���
const static int width = 640  ;  // 640 pixel    0.64m
const static int height = 480  ;

// view plane  ��ָ����ʽ�� eye space�� ��z ��ֱͨ��view plane
// view plane �Ŀ����eye space y �ᣨup��ƽ��
// x,y ����������, һ������0.1cm����view plane ��64cm�� ������25����Ļ
Ray getRay(int x, int y ) {  
    Vector up = { 0, 1, 0 };  // ͷ���ķ���, �ֲ�����ϵy��    // ��������ϵ 
    Vector lookAt = { 0, 0, 0 };  //�۾���ǰ�ķ���  

    //up = {0, 0, -1};   // ����温��
    //lookAt = {0, -10, 0};

    Ray r;
    r.origin = eye;         // �趨�۾���λ��


    Vector w = normalize( &pointDifference(eye, lookAt) ) ;  
    Vector u = normalize(&crossVector(&up, &w));
    Vector v = normalize(&crossVector(&w, &u));
    
    Vector rup = localToWorld(&up, &u, &v, &eye);

    Vector rw = {-w.x, -w.y, -w.z};

    Vector a = scalarVector(&rw, distance);
    
    double viewPlaneHeight = ((double)height) / 1000;
    Vector b = scalarVector(&v, viewPlaneHeight / 2 - ((double)y)/1000);
    
    double viewPlaneWidth = ((double)width) / 1000;
    Vector c = scalarVector(&u, - viewPlaneWidth / 2 + ((double)x) / 1000);

    r.direction = vectorAdd(a, vectorAdd(b,  c));   //

    //std::cout << vectorLength(r.direction) << "\t" << r.direction.x << "\t" << r.direction.y << "\t" << r.direction.z << std::endl;
    r.viewPlanePos = pointAdd(eye, r.direction);

    r.px = x; r.py = y; // for debug

    return r;
}

// ����ཻ���� objectId Ϊ ������ ����Ϊ-1
bool intersectSphere(Ray* ray, Sphere* sphere, Intersect* intersect)
{
    bool intersected = false;

    Vector rayDir = normalize(&ray->direction);
    Vector ce = pointDifference(eye, sphere->center);
    Vector l = normalize(&rayDir);

    Vector temp = pointDifference(ray->origin, sphere->center);
    double a = scalarProduct(&rayDir, &rayDir);
    double b = 2 * scalarProduct(&temp, &rayDir);
    double c = scalarProduct(&temp, &temp) - sphere->radius * sphere->radius;
    double disc = b * b - 4 * a*c;

    if (disc > 0) {
        //���ཻ�㣬��һ������㣬����ʱ����Ľ��㱻����
        double e = sqrt(disc);
        double t = -(b + e) / (a * 2.0);
        if (t >= 0) {
            //double tmin = t;
             
            Vector normal = scalarVector(&vectorAdd(temp, scalarVector(&rayDir, t)), sphere->radius);
            intersect->point = pointAdd(ray->origin, scalarVector(&rayDir, t));
        }
        intersected = true;
    }
    else {
        intersected = false;
    }


    return intersected;
}
// ����ֱ����ƽ��Ľ���
/*
���۾�λ��E����������v��ƽ�澭����P���䷨��n��Eͬ�࣬ EͶӰ��ƽ��ĵ�ΪE'��������ƽ�潻��Z�� EP �� EE' �н�theta(fixed)�� E ��ƽ�����h, EZ �� -n �н� beta,
| Z - E | = l =  h /  cos(beta) = EP * cos(theta) / (dot(v,n) / length(v) / length(n) )
*/
bool intersectPlane(Ray* ray, Plane* plane, Intersect* intersect)
{
    //Intersect intersection = { 0, 0, 0, -1 };
    bool intersected = false;

    Vector reverseN = reverseVector(plane->normal);
    double cos_beta = scalarProduct(&ray->direction, &reverseN) / vectorLength(ray->direction) / vectorLength(reverseN);

    if (cos_beta <= 0) {
        // no intersect  չʾ���
        intersected = false;
    }
    else {
        Vector ep = pointDifference(plane->p, ray->origin);
        double tttt = scalarProduct(&ep, &reverseN);
        double cos_theta = scalarProduct(&ep, &reverseN) / vectorLength(ep) / vectorLength(reverseN);
        double h = vectorLength(ep) * cos_theta;

        Vector normaledRay = normalize(&ray->direction);
        Vector ez = scalarVector(&normaledRay, (h / cos_beta));

        Point z = pointAdd(ray->origin, ez);

        intersect->point = z;

        intersected = true;
    }


    return intersected;
}

Color shade_Plane(Ray* ray, Plane* plane, Intersect* intersect)
{
    Color color = { 255, 0, 0 };

    Vector directionToLight = pointDifference(light, intersect->point);

    double length = vectorLength(directionToLight);

    double scale = 1 / (length * length * 0.005 + length * 0.01 + 1);  // + 0.03 * length

    color = { 255, 0, 0 };      //intersect.plane->color;
    color.x *= scale;				// ģ���Դ��˥��
    color.y *= scale;
    color.z *= scale;


    return color;
}


//Color shade_Cuboid(Ray* ray, Cuboid* cuboid, Intersect* intersect)
//{
//
//}


Intersect getFirstIntersection(Ray* ray, Object *objs[], int num)
{
    Intersect intersection = {0,0,0, -1};
    List *list = NULL, *curr = NULL, *nearestIntersect = NULL;

    // �ռ��ཻ������
    int index = -1;
    for (int i = 0; i < num; i++)
    {
        Object* obj = objs[i];
        bool intersected = false;
        switch (obj->type)
        {
        case SPHERE:
            intersected = intersectSphere(ray, (Sphere*)obj->o, &intersection);
            break;
        case PLANE:
            intersected = intersectPlane(ray, (Plane*)obj->o, &intersection);
            break;
        default:
            break;
        }

        if (intersected){
            curr = (List*)malloc(sizeof(List));
            intersection.objectId = i;
            curr->intersect = intersection;
            curr->next = NULL;

            if (i == 1)
                int j = i;

            if (NULL == list)
                list = curr;
            else
                list->next = curr;
        }
        else {

        }
    }
    // ��Ҫ�������������ж��ĸ����
    double minLength = DBL_MAX;
    curr = list;


    if (ray->px == width / 2 && ray->py == height / 2)
        int pppp = width;

    while (curr) {
        Vector ez = pointDifference(curr->intersect.point, eye);
        double vectorLen = vectorLength(ez);
        if (minLength > vectorLen) {
            minLength = vectorLen;
            nearestIntersect = curr;
            
            intersection = curr->intersect;
            //std::cout << intersection.point.z << std::endl;
        }
        curr = curr->next;
    }
    
    return intersection;
}

Color shade_Sphere(Ray* ray, Sphere* sphere, Intersect* intersection)
{
    Color color = {255, 0, 0};

    Vector rayToLigthVector = pointDifference(light, intersection->point);
    Ray rayToLight;
    rayToLight.origin = intersection->point;
    rayToLight.direction = rayToLigthVector;

    double intersectRayLen = vectorLength(rayToLigthVector);
    double length = intersectRayLen;
    double scale = 1 / (length * length * 0.025 + length * 0.001 + 1);  // + 0.03 * length

    bool isRayToLightSphereIntersecting = doesRaySphereIntersect(&rayToLight, sphere);
    if (isRayToLightSphereIntersecting) {
        color = { 0 };        // no light, dark
    }
    else {
        color.x *= scale;				// ģ���Դ��˥��
        color.y *= scale;
        color.z *= scale;
    }

    return color;
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
                color = shade_Sphere(ray, (Sphere*)obj->o, &intersection);
                break;
            case PLANE:
                color = shade_Plane(ray, (Plane*)obj->o, &intersection);
                break;
            case CUBOID:
                color = shade_Plane(ray, (Plane*)obj->o, &intersection);
            default:
                break;
            }
        }
    }

    return color;
}

// ground up P59
Color traceRay_Sphere(Ray* ray, Sphere* sphere)
{
    Color color = { 255, 0, 0 };
    // ������������ľ���  ���۾�����e��Բ��c�� ������ ec �ļн�Ϊ theta
    Vector ec = pointDifference(sphere->center, eye);
    Vector rayDir = normalize(&ray->direction);
    double cos_theta = scalarProduct(&ec, &rayDir) / vectorLength(ec) / vectorLength(rayDir);
    double theta = acos(cos_theta);
    double d = vectorLength(ec) * sin(theta);   // ���ĵ�ֱ�߾���

    Vector ce = pointDifference(eye, sphere->center);
    Vector l = normalize(&rayDir);

    Vector temp = pointDifference(ray->origin, sphere->center);
    double a = scalarProduct(&rayDir, &rayDir);
    double b = 2 * scalarProduct(&temp, &rayDir);
    double c = scalarProduct(&temp, &temp) - sphere->radius * sphere->radius;
    double disc = b * b - 4 * a*c;
 
    
    //if (d > sphere->radius) {
    if (disc > 0) {
        //���ཻ�㣬��һ������㣬����ʱ����Ľ��㱻����
        double e = sqrt(disc);
        double t = -(b + e) /( a * 2.0);
       // std::cout << disc << std::endl;
        if (t >= 0) {
            double tmin = t;
            Intersect intersection;
            Vector normal = scalarVector(&vectorAdd(temp, scalarVector(&rayDir, t)), sphere->radius);
            intersection.point = pointAdd(ray->origin, scalarVector(&rayDir, t));
            
            Vector rayToLigthVector = pointDifference(light, intersection.point);
            Ray rayToLight;
            rayToLight.origin = intersection.point;
            rayToLight.direction = rayToLigthVector;

            double intersectRayLen = vectorLength(rayToLigthVector);
            double length = intersectRayLen;
            double scale = 1 / (length * length * 0.025 + length * 0.001 + 1);  // + 0.03 * length

            bool isRayToLightSphereIntersecting = doesRaySphereIntersect(&rayToLight, sphere);
            if (isRayToLightSphereIntersecting) {
                color = {0};        // no light, dark
            }
            else {
                color.x *= scale;				// ģ���Դ��˥��
                color.y *= scale;
                color.z *= scale;
            }
             
        }
        else {
            //std::cout << t << std::endl;
        }
    }
    else {
         color = {255, 255, 255};   // sky 
         //std::cout << "no sphere  intersection" << std::endl;
    }


    return color;
}


// ����ֱ����ƽ��Ľ���
/*
���۾�λ��E����������v��ƽ�澭����P���䷨��n��Eͬ�࣬ EͶӰ��ƽ��ĵ�ΪE'��������ƽ�潻��Z�� EP �� EE' �н�theta(fixed)�� E ��ƽ�����h, EZ �� -n �н� beta,
| Z - E | = l =  h /  cos(beta) = EP * cos(theta) / (dot(v,n) / length(v) / length(n) )
*/
Color traceRay_Plane(Ray* ray, Plane* plane) {
	Color color = { 0, 0, 0 };

    Vector reverseN = reverseVector(plane->normal);
    double cos_beta = scalarProduct(&ray->direction, &reverseN) / vectorLength(ray->direction) / vectorLength(reverseN);

    if (cos_beta < 0) {
        // no intersect  չʾ���
        color = { 255, 255, 255 };
    }
    else {
        Vector ep = pointDifference(plane->p, ray->origin);
        double cos_theta = scalarProduct(&ep, &reverseN) / vectorLength(ep) / vectorLength(reverseN);
        double h = vectorLength(ep) * cos_theta;
        
        Vector normaledRay = normalize(&ray->direction);
        Vector ez = scalarVector(&normaledRay, (h / cos_beta));
       
        Point z = pointAdd(ray->origin, ez);

        Intersect intersect  ;
        intersect.point = z;

        Vector directionToLight = pointDifference(light, intersect.point);

        double length = vectorLength(directionToLight);

        double scale = 1 / (length * length * 0.005 + length * 0.01 + 1 );  // + 0.03 * length

        color = { 255, 0, 0 };      //intersect.plane->color;
        color.x *= scale;				// ģ���Դ��˥��
        color.y *= scale;
        color.z *= scale;
    }

	return color;
}


int main(int argc, char* argv[]) {
	Plane basePlane = { 0, 1, 0,   0,0,0,  255, 0, 0 };    // xoz ƽ��
    Sphere sphere = {0, 0.5, 0,      0.5 };

    Object obj1;
    obj1.type = PLANE;
    obj1.o = (void*)&basePlane;
    Object obj2;
    obj2.type = SPHERE;
    obj2.o = (void*)& sphere;

    Object* objs[] = { &obj1, &obj2 };  // 

	char* rgb = (char*)malloc(3 * width * height * sizeof(char));
	int x, y;
	for (x = 0; x<width; x++) {     // ��ɨ��
		for (y = 0; y<height; y++) {
			Ray r = getRay(x, y);
			
            //Color c = traceRay_Plane(&r, &basePlane);
            //Color c = traceRay_Sphere(&r, &sphere);

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
     
     //system("pause");
	return 0;
}