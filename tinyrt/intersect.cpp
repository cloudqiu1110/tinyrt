#define _USE_MATH_DEFINES

#include "intersect.h"


int pointInRectangle_counter = 0;

// ����ཻ���� objectId Ϊ ������ ����Ϊ-1
bool intersectSphere(Ray* ray, Sphere* sphere, Intersect* intersect)
{
    bool intersected = false;

    Vector rayDir = normalize(&ray->direction);
    Vector ce = pointDifference(&ray->origin, &sphere->center);
    
    double a = scalarProduct(&rayDir, &rayDir);
    double b = 2 * scalarProduct(&ce, &rayDir);
    double c = scalarProduct(&ce, &ce) - sphere->radius * sphere->radius;
    double disc = b * b - 4 * a*c;

    if (disc > 0) {
        //���ཻ�㣬��һ������㣬����ʱ����Ľ��㱻����
        double e = sqrt(disc);
        double t = -(b + e) / (a * 2.0);
        if (t >= 0) {
            intersect->point = pointAdd(&ray->origin, &scalarVector(&rayDir, t));
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
// 150w ���ཻ �� 1536000
bool intersectPlane(Ray* ray, Plane* plane, Intersect* intersect)
{
    bool intersected = false;

    //Vector reverseN = reverseVector(&plane->normal);
    Vector reverseN = scalarVector(&plane->normal, -1);
    double cos_beta = scalarProduct(&ray->direction, &reverseN) / vectorLength(&ray->direction) / vectorLength(&reverseN);

    if (cos_beta <= 0) {
        // no intersect  չʾ���
        intersected = false;
    }
    else {
        Vector ep = pointDifference(&plane->p, &ray->origin);
        double cos_theta = scalarProduct(&ep, &reverseN) / vectorLength(&ep) / vectorLength(&reverseN);
        double h = vectorLength(&ep) * cos_theta;

        Vector normaledRay = normalize(&ray->direction);
        Vector ez = scalarVector(&normaledRay, (h / cos_beta));

        Point z = pointAdd(&ray->origin, &ez);

        intersect->point = z;
       
        intersected = true;
    }


    return intersected;
}


// between two vectors
inline bool betweenVectors(Vector* v1, Vector* v2, Vector* v)
{
    bool in = false;

    double t1 = scalarProduct(&crossVector(v1, v), &crossVector(v1, v2));
    double t2 = scalarProduct(&crossVector(v2, v), &crossVector(v2, v1));

    if (t1 >= 0 && t2 >= 0) {
        in = true;
    }

    return in;
}

// 1000w ���ཻ
bool pointInRectangle(Point* z, Rectangle* rect)
{
    bool in = false;

    Vector pz = pointDifference(z, &rect->p);

    bool between = betweenVectors(&rect->v1, &rect->v2, &pz);
    // ��v1Ϊx��v2Ϊy����������ϵ�� �ж� pz �Ƿ��ڵ�һ����
    if ( between) {
        double lpz = vectorLength(&pz);
        Vector v3 = vectorAdd(&rect->v1, &rect->v2);  // �Խ�����
        double cos_beta = scalarProduct(&v3, &rect->v1) / vectorLength(&v3) / vectorLength(&rect->v1);   // �°벿�н�
        double cos_theta = scalarProduct(&pz, &rect->v1) / vectorLength(&pz) / vectorLength(&rect->v1);
        double lpzp = 0;
        if (cos_theta >= cos_beta) {
            double lv1 = vectorLength(&rect->v1);  // length of v1
            lpzp = lv1 / cos_theta; // length of pz`   z` Ϊ pzֱ���� v2` �Ľ���
        } 
        else  {  //�����ǲ��ֵ��ж�  
            double lv2 = vectorLength(&rect->v2);  // length of v2
            double theta = acos(cos_theta);   // �� v1 �н�
            lpzp = lv2 / sin(theta);
        }
        if (lpz <= lpzp) {
            in = true;
        }
    }

    return in;
}




// 640 * 480 * 5 * 6 �ⳤ������� һǧ��ε��� �ķ���4��ʱ�䡣    �ཻ���Թ�Ȼ�����˺ܶ�ʱ��
// pointInRectangle  612w �Σ�����е������ ��ʱ2.5s
bool intersectRect(Ray* ray, Rectangle* rect, Intersect* intersect)
{
   
    bool intersected = false;
    // ƽ�з�
    Vector rectNormalDir = crossVector(&rect->v1, &rect->v2); //������Ψһ�ģ�����ʱ�뷽��
    double  cos_theta = scalarProduct(&ray->direction, &rectNormalDir)
            / vectorLength(&ray->direction) / vectorLength(&rectNormalDir);  // �۽�

    if (cos_theta < 0 )  {  // theta < M_PI_2
        Vector pe = pointDifference(&ray->origin,  &rect->p);  // eye �� ƽ����֪��p ����
        // ͶӰ�� �� ��֪�̶����� �ļнǣ���� ee` ����ֱͶӰ��h��
        double cos_beta = scalarProduct(&pe, &rectNormalDir) / vectorLength(&pe) / vectorLength(&rectNormalDir);

        if (cos_beta > 0) { //beta < M_PI_2
            double h = vectorLength(&pe) * cos_beta;
            double theta = acos(cos_theta);
            double l = h / sin(theta - M_PI_2 );   // z Ϊֱ�ߴ���ƽ��㣬 l Ϊ ez ����
            Vector rayNormal = normalize(&ray->direction);
            Vector ez = scalarVector(&rayNormal, l);
            Point z = pointAdd(&ray->origin, &ez);
            bool in = pointInRectangle(&z, rect);
            if (in) {
                intersect->point = z;
                intersected = true;
            }
            else {

            }
        }
        //  else  ���򲻶ԣ��ڲ��ཻ��������
        // TODO ��Ҫ����ƽ�е�״�� 
    }
    //  else �뷨�ഹֱ�� ʲô��������  abs(theta - M_PI) < epsilon
    // > M_PI_2 �ڲ��ཻ

    return intersected;
}



// �ж��Ƿ���v1��v2 �н� cos_beta ��
// �μ� http://stackoverflow.com/questions/13640931/how-to-determine-if-a-vector-is-between-two-other-vectors  
bool pointInTriangle(Point* z, Triangle* triangle)
{
    bool in = false;

    Vector* v1 = &triangle->v1;
    Vector* v2 = &triangle->v2;

    Vector pz = pointDifference(z, &triangle->p);  

    double t1 = scalarProduct(&crossVector(v1, &pz), &crossVector(v1, v2));
    double t2 = scalarProduct(&crossVector(v2, &pz), &crossVector(v2, v1));

    // between two vectors
    if (t1 >= 0 && t2 >= 0) {
        Vector pz = pointDifference(z, &triangle->p);
        double lpz = vectorLength(&pz);

        double cos_theta = scalarProduct(&pz, &triangle->v1) / vectorLength(&pz) / vectorLength(&triangle->v1);
        double theta = acos(cos_theta);

        Vector rv1 = { -triangle->v1.x, -triangle->v1.y, -triangle->v1.z };
        Vector v3 = vectorAdd(&rv1, &triangle->v2);  // ��v1 �յ�Ϊ���� ��������
        double cos_gamma = scalarProduct(&v3, &rv1) / vectorLength(&v3) / vectorLength(&rv1);
        double gamma = acos(cos_gamma);

        double lv1 = vectorLength(&triangle->v1);  // length of v1

        // length of pz`   z` Ϊ pzֱ���� v3 �Ľ���
        double h = lv1 * tan(theta) * tan(gamma) / (tan(theta) + tan(gamma));

        double lpzp = h / sin(theta);
        if (lpz <= lpzp) {
            in = true;
        }
 
    }

    return in;
}

bool intersectTriangle(Ray* ray, Triangle* triangle, Intersect* intersect)
{
    bool intersected = false;
    // ƽ�з�
    Vector rectNormalDir = crossVector(&triangle->v1, &triangle->v2); //������Ψһ�ģ�����ʱ�뷽��

    double  cos_theta = scalarProduct(&ray->direction, &rectNormalDir)
        / vectorLength(&ray->direction) / vectorLength(&rectNormalDir);

    if (cos_theta < 0)  {  // theta < M_PI_2
        Vector ep = pointDifference(&ray->origin, &triangle->p);  // eye �� ƽ����֪��p ����
        // ͶӰ�� �� ��֪�̶����� �ļнǣ���� ee` ����ֱͶӰ��h��
        double cos_beta = scalarProduct(&ep, &rectNormalDir) / vectorLength(&ep) / vectorLength(&rectNormalDir);
        if (cos_beta > 0) { //beta < M_PI_2
            double h = vectorLength(&ep) * cos_beta;
            double theta = acos(cos_theta);
            double l = h / sin(theta - M_PI_2);   // z Ϊֱ�ߴ���ƽ��㣬 l Ϊ ez ����
            Vector rayNormal = normalize(&ray->direction);
            Vector ez = scalarVector(&rayNormal, l);
            Point z = pointAdd(&ray->origin, &ez );
            bool in = pointInTriangle(&z, triangle);
            if (in) {
                intersect->point = z;
                intersected = true;
            }

        }
        // else ���򲻶ԣ��ڲ��ཻ��������
    }
    // else�뷨�ഹֱ�� ʲô��������  abs(theta - M_PI) < epsilon
    // > M_PI_2 �ڲ��ཻ
 
    return intersected;
}



// �����ڰ�Χ���ཻ����
bool intersectCuboid(Ray* ray, Cuboid* cuboid, Intersect* intersect)
{
    bool intersected = false;

    // ����������
    Rectangle *rectangles = cuboid->rectangles;

    // ����������
    //int rectIdx = -1;
    for (int i = 0; i < 6; i++)
    {
        Rectangle* rect = &rectangles[i];
        // �����ж�����ཻ���������ڲ�
        intersected = intersectRect(ray, rect, intersect);
        if (intersected) {
            intersect->rectIdx = i;
            break;
        }
        else {

        }
    }

    return intersected;
}