#include "protocolo.c"

int llopen(int porta, int individual);
int llread(int fd, unsigned char ** buffer);
int llwrite(int fd, char *information, int length);
int llclose(int fd, int individual);


