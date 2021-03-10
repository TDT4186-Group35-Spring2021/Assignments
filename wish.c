#include <stdio.h>
#include <stdlib.h>



//---------- Utility functions ----------

void remove_trailing_white_spaces(char *user_in) {
    int i;
}


int get_number_white_spaces(char * user_in) {
    int counter = 0;
    int char_number = (int) (sizeof(user_in)/sizeof(char));
    printf("\nchar_number: %d", char_number);
    for(int i = 0; i < char_number; i++) {
        counter += user_in[i] == ' ';
    }
    return counter;
}





int main() {
    char test[9] = {'E', 'r', ' ', 'd', 'u', ' ', '2', '?'};
//    printf("\nsize of test: %lu", sizeof(test));
//    printf("\nsize of char: %lu", sizeof(test[0]));
    printf("\nNr ws : %d", get_number_white_spaces(test));
  
    return 0;
}
