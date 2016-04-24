#include "definition.h"

#include "bmp.h"
#include "rtmath.h"

#include <math.h>
#include <float.h>

#include <iostream>

Point light = { 0, 10, 0 };		// ���Դ��λ��
Point center = { 0, 0, 0 };     // ����ϵԭ��
Point eye = {0, 20, 320};                          // ��������ϵ
Vector lookAtDir = {0, 0, -8};  //�۾���ǰ�ķ���   // ��������ϵ
Vector up = {0, 1, 0};  // ͷ���ķ���, �ֲ�����ϵ��// ��������ϵ

double distance = 480; //  �۾����Ӵ��ľ���
const static int width = 640 * 2;
const static int height = 480 * 2;



bool isLineParallel(Ray* r1, Ray* r2)
{
    return false;
}



// view plane  ��ָ����ʽ�� eye space�� ��z ��ֱͨ��view plane
// view plane �Ŀ����eye space y �ᣨup��ƽ��
Ray getRay(int w, int h) {
	Ray r;
    r.origin = eye;         // �趨�۾���λ��

    Vector viewRight2Left = reverseVector(  crossVector(&lookAtDir, &up) );
    
    Vector rup = localToWorld(&up, &lookAtDir, &up, &eye);
    
    //testMultiplyMat3();
    
    //std::cout << rup.x << "\t" << rup.y << "\t" << rup.z << std::endl;
    

    r.direction = vectorAdd(scalarVector(&lookAtDir, distance),
        vectorAdd(scalarVector(&up, height / 2 - h), scalarVector(&viewRight2Left, -width / 2 + w)));
    
	//r.direction.x =  + 0.5;	// left to right
	//r.direction.y = height / 2 - h + 0.5;	// top to down
	//r.direction.z = -960;
	
    
    //std::cout << r.direction.x << "\t" << r.direction.y << "\t" << r.direction.z << std::endl;
    
    return r;
}



Color traceRay(Ray* ray, Plane* plane, Sphere* sphere)
{
    Color color = { 0, 0, 0 };
    // ���ÿһ�����ߣ�����Ҫ�ж��״����ĸ������ཻ�� �жϽ��㵽�۾��ľ��뼴��

    return color;
}

// ����ֱ����ƽ��Ľ���
/*
���۾�λ��E����������v��ƽ�澭����P���䷨��n��Eͬ�࣬ EͶӰ��ƽ��ĵ�ΪE'��������ƽ�潻��Z�� EP �� EE' �н�theta(fixed)�� E ��ƽ�����h, EZ �� -n �н� beta,
| Z - E | = l =  h /  cos(beta) = EP * cos(theta) / (dot(v,n) / length(v) / length(n) )
*/
Color traceRay_Plane(Ray* ray, Plane* plane) {
	Color color = { 0, 0, 0 };
    
    //std::cout << ray->direction.x << "\t" << ray->direction.y << "\t" << ray->direction.z << std::endl;

    Vector reverseN = reverseVector(plane->normal);
    double cos_beta = scalarProduct(&ray->direction, &reverseN) / vectorLength(ray->direction) / vectorLength(reverseN);

    //std::cout << cos_beta << std::endl;
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

        Intersection intersect = { z, 0, plane };

        Vector directionToLight = pointDifference(light, intersect.point);
        //Ray lightRay = { intersect.point, directionToLight };

        double length = vectorLength(directionToLight);
        

        double scale = 1 / (length * length *0.0001 + 0.003 * length +  1);
        
        //std::cout << ez.x << "\t" << length << "\t" << cos_beta << std::endl;

        color = intersect.plane->color;
        color.x *= scale;				// ģ���Դ��˥��
        color.y *= scale;
        color.z *= scale;
    }

	return color;
}


int main(int argc, char* argv[]) {
	Plane basePlane = { 0, 1, 0,   0,0,0,  255, 0, 0 };    // xoz ƽ��

    Sphere sphere = {0,0,0,     1.0 };

	char* rgb = (char*)malloc(3 * width * height * sizeof(char));
	int x, y;
	for (x = 0; x<width; x++) {     // ��ɨ��
		for (y = 0; y<height; y++) {
            //if (!(y < 583 && y > 581))
            //    continue;
			Ray r = getRay(x, y);
			
			Color c = traceRay_Plane(&r, &basePlane);
			
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