#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>



int fd[2], numbytes = 10000;
long bytesread = 0;
char text[100];


void smoking_break_over(int signum) {
    printf("\nMessage: %s\nBytesread: %d", text, bytesread);
    printf("\nAlarm triggered!\nnumbytes at %d\n", numbytes);
    bytesread = 0;
    numbytes++;
    alarm(1);
}


int main(int argc, char *agrv[]) {
    
    signal(SIGALRM, smoking_break_over);
    
    int pepsi = pipe(fd);
    
    if (pepsi != 0) {
        printf("\nItte bra %d", pepsi);
        exit(0);
    }
    
    printf("You have a pipe! Smoke to your hearts content");
    fflush(stdout);
    alarm(1);

    int pid = fork();
    
    if (pid == 0) {
//      Closing read as it is unused by child
        close(fd[0]);
        while (1) write(fd[1], "Some old Toby!", numbytes);
        close(fd[1]);
    }
    
    else {
//      Closing write as it is unused by parent
        close(fd[1]);
        while (1) bytesread += read(fd[0], text, numbytes);
        close(fd[0]);
    }
    
    return 0;
}


/*Answers to questions in b:
 Bandwidth was mostly around 35,000,000 bytes per second using a MacBokk Pro M1. 
 However, did see numbers up 42 million bytes the first time the program was run.*/
