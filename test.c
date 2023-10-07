#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char test[1000000];
    int length = 0;

    for (int i = 0; i < 10; i++) {
 
        strcat(test, "a");
        length++;

    }

    test[length] = "\0";

    printf("%s\n", test);

}