#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aplicacao.h"

int fd;

char* filename;
int filesize;
char* buffer;

Details det;

int main(int argc, char* argv[]) {

    if (argc < 2)
        {
            printf("Incorrect program usage\n"
                "Usage: %s <SerialPortNumber> <Transmitter = 1 or Receiver = 0> <#Optional: Filename> <#Optional: Bytes per Packet>\n"
                "Example: %s 1 1 a.txt 256 (Port number 1, Transmitter, File Name 'a.txt' and 256 bytes per packet sent)\n",
                argv[0],
                argv[0]);
            exit(1);
        }

        memset(&det, 0, sizeof(det));

        int num;
        short individual;
        sscanf(argv[1], "%d", &num);
        sscanf(argv[2], "%hd", &individual);

        det.entity = individual;
        det.port = num;

        FILE* file;

        if (argc == 3 || argc == 4) {
            det.filename = argv[3];

            file = fopen(argv[3], "r");

            if (file == NULL) {
                printf("Error opening file!\n");
                exit(1);
            }

            fseek(file, 0, SEEK_END);

            det.filesize = ftell(file);

            if (argc == 4)  {
                int bytes_per_packet;
                sscanf(argv[4], "%d", &bytes_per_packet);
                det.bytes_per_packet = bytes_per_packet;
            }
            else
            det.bytes_per_packet = 256;

            fclose(file);
        }
        else if (argc > 4) {
            printf("Incorrect program usage\n"
                "Usage: %s <SerialPortNumber> <Transmitter = 1 or Receiver = 0> <#Optional: Filename> <#Optional: Bytes per Packet>\n"
                "Example: %s 1 1 a.txt 256 (Port number 1, Transmitter, File Name 'a.txt' and 256 bytes per packet sent)\n",
                argv[0],
                argv[0]);
            exit(1);          
        }

        int result = aplication(&det);
        if (result != 0) {
            printf("Error in aplication\n");
            exit(1);
        }

    return 0;

}


