#include <stdio.h>

typedef struct 
{
    void (*callback)(int);
} Event;

void print_double(int x)
{
    printf("Double: %d\n", 2 * x);
}

void trigger_event(Event e, int val) 
{
    e.callback(val);
}

int main() 
{
    Event e;
    e.callback = print_double;

    trigger_event(e, 10);  
    return 0;
}

