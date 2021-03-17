#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


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
            if (**ap != '\0') {
    //          The pointer at index i points to the array at *ap
                argv[counter] = *ap;
                
                counter++;
                if (++ap >= &argv[10]) {
                    break;
                }
            }
        }
        
        // Parsing adds extra null char, so we need to remove it...
        int temp = counter - 1;
        argv[temp][strlen(argv[temp])-1] = 0;
        
        /*
        for (int i = 0; i < counter; i++) {
            printf("\nargv[%d] = %s", i, argv[i]);
        }
        */
        
        char* binaryPath = argv[0];
        int id = fork();

        if (id == 0){
            int error = execv(binaryPath, argv);
            if (error == -1){
                perror("An error occured when executing shell command\n");
                printf( "Value of errno: %d\n", errno );
            }
            exit(3);
        }

        wait(0);
            
        free(user_in);
        
    }
    
    return 0;
}
