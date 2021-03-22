#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void shell_loop(char* input);

int main(int argc, char *argv[]) {
    
    // Task e)
    // Runs a script given as argument
    if (argv[1] != NULL){
        FILE * fp;
        char * user_in = NULL;
        size_t input_lim = 30;
        ssize_t read;

        fp = fopen(argv[1], "r");
        if (fp == NULL)
            exit(EXIT_FAILURE);

        while ((read = getline(&user_in, &input_lim, fp)) != -1) {
            shell_loop(user_in);
        }
    }
    //End task e)

    while (1) {        
        shell_loop(NULL);
    }
    return 0;
}

void shell_loop(char *input){

    // Task a)
    // String parser
    char **ap, *command[10], *user_in;
    size_t input_lim = 30;
    size_t chars;
    int counter = 0;
    char *BINARYPATH;

    if (input != NULL){
        user_in = input;
    }
    else{
        user_in = (char *)malloc(input_lim * sizeof(char));
        printf("\n$: ");
        chars = getline(&user_in, &input_lim, stdin);
    }
    
    for (ap = command; (*ap = strsep(&user_in, " ")) != (void *)0;) {
        if (**ap != (char *)0) {
//          The pointer at index i points to the array at *ap
            command[counter] = *ap;
            counter++;
            if (++ap >= &command[10]) {
                break;
            }
        }
    }
    
    BINARYPATH = command[0];
    // Remove newline from command
    command[counter-1][strlen(command[counter-1])-1] = '\0';

    // End task a)

    //Task d)
    if (strcmp(BINARYPATH, "exit") == 0){
        exit(0);
    }
    if (strcmp(BINARYPATH, "cd") == 0){

        int exit_code = chdir(command[1]);
    
        if (exit_code != 0) {
            perror("An error occured when executing shell command\n");
            printf( "Value of errno: %d\n", errno );
        }
        else {
            setenv("PWD", getcwd(NULL, 0), 1);
            printf("Directory changed. The present working directory is\n \"%s\"\n",getenv("PWD"));
        }
    }
    //End task d)
    
    // Task b+c)
    else {
        int id = fork();
        if (id == 0) {
            // Begin c)
            int redir_in, redir_out, file_in_index, file_out_index;

            for (int i = 0; i < counter; i++) {
                if (strcmp("<", command[i]) == 0) {
                    redir_in = 1;
                    file_in_index = i + 1;
                }
                else if (strcmp(">", command[i]) == 0
                        ) {
                    redir_out = 1;
                    file_out_index = i + 1;
                }
            }
            //Disse endrer fra stdin/-out til en fil, de kicker inn av og til når de ikke skal. Må muligens finne en bedre måte
            if (redir_in == 1) {
                int file = open(command[file_in_index], O_RDONLY, 0777);
                //printf("\nFound <");
                /*
                if (file != 0) {
                    printf("\nFile provided does not exist!\n");
                    exit(0);
                }
                */
                dup2(file, STDIN_FILENO);
                close(file);
                command[file_in_index] = NULL;
                command[file_in_index - 1] = NULL;
            }
            
            if (redir_out == 1) {
                //printf("Found >");
                int file = open(command[file_out_index], O_WRONLY | O_CREAT, 0777);
                dup2(file, STDOUT_FILENO);
                close(file);
                command[file_out_index] = NULL;
                command[file_out_index - 1] = NULL;
            }
            // end c)
            
            // Run command from user
            int exit_code = execvp(BINARYPATH, command);
            
            if (exit_code != 0) {
                perror("An error occured when executing shell command\n");
                printf( "Value of errno: %d\n", errno );
            }
            

            exit(3);
        }
        wait(0);
        //End b)
    }
    if (input == NULL){
        free(user_in);
    }
}