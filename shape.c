#include<stdio.h>
#include<stdlib.h>
struct shape
{
	void *data;
	float (*area)(void *)
}shape;
typedef int (*fp)(void *fp);
typedef struct circle
{
	int radius;
	float (*area)(void *);	
}circle;
typedef struct rectangle
{
	int length;
	int width;
}rectangle;
float areaCircle(void *data)
{
	float area;
	circle *cir=(circle *)data;
	area=2*3.14*cir->radius;
	return area;
}
float areaRectangle(void *data)
{
	float area;
	rectangle *rec=(rectangle *)data;
	area=2*(rec->length)*(rec->width);
	return area;
}


int main()
{
	rectangle *rect=malloc(1*sizeof(rectangle));
	circle *circ=malloc(1*sizeof(circle));
	printf("enter radius of a circle\n");
	scanf("%d",circ->radius);
	printf("enter length and width of rectangle \n");
	scanf("%d ",rect->length);
	scanf("%d",rect->width);
	shape shape1;
	shape1.data=rect;
	shape1.area=areaRectangle;
	shape shape2;
	shape2.data=circ;
	shape.area=areaCircle;

	printf("area of circle =%d \t area of rectangle=%d \n",shape2.area(shape2.data),shape1.area(shape1.data));
}

