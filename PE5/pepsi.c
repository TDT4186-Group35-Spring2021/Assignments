#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

size_t bytesread=0, last=0, max_block=0;
int fd[2];

void alarmHandler(int signum) {
    last = bytesread - last;
    printf("\nBytesread: %zu", bytesread);
    printf("\nBytes last second: %zu", last);
    printf("\nMax block size encountered: %zu", max_block);
    last = bytesread;
    //numbytes *= 10;
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
    signal(SIGALRM, alarmHandler);
    signal(SIGUSR1, killHandler);
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


/*Questions from b (MacBook Pro M1):
1. ≈ 1,000 bytes per block.
2. ≈ 2.3 GB at a 1,000 bytes per block
3. ≈ 1.8 GB with two simultaniously running processes with block sizes of a 1,000 bytes
 */
