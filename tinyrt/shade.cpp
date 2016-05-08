#include "shade.h"
#include <stdlib.h>


bool addLight(Lights* lights, Light light)
{
    bool ret = false;
    if (lights->size <= 64){
        lights->data[lights->size] = light;
        lights->size++;

        ret = true;
    }
    else {

    }

    return ret;
}


bool  add_PointLight(Lights* lights, PointLight light)
{
    bool ret = false;

    Light pointLight;
    pointLight.light = (void*)malloc(sizeof(PointLight));
    pointLight.type = POINT_LIGHT;
    *(PointLight *)pointLight.light = { light };

    lights->data[lights->size] = pointLight;
    lights->size++;

    ret = addLight(lights, pointLight);
    if (false == ret) {
        free(pointLight.light);
    }
 

    return ret;
}

bool  remove_PointLight(Lights* lights, PointLight light)
{

    return false;
}


bool  add_DirectiontLight(Lights* lights, DirectionLight light)
{
    bool ret = false;
 
    Light dirLight;
    dirLight.light = (void*)malloc(sizeof(DirectionLight));
    dirLight.type = DIRECTION_LIGHT;
    *(DirectionLight *)dirLight.light = { light };

    ret = addLight(lights, dirLight);
    if (false == ret)
    {
        free(dirLight.light);
    }
 
    return ret;
}




Color shade_Sphere(Ray* ray, Sphere* sphere, Intersect* intersection, Lights* lights)
{
    Color color = { 255, 0, 0 };

    Vector* light = (Vector*)lights->data[0].light;

    Vector rayToLigthVector = pointDifference(light, &intersection->point);
    Ray rayToLight;
    rayToLight.origin = intersection->point;
    rayToLight.direction = rayToLigthVector;

    double intersectRayLen = vectorLength(&rayToLigthVector);
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


#include "intersect.h"


Color shade_Plane(Ray* ray, Plane* plane, Intersect* intersect, Lights* lights, Object *objs[], int num)
{
    Color color = { 255, 0, 0 };    // ���ռ����������ɫ
    Vector* light = (Vector*)lights->data[0].light;

    for (int i = 0; i < lights->size; i++) {
        Vector directionToLight = pointDifference(light, &intersect->point);
        double length = vectorLength(&directionToLight);
        bool canReachLight = true;
        // TODO ����Ҫ�жϴ� intersect �Ƿ��ܹ�����light, �����ܣ���shade
        Ray reflRay;
        reflRay.origin = intersect->point;
        reflRay.direction = directionToLight;    // ȫ����
        reflRay.type = REFL_RAY;
 
#ifdef DEBUG        
        double ll = 0.01;
        if (abs(intersect->point.x) < ll &&  abs(intersect->point.y) < ll   && abs(intersect->point.z) < ll) {
            int iiiii = 1;
            //canReachLight = false;
        }
#endif

        Intersect reflIntersect = getFirstIntersection(&reflRay, objs, num);
        if (-1 != reflIntersect.objectId)
        {
            Vector t = pointDifference(&reflIntersect.point, &intersect->point);
            double l = vectorLength(&t);
            double d = vectorLength(&directionToLight);
            if (l < epsilon)
                int i = 9;  // �Լ�
            else if (l < d) {
                canReachLight = false; // FIXME
            }
        }


        
        if (canReachLight){
            double scale = 1 / (length * length * 0.005 + length * 0.01 + 1);  // + 0.03 * length
            color.x *= scale;				// ģ���Դ��˥��
            color.y *= scale;
            color.z *= scale;
        }
        else {
            color.x *= 0;	 //  û��light �յ��������Ǻڵ�
            color.y *= 0;
            color.z *= 0;
        }
    }

    return color;
}






// ��Ҫ���ǿɼ��桢eye �Ƿ��ڹ�Դͬһ��, ���ж���Ҫ��shade�����������
// ��ԭ���򵥵�
/*
                |
    L.          |           .L`
                |
                |
                |
                |
 E.             |E`
                |
                |
                |
                |
                .P
                |
*/
Color shade_Cuboid(Ray* ray, Cuboid* cuboid, Intersect* intersect, Lights* lights)
{
    Color color = { 0, 0, 255 };
//#ifndef DEBUG
    switch (intersect->rectIdx)
    {
    case 0:
        color = { 255, 0, 0 };
        break;
    case 1:
        color = { 0, 255, 0 };
        break;
    case 2:
        color = { 0, 0, 255 };  // 
        break;
    case 3:
        color = { 128, 128, 0 };  // 
        break;
    case 4:
        color = { 128, 0, 128 };
        break;
    case 5:
        color = { 128, 128, 128 };  //
        break;
    default:
        break;
    }
//#endif

    Vector* light = (Vector*)lights->data[0].light;
    Vector directionToLight = pointDifference(light, &intersect->point);

    double length = vectorLength(&directionToLight);

    double scale = 1 / (length * length * 0.005 + length * 0.01 + 1);  // + 0.03 * length
    //printf("%f\n", scale);
    color.x *= scale;				// ģ���Դ��˥��
    color.y *= scale;
    color.z *= scale;

    return color;
}


Color shade_Rectange(Ray* ray, Rectangle* cuboid, Intersect* intersect, Lights* lights)
{
    Color color = { 0, 0, 200 };
    Vector* light = (Vector*)lights->data[0].light;
    Vector directionToLight = pointDifference(light, &intersect->point);

    double length = vectorLength(&directionToLight);

    double scale = 1 / (length * length * 0.005 + length * 0.01 + 1);  // + 0.03 * length
    //printf("%f\n", scale);
    color.x *= scale;				// ģ���Դ��˥��
    color.y *= scale;
    color.z *= scale;


    return color;
}


Color shade_Triangle(Ray* ray, Triangle* triangle, Intersect* intersect, Lights* lights)
{
    Color color = { 0, 0, 200 };
    Vector* light = (Vector*)lights->data[0].light;
    Vector directionToLight = pointDifference(light, &intersect->point);

    double length = vectorLength(&directionToLight);

    double scale = 1 / (length * length * 0.005 + length * 0.01 + 1);  // + 0.03 * length
    //printf("%f\n", scale);
    color.x *= scale;				// ģ���Դ��˥��
    color.y *= scale;
    color.z *= scale;


    return color;
}
 