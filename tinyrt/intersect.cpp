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
/*
#ifdef DEBUG
if (ray->type == REFL_RAY)
int jjjjjjjjjj = 1;
#endif
*/
bool intersectRect_backup(Ray* ray, Rectangle* rect, Intersect* intersect)
{
    bool intersected = false;

    Vector rectNormalDir = crossVector(&rect->v1, &rect->v2); //������Ψһ�ģ�����ʱ�뷽��
    Vector pe = pointDifference(&ray->origin, &rect->p);  // eye �� ƽ����֪��p ����
    // ����Ҫ�ж�ray normal �Ƿ������������ж������
    double t = scalarProduct(&pe, &rectNormalDir);
   
    double cos_beta = scalarProduct(&pe, &rectNormalDir) / vectorLength(&pe) / vectorLength(&rectNormalDir);
    double  cos_theta = scalarProduct(&ray->direction, &rectNormalDir)
        / vectorLength(&ray->direction) / vectorLength(&rectNormalDir);

    bool rayThroughPlane = false;
    double l = 0;
    if ( t >= 0 )  {    // ���� ray->origin ��ͬһ��   //  beta��ǣ�  0 <= theta  <= M_PI
        // ���� �� ��֪�̶����� �ļнǣ���� ee` ����ֱͶӰ��h��
        double t2 = scalarProduct(&ray->direction, &rectNormalDir);
        if (t2 < 0) {   // ray  �� ����ƽ���ཻ
            rayThroughPlane = true;

            double c_theta =  M_PI - acos(cos_theta);
            double h = vectorLength(&pe) * cos_beta;
            l = h / cos(c_theta);
        }  // else ����ԭ��ƽ�淢��ȥ��
    }
    else {  
        double t2 = scalarProduct(&ray->direction, &rectNormalDir);
        if (t2 >= 0){ 
            rayThroughPlane = true;

            double c_beta = M_PI - acos(cos_beta);
            double h = vectorLength(&pe) * cos(c_beta);
            l = h / cos_theta;
        }  // else ���߷�����
    }
    //   TODO �ж��Ƿ���   �뷨�ഹֱ�� ʲô�������� 
    if (rayThroughPlane) {
        Vector rayNormal = normalize(&ray->direction);
        Vector ez = scalarVector(&rayNormal, l);
        Point z = pointAdd(&ray->origin, &ez);

        bool in = pointInRectangle(&z, rect);
        if (in) {
            intersect->point = z;
            intersected = true;
        }
    }

    return intersected;
}


bool intersectRect(Ray* ray, Rectangle* rect, Intersect* intersect)
{
    bool intersected = false;
    Vector rectNormalDir = crossVector(&rect->v1, &rect->v2); //������Ψһ�ģ�����ʱ�뷽��
    double  cos_theta = scalarProduct(&ray->direction, &rectNormalDir)
        / vectorLength(&ray->direction) / vectorLength(&rectNormalDir);  // �۽�

    if (cos_theta < 0)  {  // theta < M_PI_2
        Vector pe = pointDifference(&ray->origin, &rect->p);  // eye �� ƽ����֪��p ����
        // ͶӰ�� �� ��֪�̶����� �ļнǣ���� ee` ����ֱͶӰ��h��
        double cos_beta = scalarProduct(&pe, &rectNormalDir) / vectorLength(&pe) / vectorLength(&rectNormalDir);

        if (cos_beta > 0) { //beta < M_PI_2
            double h = vectorLength(&pe) * cos_beta;
            double theta = acos(cos_theta);
            double l = h / sin(theta - M_PI_2);   // z Ϊֱ�ߴ���ƽ��㣬 l Ϊ ez ����
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
    }

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
        assert(h >= 0);

        double lpzp = h / sin(theta);
        if (lpz <= lpzp) {
            in = true;
        }
 
    }

    return in;
}


//if (REFL_RAY == ray->type)
//{
//    double ll = 0.1;
//    if (abs(intersect->point.x) < ll &&  abs(intersect->point.y) < ll   && abs(intersect->point.z) < ll) {
//        int iiiii = 1;
//    }
//}
bool intersectTriangle(Ray* ray, Triangle* triangle, Intersect* intersect)
{
    bool intersected = false;
    // ƽ�з�
    Vector rectNormalDir = crossVector(&triangle->v1, &triangle->v2); //������Ψһ�ģ�����ʱ�뷽��

    // �ж�Ray �� Triangle ����ƽ�棬 ��dotProduct
    bool orth = abs( scalarProduct(&ray->direction, &rectNormalDir) ) < epsilon ? true : false;
    if (!orth)  {   
        Vector pe = pointDifference(&ray->origin, &triangle->p);  // eye �� ƽ����֪��p ����
        // ͶӰ�� �� ��֪�̶����� �ļнǣ���� ee` ����ֱͶӰ��h��
        // �ж� EP �� triangle�����棬�༴������
        bool orth2 = abs(scalarProduct(&pe, &rectNormalDir)) < epsilon ? true : false;
        if ( !orth2) { //beta < M_PI_2
            double cos_beta = scalarProduct(&pe, &rectNormalDir) / vectorLength(&pe) / vectorLength(&rectNormalDir);
            double h = vectorLength(&pe) * cos_beta;
            double  cos_theta = scalarProduct(&ray->direction, &rectNormalDir)
                / vectorLength(&ray->direction) / vectorLength(&rectNormalDir);
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
// Ҫ���볤�������������Ϊ���ཻ
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


// �ж����ĸ������״��ཻ
Intersect getFirstIntersection(Ray* ray, Object *objs[], int num)
{
    Intersect intersection = { 0, 0, 0, -1 };
    List *list = NULL, *curr = NULL, *nearestIntersect = NULL;

    // �ռ��ཻ������
    //int index = -1;
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
            //std::cout << "obj not checked intersection, bad logic" << std::endl;
            break;
        }

        // ���
        if (intersected){
            curr = (List*)malloc(sizeof(List));
            intersection.objectId = i;
            curr->intersect = intersection;
            curr->next = NULL;

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

    while (curr) {
        Vector ez = pointDifference(&curr->intersect.point, &ray->origin);
        double vectorLen = vectorLength(&ez);
        if (minLength > vectorLen) {
            minLength = vectorLen;
            nearestIntersect = curr;

            intersection = curr->intersect;
        }
        curr = curr->next;
    }

    // release List mem
    curr = list;
    while (curr) {
        List* next = curr->next;
        free((void*)curr);
        curr = next;
    }

    return intersection;
}
