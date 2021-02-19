#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void soundAlarm(){
    printf("\a");
    printf("\a");
}

// Alarm loop
void timerFunc(int n){
    sleep(n);
    soundAlarm();
    printf("\nTime is up!\n");
}


int main() {
    
    printf("\nEnter a negative value for timer to end process\n");
    
    int timer;
    
    // printf("PID of main process %d\n", getpid());
    
    while (1) {
        
        printf("Enter timer: \n");
        scanf("%d", &timer);
        
        if (timer < 0) {
            break;
        }
        
        timerFunc(timer);
        // printf("PID of current process %d\n", getpid());
    }
    
    return 0;
}
