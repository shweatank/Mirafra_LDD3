#include<stdio.h>//problem1
#include<stdlib.h>
typedef struct 
{
	float radius;
}circle;
typedef struct
{
	float length;
	float width;
}rectangle;
typedef struct 
{
	void *data;
        float (*area)(void *);
}shape;
float areacircle(void *data)
{
	circle *c=data;
return 3.14*c->radius*c->radius;
}
float arearectangle(void *data)
{
	rectangle *r=data;
return r->length*r->width;
}
int main()
{
	circle *c=(circle *)malloc(sizeof(circle));
	rectangle *r=(rectangle *)malloc(sizeof(rectangle));
	c->radius=7;
        shape scircle,srectangle;
	scircle.data=c;
	scircle.area=areacircle;
	r->length=4;
	r->width=3;
        srectangle.data=r;
	srectangle.area=arearectangle;
	printf("area of circle:%f\n",scircle.area(scircle.data));
	printf("area of rectangle:%f\n",srectangle.area(srectangle.data));
}


	
