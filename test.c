#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char* test = malloc(21);

    for (int i = 0; i < 20; i++) {
        test[i] = 'a';
    }

    test[21] = '\0';

    printf("%s", test);

}