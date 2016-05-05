#include "definition.h"
#include "intersect.h"
#include "shade.h"

#include "bmp.h"
#include "rtmath.h"

#include <math.h>
#include <float.h>

#include <iostream>

Point light = { 0, 5, 0 };		// ���Դ��λ��
                         // ��������ϵ



double distance =  1.5; //  �۾����Ӵ��ľ���
const static int width = 640  ;  // 640 pixel    0.64m
const static int height = 480  ;

// view plane  ��ָ����ʽ�� eye space�� ��z ��ֱͨ��view plane
// view plane �Ŀ����eye space y �ᣨup��ƽ��
// x,y ����������, һ������0.1cm����view plane ��64cm�� ������25����Ļ
Ray getRay(int x, int y ) {  

    Point eye = {0, 5, 30}; 

    Vector up = { 0, 1, 0 };  // ͷ���ķ���, �ֲ�����ϵy��    // ��������ϵ 
    Vector lookAt = { 0, 0, 0 };  //�۾���ǰ�ķ���  

    //up = {0, 0, -1};   // ����温��
    //lookAt = {0, -10, 0};

    Ray r;
    r.origin = eye;         // �趨�۾���λ��

    Vector tmpW = pointDifference(eye, lookAt);
    Vector w = normalize( &tmpW ) ;
    Vector tmpU = crossVector(&up, &w);
    Vector u = normalize(&tmpU);
    Vector tmpV = crossVector(&w, &u);
    Vector v = normalize(&tmpV);
    
    Vector rup = localToWorld(&up, &u, &v, &r.origin); //

    Vector  rw = {-w.x, -w.y, -w.z};

    Vector a = scalarVector(&rw, distance);
    
    double viewPlaneHeight = ((double)height) / 1000;
    Vector b = scalarVector(&v, viewPlaneHeight / 2 - ((double)y)/1000);
    
    double viewPlaneWidth = ((double)width) / 1000;
    Vector c = scalarVector(&u, - viewPlaneWidth / 2 + ((double)x) / 1000);

    r.direction = vectorAdd(a, vectorAdd(b,  c));   //

    //std::cout << vectorLength(r.direction) << "\t" << r.direction.x << "\t" << r.direction.y << "\t" << r.direction.z << std::endl;
    r.viewPlanePos = pointAdd(r.origin, r.direction);

    r.px = x; r.py = y; // for debug

    return r;
}



// �ж����ĸ������״��ཻ
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
        case CUBOID:
            intersected = intersectCuboid(ray, (Cuboid*)obj->o, &intersection);
            break;
        case RECTANGLE:
            intersected = intersectRect(ray, (Rectangle*)obj->o, &intersection);
            break;
        case TRIANGLE:
            intersected = intersectTriangle(ray, (Triangle*)obj->o, &intersection);
            break;
        default:
            std::cout << "obj not checked intersection, bad logic" << std::endl;
            break;
        }

        // ���
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
        Vector ez = pointDifference(curr->intersect.point, ray->origin);
        double vectorLen = vectorLength(ez);
        if (minLength > vectorLen) {
            minLength = vectorLen;
            nearestIntersect = curr;
            
            intersection = curr->intersect;
            //std::cout << intersection.point.z << std::endl;
        }
        curr = curr->next;
    }

    // TODO release List mem
    
    return intersection;
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
                color = shade_Sphere(ray, (Sphere*)obj->o, &intersection, &light);
                break;
            case PLANE:
                color = shade_Plane(ray, (Plane*)obj->o, &intersection, &light);
                break;
            case CUBOID:
                color = shade_Cuboid(ray, (Cuboid*)obj->o, &intersection, &light);
                break;
            case  RECTANGLE:
                color = shade_Rectange(ray, (Rectangle*)obj->o, &intersection, &light);
                break;
            case  TRIANGLE:
                color = shade_Triangle(ray, (Triangle*)obj->o, &intersection, &light);
                break;
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
    Vector ec = pointDifference(sphere->center, ray->origin);
    Vector rayDir = normalize(&ray->direction);
    double cos_theta = scalarProduct(&ec, &rayDir) / vectorLength(ec) / vectorLength(rayDir);
    double theta = acos(cos_theta);
    double d = vectorLength(ec) * sin(theta);   // ���ĵ�ֱ�߾���

    Vector ce = pointDifference(ray->origin, sphere->center);
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
            Vector ttttttttt = vectorAdd(temp, scalarVector(&rayDir, t));
            Vector normal = scalarVector(&ttttttttt, sphere->radius);
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
    Sphere sphere = {0, 0.5, 0,      1 };
    
    Cuboid cuboid = { 1, 0, 0,  1, 0, -1,    1, 0, 1,  1.414213562373 };   // obj3
    Point p = cuboid.p;
    Vector tmpYDir = crossVector(&cuboid.hVector, &cuboid.wVector);
    Vector normaledY = normalize(&tmpYDir);
    Vector yVec = scalarVector(&normaledY, cuboid.yLenth);
    Vector tmpVec = vectorAdd(vectorAdd(cuboid.wVector, cuboid.hVector), yVec);   // �Խ�����
    Point pp = pointAdd(p, tmpVec);
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

    Rectangle rect = { -3, 0.5, 5,  1.5, 0, 0,      0, 1, -1, };
    Triangle triangle = { 1, 0.5, 0, 1.5, 0, 0, 0, 1, -1, };

    Object obj1 = { PLANE, (void*)&basePlane };
    Object obj2 = { SPHERE, (void*)& sphere };
    Object obj3 = { CUBOID, (void*)& cuboid };
    Object obj4 = { RECTANGLE, (void*)& rect };
    Object obj5 = { TRIANGLE, (void*)& triangle };

    Object* objs[] = { &obj2, &obj5 };  //   &obj1, &obj2, &obj3, &obj4,

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