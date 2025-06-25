#include<stdio.h>
#include<stdlib.h>
#define PI 3.14
struct shape
{
	void *ptr;
	float (*fptr)(void *data);
};
struct circle
{
	float radius;
};
struct rectangle
{
	float length;
	float width;
};
float areaCircle(void *data)
{
	struct circle *c=data;
	return PI*c->radius*c->radius;
}
float areaRectangle(void *data)
{
	struct rectangle *r=data;
	return r->length*r->width;
}
int main()
{
	struct circle *c=(struct circle *)malloc(sizeof(struct circle));
	c->radius=5.1;
	struct rectangle *r=(struct rectangle *)malloc(sizeof(struct rectangle));
	r->length=3.1;
	r->width=4.2;
	struct shape s1;
	s1.ptr=c;
	s1.fptr=areaCircle;
	struct shape s2;
	s2.ptr=r;
	s2.fptr=areaRectangle;
	printf("area of circle: %.2f\n",s1.fptr(s1.ptr));
	printf("area of rectangle: %.2f\n",s2.fptr(s2.ptr));
	free(c);
	free(r);
	return 0;
}

