#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flags.h"
#include "protocolo.h"
#include "aplicacao.h"

int aplication(struct Details* details) {

    FILE* file;

    printf("Attempting to open port %d\n", details->port);

    int fd = llopen(details->port, details->entity);
    if (fd == -1) {
        printf("Error opening port\n");
        return -1;
    }

    printf("Port opened\n");

    if (details->entity == 1) {
        file = fopen(details->filename, "rb");

        if (file == NULL) {
            printf("Error opening file\n");
            return -1;
        }

        unsigned char *start_ctrl = malloc(5 + strlen(details->filename) + sizeof(details->filesize));

        start_ctrl[0] = C_START;
        start_ctrl[1] = FILE_SIZE;
        start_ctrl[2] = sizeof(details->filesize);
        
        for (int i = 0; i < sizeof(details->filesize); i++) {
            start_ctrl[3 + i] = details->filesize >> (i * 8);
        }

        start_ctrl[3 + sizeof(details->filesize)] = FILE_NAME;
        start_ctrl[4 +sizeof(details->filesize)] = strlen(details->filename);
        for (int i = 0; i < sizeof(details->filename); i++) {
            start_ctrl[5 + sizeof(details->filesize) + i] = details->filename[i];
        }

        int bytes = llwrite(fd, start_ctrl, 5 + strlen(details->filename) + sizeof(details->filesize));
        if (bytes == -1) {
            printf("Error sending start control packet\n");
            return -1;
        }

        printf("Start control packet sent\n");
        printf("Starting to send data packets\n");

        unsigned char* data = malloc(details->bytes_per_packet + 3);

        printf("Sending %ld packets\n", details->filesize / details->bytes_per_packet + 1);

        for (int i = 0; i < details->filesize; i += details->bytes_per_packet) {

            printf("Current packet: %d\n", i/details->bytes_per_packet);
            
            data[0] = C_DATA;
            int L1_size = details->bytes_per_packet % 256;
            int L2_size = details->bytes_per_packet - (L1_size * 256);

            if (i + details->bytes_per_packet > details->filesize) {
                L1_size = (details->filesize - i) % 256;
                L2_size = (details->filesize - i) - (L1_size * 256);
            }
            
            data[1] = L1_size;
            data[2] = L2_size;

            int bytes_read = fread(data + 3, 1, details->bytes_per_packet, file);
            if (bytes_read < details->bytes_per_packet && !feof(file)) {
                if (feof(file)) {
                    printf("End of file reached\n");
                } else if (ferror(file)) {
                    printf("Error reading file\n");
                }
                return -1;
            }

            bytes = llwrite(fd, data, bytes_read);
            if (bytes == -1) {
                printf("Error sending data packet\n");
                return -1;
            }
        }

        printf("Data packets sent\n");
        printf("Sending end control packet\n");

        unsigned char end_ctrl[5 + strlen(details->filename) + sizeof(details->filesize)];
        end_ctrl[0] = C_START;
        end_ctrl[1] = FILE_SIZE;
        end_ctrl[2] = sizeof(details->filesize);
        
        for (int i = 0; i < sizeof(details->filesize); i++) {
            end_ctrl[3 + i] = details->filesize >> (i * 8);
        }

        end_ctrl[3 + sizeof(details->filesize)] = FILE_NAME;
        end_ctrl[4 +sizeof(details->filesize)] = strlen(details->filename);
        for (int i = 0; i < sizeof(details->filename); i++) {
            end_ctrl[5 + sizeof(details->filesize) + i] = details->filename[i];
        }

        bytes = llwrite(fd, end_ctrl, 5 + strlen(details->filename) + sizeof(details->filesize));
        if (bytes == -1) {
            printf("Error sending end control packet\n");
            return -1;
        }

        printf("End control packet sent\n");
        printf("File sent\n");

        free(data);
        

    } else if (details->entity == 0) {
        
        int state = C_START;
        unsigned char* packet = malloc(details->bytes_per_packet + 3);

        while(state != C_END) {
            int bytes = llread(fd, packet);
            if (bytes == -1) {
                printf("Error reading packet\n");
                return -1;
            }

            printf("%d bytes read\n", bytes);

            for (int i = 0; i < bytes; i++) {
                printf("%x ", packet[i]);
            }

            switch (state) {
                case C_START:
                    if (packet[0] == C_START) {
                        state = C_DATA;
                        printf("Start control packet received\n");
                        if (packet[1] == FILE_SIZE) {
                            details->filesize = packet[3];
                        }
                        if (packet[4] == FILE_NAME) {
                            details->filename = malloc(packet[5]);
                            for (int i = 0; i < packet[5]; i++) {
                                details->filename[i] = packet[6 + i];
                            }
                        }

                        file = fopen(details->filename, "wb");

                        if (file == NULL) {
                            printf("Error creating/opening file\n");
                            return -1;
                        }

                        printf("File created/opened\n");
                        printf("Starting to receive data packets\n");

                        free(details->filename);
                    } 
                case C_DATA:
                    if (packet[0] == C_DATA){
                        state = C_DATA;
                        int bytes_written = fwrite(packet + 3, 1, packet[2] + packet[1] * 256, file);
                        if (bytes_written < packet[2] + packet[1] * 256) {
                            printf("Error writing to file\n");
                            return -1;
                        }
                    }
                    else if (packet[0] == C_END){
                        state = C_END;
                        printf("End control packet received\n");
                        if (packet[3] != details->filesize) {
                            printf("Error: file size doesn't match\n");
                        }
                        for (int i = 0; i < packet[5]; i++) {
                            if (packet[6 + i] != details->filename[i]) {
                                printf("Error: file name doesn't match\n");
                            }
                        }
                        printf("File received\n");
                        printf("File name: %s\n", details->filename);
                        printf("File size: %ld\n", details->filesize);
                    }
                    break;   
            }
        } 
        free(packet);   

    }

    

    int ret = llclose(fd, details->entity);
    if (ret == -1) {
        printf("Error closing connection\n");
        return -1;
    }

    fclose(file);

    return 0;
}