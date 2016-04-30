#include "intersect.h"


// ����ཻ���� objectId Ϊ ������ ����Ϊ-1
bool intersectSphere(Ray* ray, Sphere* sphere, Intersect* intersect)
{
    bool intersected = false;

    Vector rayDir = normalize(&ray->direction);
    Vector ce = pointDifference(ray->origin, sphere->center);
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
            //Vector ttttt = vectorAdd(temp, scalarVector(&rayDir, t));
            //Vector normal = scalarVector(&, sphere->radius);
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


bool intersectRect(Ray* ray, Rectangle* rect, Intersect* intersect)
{

    return false;
}


// �����ڰ�Χ���ཻ����
bool intersectCuboid(Ray* ray, Cuboid* cuboid, Intersect* intersect)
{
    // ����������
    Rectangle *rectangles = cuboid->rects;
    // ȡ�Խǵ㣬 ������Զ�Ӧ������0,3,5,     1,2,4
    Point p = cuboid->p;
    Vector tmpYDir = crossVector(&cuboid->wVector, &cuboid->hVector);
    Vector normaledY = normalize(&tmpYDir);
    Vector yVec = scalarVector(&normaledY, cuboid->yLenth);
    Vector tmpVec = vectorAdd(vectorAdd(cuboid->wVector, cuboid->hVector), yVec);   // �Խ�����
    Point pp = pointAdd(p, tmpVec);

    Vector wVec = cuboid->wVector;
    Vector hVec = cuboid->hVector;

    rectangles[0] = { p, wVec, hVec };
    rectangles[1] = { pp, hVec, yVec };
    rectangles[2] = { pp, wVec, hVec };
    rectangles[3] = { p, hVec, yVec };
    rectangles[4] = { pp, wVec, yVec };
    rectangles[5] = { p, wVec, yVec };

    Point center;

    // ����������
    int rectIdx = -1;
    for (int i = 0; i < 6; i++)
    {
        Rectangle* rect = &rectangles[i];
        // �����ж�����ཻ���������ڲ�
        bool intersected = intersectRect(ray, rect, intersect);
        if (intersected) {

        }
        else {

        }
    }




    return false;
}