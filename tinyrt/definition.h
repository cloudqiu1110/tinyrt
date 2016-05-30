#ifndef RT_DEFINITION_H
#define RT_DEFINITION_H

#define _USE_MATH_DEFINES

static const double epsilon = 0.000000000000001;

typedef unsigned char byte;

typedef struct {
	double  x;
	double 	y;
	double 	z;
} Vector;
typedef Vector Point;
typedef Vector Color;

typedef struct {
    byte r;
    byte g;
    byte b;
} RGBColor;

typedef struct {
    double  x;
    double 	y;
    double 	z;
    double  w;
} Vector4;

enum RayType
{
    EYE_RAY,
    REFL_RAY,
    REFR_RAY,
    LIGHT_RAY,
    _Ray_Type_Size
};

typedef struct {
	Point origin;
	Vector direction;
    Vector viewPlanePos;
    RayType type;
#ifdef DEBUG
    int px, py;   // for debug
#endif
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
    Point p;
    Vector v1;
    Vector v2;
} Triangle;

typedef struct {
	Point center;
	double radius;
} Sphere;


typedef struct {
    Vector u;
    Vector v;
    Vector w;

    double height;  // view plane
    double width;

    Vector eye2Center;

    Point eye;
} EyeSpace;


typedef struct {
    Point point;
    int objectId;   // index from 0 in  Object[],  if -1, means no intersect
    int rectIdx;    // Ϊ��box �ĵ���
} Intersect;

// ֻ��Ҫָ���ײ��ĳ���ʣ�µ�����y�ᣬָֻ�����ȼ��ɣ�
// �ײ������1����ʱ�룬 ����Ϊ3��  ǰ��5�� ���� 6��  
typedef struct {
    Point p;   
    Vector wVector;   // widht  = x 
    Vector hVector;   // z
    double yLenth;      // y

    Rectangle rectangles[6];
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

enum MATERIAL_TYPE {
    MATTE,
    PLASTIC,
    METAL,
    SILK,
    _MATERIAL_TYPE_SIZE
};

typedef struct {
    float	kd;
    Color 	cd;
} Lambertian;

typedef struct {
    Lambertian ambient_brdf;
    Lambertian diffuse_brdf;
} Matte;

typedef struct {
    MATERIAL_TYPE type;
    void *data;
} Material;

// �򵥵�ʾ�������У����ּ򵥵Ľṹ��û�д��ڵı�Ҫ�ˣ�ֱ�����ϲ������ʵ�ּ���


// need to free the source manually
typedef struct {
    enum TYPE type;
    void *o;
    Material material;
} Object;



typedef struct _List{
    Intersect intersect ;
    struct _List* next ;
} List;


enum LIGHTTYPE
{
    POINT_LIGHT,
    DIRECTION_LIGHT,
    AMBIENT_LIGHT,
};


typedef struct {
    Vector pos;
    Vector color;
} PointLight;

typedef struct {
    Vector dir;
    Vector color;
    double ls;
} DirectionLight;

typedef struct {
    Color color;
    double ls;  // scale_radiance
} AmbientLight;

// �������ڵ�����ʵ�������ַ�ʽ��
//1�� ͨ�����ڰ����������ز�����ʵ�֣���Ҫָ����������Ĳ���
//2�� ����ʹ��������µĹ�Դ��������� Env_light
typedef struct {
    Vector u;
    Vector v;
    Vector w;
    Color min_amount;
    //Sampler sampler_p;
} AmbientOccluderLight;

typedef struct {
    void *light;
    LIGHTTYPE type;
} Light;

typedef struct {
    Light data[64];  // ���֧�� 64 ����Դ
    int size;       
} Lights;


typedef struct {
    Object objs[1024];
    int size;  // ����ܹ����õ��������� 1024

    Lights lights; // ��Դ����




} World;

typedef struct {
    double x;
    double y;
} Point2d;


typedef struct {
    int num_samples; // the number of sample points in a pattern
    int num_sets;    // the number of sample sets(patterns) stored
    Point2d *samples; // ��̬�ڴ棬��Ҫע�����
    int *shuffed_indicies;
    unsigned long count;
    int jump;

} Sampler;





#endif 