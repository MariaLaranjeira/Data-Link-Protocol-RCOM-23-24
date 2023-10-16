#pragma once

typedef struct Details {
    int port;
    unsigned char* filename;
    long int filesize;
    short entity; // 1 - transmitter, 0 - receiver
    int bytes_per_packet;
} Details;

int aplication(struct Details* details);