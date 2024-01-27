#pragma once
int llopen(int porta, int individual);
int llread(int fd, unsigned char * buffer);
int llwrite(int fd, unsigned char *information, int length);
int llclose(int fd, int individual);


