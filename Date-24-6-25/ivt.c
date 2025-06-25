#include <stdio.h>

void ISR0() 
{ 
	printf("ISR 0: Timer Interrupt\n"); 
}
void ISR1() 
{
       	printf("ISR 1: Keyboard Interrupt\n");
}
void ISR2() 
{ 
	printf("ISR 2: Disk Interrupt\n"); 
}

int main() 
{
    void (*ISR_vector[3])() = {ISR0, ISR1, ISR2};

    int interrupt_id;
    printf("Enter interrupt ID (0-2): ");
    scanf("%d", &interrupt_id);

    if (interrupt_id >= 0 && interrupt_id < 3)
        ISR_vector[interrupt_id]();
    else
        printf("Invalid Interrupt ID\n");

    return 0;
}

