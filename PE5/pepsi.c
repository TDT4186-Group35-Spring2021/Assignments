#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

size_t numbytes=1, bytesread=0;
int fd[2];

void sigHandler(int signum) {
    printf("\nNumbytes: %zu\nBytesread: %zu", numbytes, bytesread);
    numbytes *= 10;
    alarm(1);
}

int main(int argv, char *argc[]) {
    
    char data[100];
    signal(SIGALRM, sigHandler);
    alarm(1);
    
    if(pipe(fd) == -1) {
        perror("\nCould not create pipe");
        return -1;
    }
    
    pid_t pid = fork();
    
    if (pid == 0) {
        close(fd[0]);
        while (1) {
            if ((write(fd[1], "a", numbytes)) == -1) {
                perror("\nWrite failed");
                return -1;
            }
        }
        close(fd[1]);
    }
    
    else {
        close(fd[1]);
        while (1) {
            int bytes = read(fd[0], data, numbytes);
            if (bytes == -1) {
                perror("\nRead failed");
                return -1;
            }
            bytesread += (size_t) bytes;
        }
        close(fd[0]);
    }
    
    return 0;
}
