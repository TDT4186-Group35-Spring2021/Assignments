#include <stdio.h>

/**
* Group 35 - TDT4186
* Matias, Martin og Lars
*/

static char ui_data;
int counter = 0;

int sum_n(int n)
{
    //int d;
    //printf("Address of d is %p after declaring\n", &d);

    if (n > 1)
    {
        return (n + sum_n(--n));
    }
    else
    {
        return 1;
    }
}

int main(void)
{
    /* Memory address tests
    char a;
    int b = 0;
    double c;
    double d;
    int e;
    char f;
    */

    int n = 65; //Integer overflow at 65536 and seg fault at 261500 < n < 262000
    /* Print addresses
    printf("Address of a, b, c, d, e and f is %p, %p, %p, %p, %p, %p\n", &a, &b, &c, &d, &e, &f);
    printf("Address of n is %p after initializing\n", &n);
    printf("Address of ui is %p\n", &ui_data);
    printf("Address of counter is %p\n", &counter);
    */
    counter = sum_n(n);
    printf("The sum of numbers from 1 to %d is %d \n", n, counter);
    return 0;
}

/**
 * a) 
 * The value where the program stops running correctly depends on two things:
 *  - Integer overflow when using the int data type in C.
 *  - Segmentation fault stemming from exceeding the call stack buffer after too many recursions
 * Integer overflow happens when n = 65 536, and we can see from the output that the output is a 
 * negative number. Ints in C are 4 bytes long, where the last bit specifies whether the integer is 
 * negative or positive. This means the potential maximal value of an int, is 2^31 - 1 = 2 147 483 647.
 * The value of the numbers from 1 to 65536 is (65536*65537)/2 = 2 147 516 416, which is larger than the
 * maximal value of an int. The output overflows the sign bit, turning the sum into a negative number.
 * 
 * However, we could potentially use an integer type with space for larger numbers. In a 64 bit system, the
 * long data type has room for 8 bytes, leading to a max value of 2^63 - 1 = 9 223 372 036 854 775 807. 
 * The program will now run correctly for n = 65 536, but another problem occurs at larger values for n. 
 * It differs from run to run, but for values of n between n = 261500 and n = 262500, the program halts and returns
 * a segmentation fault. The problem is now a matter of available space on the function call stack. Each new call
 * to the recursive function sum_n(n) allocates more space on the call stack. The memory is not freed until the 
 * last recursive call when we call sum_n(1). This means that if the original value of n is large enough, we run out
 * of memory on the call stack before we reach n = 1. On the computer we tested on, this value was between 261700 and
 * 262000 - the exact value varied from run to run.
 *  
 * b)
 * After testing different types in the main function, we found that the memory addresses seemed to be clustered
 * around the type. So, if we define a char, then two ints and then another char, the chars will be placed after 
 * one another in memory, and then the ints are placed next to each other in memory. The distance between the 
 * addresses is usually just the length of variable, so the distance between two chars is 1 byte, the distance between
 * two ints is 4 bytes, the distance between two doubles is 8 bytes and so on. This pattern holds in most cases, but
 * in some cases we also see what is known as "alignment". The compiler prefers addresses which are divisible by the
 * length of the data type. So an int is placed on addresses ending with 0x00, 0x04, 0x08 or 0x0c, as these are divisible 
 * by 4, the number of bytes in an int.
 * We suspect the type clustering happens because its more optimal to cluster according to type than to follow the order in
 * which variables are declared.
 * It's important to note that this might differ between compilers.
 * We used gcc as our compiler for testing.
 * 
 * c) 
 * Global variables are not placed on the call stack, rather they are placed in the data segment of the heap memory.
 * As the heap memory have different memory addresses than the stack, the global variables also have different addresses.
 * In general, heap memory starts with lower addresses and grows upwards, while stack memory start with high addresses and
 * grows downwards, so the global variables have lower addresses than the local variables in main.
 * 
 * d)
 * Local variables in functions are assigned memory addresses on the call stack. In general the call stack grows 
 * from higher addresses to lower memory addresses, so local variables in new recursion calls will be assigned 
 * a memory address later, and therefore also a lower memory address.
**/
