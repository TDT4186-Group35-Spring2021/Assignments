#include <stdio.h>

static char ui_data;
int counter = 0;

void sum_n(int n)
{
    counter += n;

    if (n != 1)
    {
        sum_n(--n);
    }
}

int main(void)
{
    char declared;
    printf("Address of declared is %p\n", &declared);
    int n = 5;
    printf("Address of n is %p after initializing\n", &n);
    printf("Address of ui is %p\n", &ui_data);
    printf("Address of counter is %p\n", &counter);
    sum_n(n);

    printf("The sum of numbers from 1 to %d is %d \n", n, counter);
    return 0;
}
