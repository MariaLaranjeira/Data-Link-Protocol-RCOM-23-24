#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *v[3];
    char *serialPortName;

    int i = 10;

    int length = snprintf(NULL, 0, "%d", i);
    char *porta = malloc(length + 1);
    snprintf(porta, length + 1, "%d", i);

    v[0] = "/dev/ttyS";
    v[1] = porta;
    v[2] = "\0";

    serialPortName = malloc(strlen(v[0]) + strlen(v[1]) + 2);

    sprintf(serialPortName, "%s%s", v[0], v[1]);

    printf("%s\n", serialPortName);
}