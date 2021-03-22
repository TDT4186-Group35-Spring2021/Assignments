#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>



int main() {
    
    while (1) {
        
        char **ap, *argv[10], *user_in;
        size_t input_lim = 30;
        size_t chars;
        int counter = 0;
        char *BINARYPATH;
        
        user_in = (char *)malloc(input_lim * sizeof(char));
        printf("\n$: ");
        chars = getline(&user_in, &input_lim, stdin);
        
        for (ap = argv; (*ap = strsep(&user_in, " ")) != (void *)0;) {
            if (**ap != (char *)0) {
    //          The pointer at index i points to the array at *ap
                argv[counter] = *ap;
                counter++;
                if (++ap >= &argv[10]) {
                    break;
                }
            }
        }
        
        BINARYPATH = argv[0];
        argv[counter-1][strlen(argv[counter-1])-1] = '\0';
        
        printf("\ncounter = %d", counter);
        
        int id = fork();
        if (id == 0) {
            
            int redir_in, redir_out, file_in_index, file_out_index;

            for (int i = 0; i < counter; i++) {
                if (strcmp("<", argv[i]) == 0) {
                    redir_in = 1;
                    file_in_index = i - 1;
                }
                else if (strcmp(">", argv[i]) == 0
                         ) {
                    redir_out = 1;
                    file_out_index = i + 1;
                }
            }
            //Disse endrer fra stdin/-out til en fil, de kicker inn av og til når de ikke skal. Må muligens finne en bedre måte
            if (redir_in == 1) {
                int file = open(argv[file_in_index], O_RDONLY, 0777);
                printf("\nFound <");
                if (file != 0) {
                    printf("\nFile provided does not exist!\n");
                    exit(0);
                }
                dup2(file, STDIN_FILENO);
            }
            
            if (redir_out == 1) {
                printf("Found >");
                int file = open(argv[file_out_index], O_WRONLY | O_CREAT, 0777);
                dup2(file, STDOUT_FILENO);
            }
            int exit_code = execlp(BINARYPATH, BINARYPATH, argv[1], NULL);
            
            //Det kan være flere argumenter...
            /*switch (counter) {
                                
                case 1:
                    exit_code = execlp(BINARYPATH, BINARYPATH, NULL);
                    break;
                                
                case 2:
                    exit_code = execlp(BINARYPATH, BINARYPATH, argv[1], NULL);
                    break;
                                
                case 3:
                    exit_code = execlp(BINARYPATH, BINARYPATH, argv[1], argv[2], NULL);
                    break;
                                
                case 4:
                    exit_code = execlp(BINARYPATH, BINARYPATH, argv[1], argv[2], argv[3], NULL);
                    break;
                                
                default:
                    exit_code = execlp(BINARYPATH, BINARYPATH, argv[1], argv[2], argv[3], argv[4], NULL);
                    break;
                }*/
            
            
            if (exit_code != 0) {
                printf("\nInvalid Syntax!");
            }
            free(user_in);
        }
        wait(0);
    }
    
    
    return 0;
}

