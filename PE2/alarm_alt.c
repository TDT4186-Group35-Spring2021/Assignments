#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Function for playing sound when an alarm is done
void soundAlarm(){
    printf("\a");
    printf("\a");
}

// Main function for fork and running of alarm
void timerFunc(int n){
    int id = fork();
    if(id == 0){
        sleep(n);
        soundAlarm();
        printf("\nTime is up in process %d \n", getpid());
        printf("Enter timer: \n");
        exit(3);
    }
}


int main() {

    printf("\nEnter a negative value for timer to end process\n");

    int timer;

    printf("PID of main process %d\n", getpid());

    while (1) {

        printf("Enter timer: \n");
        scanf("%d", &timer);

        //Culling of zombies will be done here.
        waitpid(-1, NULL, 0);
        
        printf("PID of current process %d\n", getpid());
    }

    //sleep(10);
    return 0;
}
