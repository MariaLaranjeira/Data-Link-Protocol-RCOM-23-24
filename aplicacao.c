#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include "flags.h"
#include "protocolo.h"


int fd;

char* filename;
int filesize;
char* buffer;

short individual;

FILE * fp;



int main(int argc, char* argv[]) {

    if (argc < 3)
        {
            if (argv[2] == "1" && argc < 4) { 
                printf("Incorrect program usage\n"
                    "Usage: %s <SerialPortNumber> <Transmitter = 1 or Receiver = 0> <Filename>\n"
                    "Example: %s 1 1 a.txt (Port number 1, Transmitter and File Name 'a.txt')\n",
                    argv[0],
                    argv[0]);
                exit(1);
            }
            else {
                printf("Incorrect program usage\n"
                    "Usage: %s <SerialPortNumber> <Transmitter = 1 or Receiver = 0>\n"
                    "Example: %s 1 0 (Port number 1 and Receiver)\n",
                    argv[0],
                    argv[0]);
                exit(1);
            }
        }

        int num;
        sscanf(argv[1], "%d", &num);
        sscanf(argv[2], "%d", &individual);        

        unsigned char *message;

        int fd = llopen(num, individual);
        if (sizeof(argv[3]) > 0 && individual) {
            fp = fopen(filename, "w+");
            int message_length = 0;
            char *argv3_count = argv[3];
            char *argv3 = argv[3];
            while(*argv3_count) {
                message_length++;
                argv3_count++;
            }
            int bytes_written = llwrite(fd, argv3, message_length);
            printf("Wrote %d bytes to llwrite().\n", bytes_written);
        } else if (!individual) {
            int bytes_read = llread(fd, &message);
            printf("Read %d bytes to llread().\n", bytes_read);
            printf("%s\n", message);
        }

        llclose(fd);

    return 0;

}


