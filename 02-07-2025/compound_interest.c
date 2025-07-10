#include <stdio.h>
#include <math.h>

int main()
{
    double principal, time, rate;
    printf("Enter principal, time and rate of interest: ");
    scanf("%lf %lf %lf", &principal, &time, &rate);

    double amount = principal * pow((1 + rate / 100), time);
    double C_I = amount - principal;

    printf("Compound Interest is: %.2lf\n", C_I);

    return 0;
}

