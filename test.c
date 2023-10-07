#include <stdio.h>
#include <string.h>

int main() {
    char string[12];

    snprintf(string, 12, "Hello.co%d", 10);

    printf("%s", string);
}