#ifndef TINYRT_BRDF_H
#define TINYRT_BRDF_H

#include "definition.h"

Color matte_ambient_brdf(Object *obj, double ka, double rho);

Color matte_diffuse_brdf();



Color plastic_ambient_brdf();

Color plastic_diffuse_brdf();

Color plastic_specular_brdf();

 
/*
ɢ�䡢ambient occlusion �������ֵ�ȷ��һЩ�Ѷȣ�������Щ���ϣ������޷�ʵ�ֳ�����

*/


#endif
