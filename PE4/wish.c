#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
    
    while (1) {
        
        char **ap, *argv[10], *user_in;
        size_t input_lim = 30;
        size_t chars;
        int counter = 0;
        
        user_in = (char *)malloc(input_lim * sizeof(char));
        printf("\n$: ");
        chars = getline(&user_in, &input_lim, stdin);
        
        for (ap = argv; (*ap = strsep(&user_in, " ")) != NULL;) {
            if (**ap != NULL) {
    //          The pointer at index i points to the array at *ap
                argv[counter] = *ap;
                counter++;
                if (++ap >= &argv[10]) {
                    break;
                }
            }
        }
        
        
        
        for (int i = 0; i < counter; i++) {
            printf("\nargv[%d] = %s", i, argv[i]);
        }
            
        free(user_in);
        
    }
    
    return 0;
}
