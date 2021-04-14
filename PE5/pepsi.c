#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

size_t bytesread=0, max_block=0;
int fd[2];

void sigHandler(int signum) {
    printf("\nBytesread: %zu", bytesread);
    //numbytes *= 10;
    alarm(1);
}

int main(int argc, char *argv[]) {
    
    size_t numbytes = atoi(argv[1]);
    printf("Number of bytes: %zu", numbytes);
    char data[100];
    signal(SIGALRM, sigHandler);
    alarm(1);
    
    if(pipe(fd) == -1) {
        perror("\nCould not create pipe");
        return -1;
    }
    
    pid_t pid = fork();
    
    if (pid == 0) {
        //close(fd[0]);
        while (1) {
            if ((write(fd[1], "a", numbytes)) == -1) {
                perror("\nWrite failed");
                return -1;
            }
        }
        close(fd[1]);
    }
    
    else {
        //close(fd[1]);
        while (1) {
            int bytes = read(fd[0], data, numbytes);
            if (bytes == -1) {
                perror("\nRead failed");
                return -1;
            }
            bytesread += (size_t) bytes;
            if (bytes>max_block){
                max_block = bytes;
                printf("\nMax_block size: %zu", max_block);
            }
        }
        close(fd[0]);
    }
    
    return 0;
}
