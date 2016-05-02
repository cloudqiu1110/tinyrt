#define _USE_MATH_DEFINES

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


bool intersectRect(Ray* ray, Rectangle* rect, Intersect* intersect)
{
    bool intersected = false;
    // ƽ�з�
    Vector rectNormalDir = crossVector(&rect->v1, &rect->v2); //������Ψһ�ģ�����ʱ�뷽��
    double cos_beta = scalarProduct(&ray->direction, &rectNormalDir) / vectorLength(ray->direction) / vectorLength(rectNormalDir);
    if (abs(cos_beta - M_PI_4) < epsilon)  {  // �뷨�ഹֱ�� ʲô��������
         // false;
    }
    else {
        if (cos_beta > M_PI_4) {
            //
            Vector ep = pointDifference(rect->p, ray->origin);  // eye �� ƽ����֪������
            double cos_theta = scalarProduct(&ray->direction, &ep) / vectorLength(ray->direction) / vectorLength(ep);
            double h = vectorLength(ep) * cos_theta;
           
            double l = h / cos_beta;   // z Ϊֱ�ߴ���ƽ��㣬 l Ϊ ez ����
            Vector rayNormal = normalize(&ray->direction);
            Vector ez = scalarVector(&rayNormal, l);
            Point z = pointAdd(ray->origin, ez);

            intersect->point = z;
            intersected = true;
        }
        else {
            // ���򲻶ԣ��ڲ��ཻ��������
        }
    }


    return intersected;
}


// �����ڰ�Χ���ཻ����
bool intersectCuboid(Ray* ray, Cuboid* cuboid, Intersect* intersect)
{
    bool intersected = false;

    // ����������
    Rectangle *rectangles = cuboid->rects;
    // ȡ�Խǵ㣬 ������Զ�Ӧ������0,3,5,     1,2,4
    Point p = cuboid->p;
    Vector yVec = scalarVector(&normalize(&crossVector(&cuboid->wVector, &cuboid->hVector)), cuboid->yLenth);
    Vector tmpVec = vectorAdd(vectorAdd(cuboid->wVector, cuboid->hVector), yVec);   // �Խ�����
    Point pp = pointAdd(p, tmpVec);

    Vector wVec = cuboid->wVector;
    Vector hVec = cuboid->hVector;

    Vector rwVec = { -wVec.x, -wVec.y, -wVec.z};
    Vector rhVec = { -hVec.x, -hVec.y, -hVec.z};
    Vector ryVec = { -yVec.x, -yVec.y, -yVec.z};

    // ��ʱ�뷽�򣬼����ж�
    rectangles[0] = { p,  wVec,  hVec };  // 
    rectangles[1] = { pp, ryVec, rhVec, };
    rectangles[2] = { pp, rhVec, rwVec, };
    rectangles[3] = { p,  hVec,  yVec, };
    rectangles[4] = { pp, rwVec, ryVec, };
    rectangles[5] = { p,  yVec,  wVec, };

    Point center;

    // ����������
    int rectIdx = -1;
    for (int i = 0; i < 6; i++)
    {
        Rectangle* rect = &rectangles[i];
        // �����ж�����ཻ���������ڲ�
        intersected = intersectRect(ray, rect, intersect);
        if (intersected) {
            rectIdx = i;
        }
        else {

        }
    }

    return intersected;
}