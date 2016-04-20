#ifndef RT_DEFINITION_H
#define RT_DEFINITION_H


static const double epsilon = 0.000000000000001;



typedef struct {
	double  x;
	double 	y;
	double 	z;
} Vector;
typedef Vector Point;
typedef Vector Color;

typedef struct {
	Point origin;
	Vector direction;
} Ray;

typedef  Ray Line;  // ֱ��

 
typedef struct {
	Vector normal;
	//double distance;
    Point p;
	Color color;
} Plane;		// ���޿���ƽ��, ��ֻ������ƽ��ƽ��

typedef struct {
	Point p;  //
	Vector v1;
	Vector v2;
} Rectangle;

typedef struct {
	Point center;
	double radius;
} Sphere;

typedef struct {
	Point point;
	double lambda;
	Plane* plane;
} Intersection;




#endif 