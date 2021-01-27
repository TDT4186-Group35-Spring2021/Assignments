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
    char a, c;
    int b;

    int n = 2000;
    printf("Address of declared is %p, %p, %p\n", &a, &b, &c);
    printf("Address of n is %p after initializing\n", &n);
    printf("Address of ui is %p\n", &ui_data);
    printf("Address of counter is %p\n", &counter);
    sum_n(n);

    printf("The sum of numbers from 1 to %d is %d \n", n, counter);
    return 0;
}

/**
 * a) 
 * 
 * b)
 * In our example, the variables "declared" and "n" are both local variables defined in main().
 * As they are defined within main, they are placed on the call stack in memory, which grows 
 * downwards. Thus, "n" will get a lower address than "declared" as it is initialized after "declared" is declared.
 * The difference between the memory addresses of "declared" and "n" is 1 byte, as the size of a char is 1 byte.
 * 
 * c) 
 * Global variables are not placed in the call stack, 
 * rather they are placed in the initialized data segment.
 * 
 * d)
 * Local variables are placed on the call stack that starts with high memory addresses,
 * and new data is placed on progressively lower memory addresses.
**/