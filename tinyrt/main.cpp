#include "definition.h"

#include "bmp.h"


#include <math.h>
#include <float.h>

#include <iostream>

Point light = { 0, 5, 0 };		// ���Դ��λ��
Point center = { 0, 0, 0 };     // ����ϵԭ��
const static int width = 640 * 2;
const static int height = 480 * 2;



Ray getRay(int x, int y) {
	Ray r;
	r.origin.x = 0;         // �趨�۾���λ��
	r.origin.y = 15;
	r.origin.z = 60;
	r.direction.x = -width / 2 + x + 0.5;	// left to right
	r.direction.y = height / 2 - y + 0.5;	// top to down
	r.direction.z = -400;
	return r;
}


double vectorLength(Vector v) {
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

Vector reverseVector(Vector v1) {
    Vector v;
    v.x = -1 * v1.x;
    v.y = -1 * v1.y;
    v.z = -1 * v1.z;

    return v;
}


Vector pointDifference(Point p1, Point p2) {
	Vector v = { p1.x - p2.x, p1.y - p2.y, p1.z - p2.z };
	return v;
}

// ĳ�㾭��������transform�������µĵ�
Point pointAdd(Point p1, Vector v) {
    Vector v1 = { p1.x + v.x, p1.y + v.y, p1.z + v.z };
    return v1;
}

double scalarProduct(Vector* v1, Vector* v2) {
	return v1->x*v2->x + v1->y*v2->y + v1->z*v2->z;
}


void  normalizeColor(Color* color) {
	double factor = 255 / fmax(color->x, fmax(color->y, color->z));
	if (factor < 1) {
		color->x = color->x*factor;
		color->y = color->y*factor;
		color->z = color->z*factor;
	}
}

Vector scalarVector(Vector* v, double scale)
{
    Vector v1;
    v1.x = v->x * scale;
    v1.y = v->y * scale;
    v1.z = v->z * scale;

    return v1;
}

Vector normalize(Vector* v)
{
    Vector v1;
    double l = vectorLength(*v);
    v1.x = v->x / l;
    v1.y = v->y / l;
    v1.z = v->z / l;
    return  v1;
}


bool isLineParallel(Ray* r1, Ray* r2)
{
    return false;
}


Color traceRay(Ray* ray, Plane* plane, Sphere* sphere)
{
    Color color = { 0, 0, 0 };
    // ���ÿһ�����ߣ�����Ҫ�ж��״����ĸ������ཻ�� �жϽ��㵽�۾��ľ��뼴��

    return color;
}

// ����ֱ����ƽ��Ľ���
/*
���۾�λ��E����������v��ƽ�澭����p���䷨��n��Eͬ�࣬ ������ƽ�潻��Z�� EP �� -n �н�theta�� E ��ƽ�����h, EZ �� -n �н� beta,
| Z - E | = l =  h /  cos(beta) = EP * cos(theta) / (dot(v,n) / length(v) / length(n) )
*/
Color traceRay_Plane(Ray* ray, Plane* plane) {
	Color color = { 0, 0, 0 };

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
        Ray lightRay = { intersect.point, directionToLight };

        double length = vectorLength(directionToLight);
        

        double scale = 1 / (length * length *0.001 + 0.005 * length +  1);

        color = intersect.plane->color;
        color.x *= scale;				// ģ���Դ��˥��
        color.y *= scale;
        color.z *= scale;
    }

	return color;
}



int main(int argc, char* argv[]) {
	Plane basePlane = { 0, 1, 0,   0,0,0,  255, 0, 0 };    

    Sphere sphere = {0,0,0, 1.0 };

	char* rgb = (char*)malloc(3 * width * height * sizeof(char));
	int x, y;
	for (x = 0; x<width; x++) {
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