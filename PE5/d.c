#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

size_t bytesread=0, last=0, max_block=0;
int fd[2];

void alarmHandler(int signum) {
    last = bytesread - last;
    printf("\nBytesread: %zu", bytesread);
    printf("\nBytes last second: %zu", last);
    printf("\nMax block size encountered: %zu", max_block);
    last = bytesread;
    alarm(1);
}

void killHandler(int signum) {
    printf("\n!!!!!!!!!!!!!!!!\nKILL ALARM Bytesread: %zu\n!!!!!!!!!!!!!!!!", bytesread);
}

int main(int argc, char *argv[]) {
    
    size_t numbytes = atoi(argv[1]);
    printf("Number of bytes: %zu", numbytes);
    printf("\nParent id: %d", getpid());
    char data[100];
    
    if (signal(SIGALRM, alarmHandler) == SIG_ERR) {
        perror("\nSignal failed for SIGALRM");
        return -1;
    }
    
    if (signal(SIGUSR1, alarmHandler) == SIG_ERR) {
        perror("\nSignal failed for SIGUSR1");
        return -1;
    }
    
    alarm(1);
    
    if (unlink("Solo") == -1) perror("\nunlink failed!");
    
    if (mkfifo("Solo", 0666) == -1){
        perror("\nCould not make named pipe");
        return -1;
    }
    
    pid_t pid;
    if ((pid = fork()) == -1) {
        perror("\nSpoon");
        return -1;
    }
    
    
    if (pid == 0) {
        
        close(fd[0]);
        
        if ((fd[1] = open("Solo", O_WRONLY)) == -1 ) {
            perror("\nCould not open SOLO (WRITE)");
            return -1;
        }
        
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
        if ((fd[0] = open("Solo", O_RDONLY)) == -1 ) {
            perror("\nCould not open SOLO (READ)");
            return -1;
        }
        
        while (1) {
            int bytes = read(fd[0], data, numbytes);
            if (bytes == -1) {
                perror("\nRead failed");
                return -1;
            }
            bytesread += (size_t) bytes;
            //Uncomment for task a
            //printf("\nBytesread: %zu", bytesread);
            if (bytes>max_block){
                max_block = bytes;
            }
        }
        close(fd[0]);
    }
    
    return 0;
}
