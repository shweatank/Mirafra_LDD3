#include<stdio.h>
#include<stdlib.h>


struct Shape
{
	void *type;
	double (*area)(void *data);
};

struct Circle
{
	int radius;
};

struct Rectangle
{
	int length;
	int width;
};

double areaCircle(void *data)
{
	struct Circle *circle = (struct Circle *)(data);
	return 3.14*(circle->radius)*(circle->radius);
}

double areaRectangle(void *data)
{
	struct Rectangle *rectangle = (struct Rectangle *)(data);
	return (rectangle->length)*(rectangle->width);
}


int main()
{
	struct Circle *circle = (struct Circle *)malloc(sizeof(struct Circle));
	circle->radius = 4;

	struct Rectangle *rectangle = (struct Rectangle *)malloc(sizeof(struct Rectangle));
	rectangle->length = 6;
	rectangle->width  = 10;
	
	struct Shape shapeCircle;
	shapeCircle.type = circle;
	shapeCircle.area = &areaCircle;

	struct Shape shapeRectangle;
	shapeRectangle.type = rectangle;
	shapeRectangle.area = &areaRectangle;
	printf("hello\n");
	printf("area of circle = %f\n", shapeCircle.area(circle));
	printf("area of rectangle = %f\n", shapeRectangle.area(rectangle));


	return 0;
}

