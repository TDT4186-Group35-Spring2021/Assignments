#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

extern int errno ;

void soundAlarm(){
    printf("\a");
    printf("\a");
}

/*2.2 Function to handle multiple alarms*/
void timerFunc(int n){
    int id = fork();
    if (id == -1){
        perror("An error occured when forking the process\n");
        printf( "Value of errno: %d\n", errno );
        exit(1);
    }
    else if (id > 0){
        printf("The process id of created child is %d\n", id);
    }
    else if(id == 0){
        sleep(n);
        soundAlarm();
        printf("\nTime is up in process %d \n", getpid());
        printf("Enter timer: \n");
        exit(3);
    }
}

/*2.3 Kill loop to handle the zombies*/
/*options will be set to WNOHANG when adding new alarms, and 0 when killing zombies at upon termination*/
void killLoop(int options){
    int kill;
    do {
        kill = waitpid(-1, NULL, options);
        if (kill > 0){
            printf("Child process %d is deceased\n", kill);
        }
        else if (kill == -1){
            perror("An error occured when waiting for child process\n");
            printf( "Value of errno: %d\n", errno );
            exit(1);
        }
    }while (kill > 0);
}

int main() {
    
    printf("\nEnter a negative value for timer to end process\n");
    
    int timer;
    
    // getpid() is always successful, so no need to handle errors
    printf("PID of main process %d\n", getpid());
    
    while (1) {
        
        printf("Enter timer: \n");
        scanf("%d", &timer);

        if (timer < 0) {
            killLoop(0);
            break;
        }

        timerFunc(timer);
        
        killLoop(WNOHANG);
        
        
        
        
        
        // printf("PID of current process %d\n", getpid());
    }
    
    return 0;
}
