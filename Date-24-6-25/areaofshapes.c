#include<stdio.h>
#include<stdlib.h>
struct shape
{
	void *data;
	double (*area)(void *data);
};
struct circle
{
	double radius;
};
struct rectangle
{
	double length;
	double breadth;
};

double areaCircle(void *data)
{
	struct circle *circle = (struct circle *)data;
	return 3.14*circle->radius*circle->radius;
}

double areaRectangle(void *data)
{
	struct rectangle *rectangle = (struct rectangle *)data;
	return rectangle->length*rectangle->breadth;
}

int main()
{
	struct circle *c=(struct circle *)malloc(sizeof(struct circle));

	struct rectangle *r=(struct rectangle *)malloc(sizeof(struct rectangle));


	c->radius= 6.0;
	r->length=4.0;
	r->breadth=5.0;

	struct shape areac;
	areac.data=c;
	areac.area=areaCircle;

	struct shape arear;
	arear.data=r;
	arear.area=areaRectangle;

	printf("area of Circle:%lf\n",areac.area(areac.data));
	printf("area of Rectangle:%lf\n",arear.area(arear.data));

	free(c);
	free(r);
}

