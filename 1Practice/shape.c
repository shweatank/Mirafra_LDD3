#include<stdio.h>
#include<stdlib.h>

#define pie 3.14
typedef struct
{
	int radius;
}Circle;

typedef struct
{
	int length;
	int width;
}Rectangle;

typedef struct
{
	void *data;
	int (*area)(void *data);
}Shape;

int areaCircle(void *data)
{
	Circle *circle = (Circle*)data;
	return pie* circle->radius * circle->radius;
}

int areaRectangle(void *data)
{
	Rectangle *rectangle = (Rectangle*)data;
	return rectangle->length * rectangle->width;
}
int main()
{
	Circle *circle =(Circle*)malloc(sizeof(Circle));

	circle->radius = 4;

	Shape shapeCircle;
	shapeCircle.data=circle;
	shapeCircle.area = areaCircle;

	printf("Area of Circle %d\n",shapeCircle.area(shapeCircle.data));

	free(circle);


	Rectangle *rectangle = (Rectangle*)malloc(sizeof(Rectangle));
	rectangle -> length = 4;
	rectangle -> width = 2;

	Shape shapeRec;
	shapeRec.data = rectangle;
	shapeRec.area = areaRectangle;

	printf("Area of Rectangle %d\n", shapeRec.area(shapeRec.data));
	free(rectangle);

	return 0;

}
