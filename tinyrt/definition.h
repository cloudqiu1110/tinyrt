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
    double  x;
    double 	y;
    double 	z;
    double  w;
} Vector4;

typedef struct {
	Point origin;
	Vector direction;
    Vector viewPlanePos;
    int px, py;   //
} Ray;

typedef  Ray Line;  // ֱ��

 
typedef struct {
	Vector normal;
    Point p;
	Color color;
} Plane;		// ���޿���ƽ��, ��ֻ������ƽ��ƽ��

// ��ϸѡ��p�� v1, v2 �豣֤rect �������
typedef struct {
	Point p;  //
	Vector v1;
	Vector v2;
} Rectangle;

typedef struct {
	Point center;
	double radius;
} Sphere;


//typedef struct {
//	Point point;
//	//double lambda;
//	//Plane* plane;
//} Intersection;


typedef struct {
    Point point;
    int objectId;   // index from 0 in  Object[],  if -1, means no intersect
} Intersect;

// ֻ��Ҫָ���ײ��ĳ���ʣ�µ�����y�ᣬָֻ�����ȼ��ɣ�
// �ײ������1����ʱ�룬 ����Ϊ3��  ǰ��5�� ���� 6��  
typedef struct {
    Point p;   
    Vector wVector;   // widht  = x 
    Vector hVector;   // z
    double yLenth;      // y

    Rectangle rects[6];
} Cuboid;



typedef struct  {
    double m[3][3];
} Mat3;

typedef struct {
    double m[4][4];
} Mat4;

typedef struct {
    double m[4][1];
} Mat4_1;

enum TYPE {
    PLANE = 0,
    RECTANGLE ,
    TRIANGLE,

    SPHERE,
    CUBOID,
    CONE,
    FRUSTRUM
};

// need to free the source manually
typedef struct {
    enum TYPE type;
    void *o;
} Object;



typedef struct _List{
    Intersect intersect ;
    struct _List* next ;
} List;

#endif 