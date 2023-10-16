#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aplicacao.h"

Details det;

int main(int argc, char* argv[]) {

    printf("Starting program\n");

    if (argc < 3) {
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

    if (argc == 4 || argc == 5) {

        printf("Attempting to open file to verify its size\n");

        det.filename = malloc(strlen(argv[3]) + 1);
        strcpy(det.filename, argv[3]);

        file = fopen(det.filename, "r");

        if (file == NULL) {
            printf("Error opening file to verify its size!\n");
            exit(1);
        }

        fseek(file, 0, SEEK_END);

        det.filesize = ftell(file);

        printf("Setting the bytes per packet\n");

        if (argc == 5)  {
            int bytes_per_packet;
            sscanf(argv[4], "%d", &bytes_per_packet);
            det.bytes_per_packet = bytes_per_packet;
        }
        else
        det.bytes_per_packet = 256;

        fclose(file);
    }
    else if (argc > 5) {
        printf("Incorrect program usage\n"
            "Usage: %s <SerialPortNumber> <Transmitter = 1 or Receiver = 0> <#Optional: Filename> <#Optional: Bytes per Packet>\n"
            "Example: %s 1 1 a.txt 256 (Port number 1, Transmitter, File Name 'a.txt' and 256 bytes per packet sent)\n",
            argv[0],
            argv[0]);
        exit(1);          
    }

    printf("Opening the aplication layer.\n");
    int result = aplication(&det);
    if (result != 0) {
        printf("Error in aplication\n");
        exit(1);
    }

    return 0;

}


