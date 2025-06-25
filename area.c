//24 June 2025
//generic structure with void pointer and function pointer to find area
#include<stdio.h>
#include<stdlib.h>
typedef float(*calculate_area)(void *);
struct genericshape
{
	void *data;
	calculate_area area;
};
float areaCircle(void *data)
{
	float area=0;
	int radius=*(int *)data;
	area=3.14*radius*radius;
	return area;
}
float areaRectangle(void *data)
{
	int *dimension=(int *)data;
	return dimension[0]*dimension[1];
}
void main()
{
	struct genericshape *circle,*rectangle;
	int lw[2]={10,5},radius=10;
	circle=(struct genericshape *)malloc(sizeof(struct genericshape));
	rectangle=(struct genericshape *)malloc(sizeof(struct genericshape));
	circle->data=(void *)&radius;
	circle->area=areaCircle;
	rectangle->data=(void *)lw;
	rectangle->area=areaRectangle;
	printf("Rectangle = %f\n",(*(rectangle->area))(rectangle->data));
	printf("Circle = %f\n",(*(circle->area))(circle->data));
	free(circle);
	free(rectangle);
}
