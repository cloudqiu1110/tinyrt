#ifndef TINYRT_INTERSECT_H
#define TINYRT_INTERSECT_H
#include "definition.h"
#include "rtmath.h"
#include "math.h"




bool intersectSphere(Ray* ray, Sphere* sphere, Intersect* intersect);

bool intersectPlane(Ray* ray, Plane* plane, Intersect* intersect);


bool intersectCuboid(Ray* ray, Cuboid* cuboid, Intersect* intersect);



#endif