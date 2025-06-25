#include <stdio.h>
#include <stdlib.h>
#define PI 3.14


struct Shape {
    void *data;                            
    float (*area)(void *);         
};


struct Circle {
    float radius;
};

struct Rectangle {
    float length;
    float width;
};

float areaCircle(void *data) {
    struct Circle *c = (struct Circle *)data;
    return PI * c->radius * c->radius;
}

float areaRectangle(void *data) {
    struct Rectangle *r = (struct Rectangle *)data;
    return r->length * r->width;
}

int main() {
  
    struct Circle *c = malloc(sizeof(struct Circle));
    struct Rectangle *r = malloc(sizeof(struct Rectangle));
    c->radius = 5.0;
    r->length = 4.0;
    r->width = 3.0;
    
    struct Shape shape1;
    shape1.data = c;
    shape1.area = areaCircle;
    
    struct Shape shape2;
    shape2.data = r;
    shape2.area = areaRectangle;

    printf("Area of Circle: %f\n", shape1.area(shape1.data));
    printf("Area of Rectangle: %f\n", shape2.area(shape2.data));
    free(c);
    free(r);

    return 0;
}

