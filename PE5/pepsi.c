#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char *agrv[]) {
    
    int fd[2], bytesread;
    
    char text[100];
    
    int pepsi = pipe(fd);
    
    if (pepsi != 0) {
        printf("\nItte bra %d", pepsi);
        exit(0);
    }
    
    printf("You have a pipe! Smoke to your hearts content");
    fflush(stdout);
    
    int pid = fork();
    
    if (pid == 0) {
//      Closing read as it is unused by child
        close(fd[0]);
        write(fd[1], "Some old Toby!", 100);
        close(fd[1]);
    }
    
    else {
//      Closing write as it is unused by parent
        close(fd[1]);
        bytesread = read(fd[0], text, 100);
        close(fd[0]);
        printf("\nMessage: %s\nBytesread: %d", text, bytesread);
    }
    
    return 0;
}
