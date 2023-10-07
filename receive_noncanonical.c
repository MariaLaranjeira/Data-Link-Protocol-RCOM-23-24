// Read from serial port in non-canonical mode
//
// Modified by: Eduardo Nuno Almeida [enalmeida@fe.up.pt]

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include "flags.h"

// Baudrate settings are defined in <asm/termbits.h>, which is
// included by <termios.h>
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define BUF_SIZE 256

volatile int STOP = FALSE;
int state = SET_START;
int fd;
struct termios oldtio;



int llopen(int porta, int person){
	// Program usage: Uses either COM1 or COM2
    const char *serialPortNameBase = "/dev/ttyS";
    
    char serialPortName[sizeof(serialPortNameBase+2)];
    
    strncpy(serialPortName, serialPortNameBase, 9);
    
    strncat(serialPortName, porta);

    if (argc < 2)
    {
        printf("Incorrect program usage\n"
               "Usage: %s <SerialPort>\n"
               "Example: %s /dev/ttyS1\n",
               argv[0],
               argv[0]);
        exit(1);
    }

    // Open serial port device for reading and writing and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    fd = open(serialPortName, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(serialPortName);
        exit(-1);
    }

    struct termios newtio;

    // Save current port settings
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        exit(-1);
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0; // Inter-character timer unused
    newtio.c_cc[VMIN] = 0;  // Blocking read until 5 chars received

    // VTIME e VMIN should be changed in order to protect with a
    // timeout the reception of the following character(s)

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred to
    // by fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    // Loop for input
    unsigned char newbuf[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char
	unsigned char buf[1] = {0};
	
	int snd_a;
	int snd_c;
	
    while (state != SET_STOP)
    {
        // Returns after 5 chars have been input
        int bytes = read(fd, buf, 1);
        //buf[bytes] = '\0'; // Set end of string to '\0', so we can printf
        if(bytes >0){
			switch(state){
				case SET_START:
					if (buf[0] == FLAG)
						state = SET_FLAG_RCV;
					
					break;
					
				case SET_FLAG_RCV:
					if(buf[0] == SND_A){
						state = SET_A_RCV;
						snd_a = buf[0];
					} else if (buf[0] == FLAG){
						break;
					} else {
						state = SET_START;
					}
					break;
						
				case SET_A_RCV:
					if(buf[0] == SET){
						snd_c = buf[0];
						state = SET_C_RCV;
					} else if (buf[0] == FLAG){
						state = SET_FLAG_RCV;
					} else {
						state = SET_START;
					}			
					break;
							
				case SET_C_RCV:
					if(buf[0] == snd_a^snd_c){
						state = SET_BCC_OK;
					} else if (buf[0] == FLAG){
						state = SET_FLAG_RCV;
					} else {
						state = SET_START;
					}			
					break;
					
				case SET_BCC_OK:
					if(buf[0] == FLAG){
						state = SET_STOP;
					} else {
						state = SET_START;
					}		
					break;
				
				default:
					
					break;
			}
		}		

    }
    
    printf("Received SET information.\n");
    
	
	newbuf[0]=FLAG;
	newbuf[1]=RCV_A;
	newbuf[2]=UA;
	newbuf[3]=newbuf[1]^newbuf[2];
	newbuf[4]=FLAG;
	
	int bytes = write(fd, newbuf, BUF_SIZE);
    printf("%d bytes written\n", bytes);
	

    // The while() cycle should be changed in order to respect the specifications
    // of the protocol indicated in the Lab guide

    // Restore the old port settings
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);

    return 0;
}

	
int llread(){
	
	unsigned char newbuf[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char
	unsigned char buf[1] = {0};
	
	int snd_a;
	int snd_c;
	int temp;
	
    while (state != SET_STOP)
    {
        // Returns after 5 chars have been input
        int bytes = read(fd, buf, 1);
        //buf[bytes] = '\0'; // Set end of string to '\0', so we can printf
        if(bytes >0){
			switch(state){
				case SET_START:
					if (buf[0] == FLAG)
						state = SET_FLAG_RCV;
					
					break;
					
				case SET_FLAG_RCV:
					if(buf[0] == SND_A){
						state = SET_A_RCV;
						snd_a = buf[0];
					} else if (buf[0] == FLAG){
						break;
					} else {
						state = SET_START;
					}
					break;
						
				case SET_A_RCV:

					if(buf[0] == IN0){
						snd_c = buf[0];
						state = SET_C_RCV;
					} else if (buf[0] == FLAG){
						state = SET_FLAG_RCV;
					} else {
						state = SET_START;
					}			
					break;
							
				case SET_C_RCV:
					if(buf[0] == snd_a^snd_c){
						state = SET_BCC_OK;
					} else if (buf[0] == FLAG){
						state = SET_FLAG_RCV;
					} else {
						state = SET_START;
					}			
					break;
					
				case SET_BCC_OK:
					if(buf[0] == FLAG){
						state = SET_STOP;
					} else {
						state = SET_START;
					}		
					break;
				
				case RR0:
					
					break;
					
				case RR1:
					break;
					
					
				default:
					break;
			}
		}		

    }
    
    printf("Received SET information.\n");
    
	
	newbuf[0]=FLAG;
	newbuf[1]=RCV_A;
	newbuf[2]=UA;
	newbuf[3]=newbuf[1]^newbuf[2];
	newbuf[4]=FLAG;
	
	int bytes = write(fd, newbuf, BUF_SIZE);
	
}	

int llclose(){
	
}	


int main(int argc, char *argv[])
{
    // Program usage: Uses either COM1 or COM2
    const char *serialPortName = argv[1];

    if (argc < 2)
    {
        printf("Incorrect program usage\n"
               "Usage: %s <SerialPort>\n"
               "Example: %s /dev/ttyS1\n",
               argv[0],
               argv[0]);
        exit(1);
    }

    // Open serial port device for reading and writing and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    int fd = open(serialPortName, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(serialPortName);
        exit(-1);
    }

    struct termios oldtio;
    struct termios newtio;

    // Save current port settings
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        exit(-1);
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0; // Inter-character timer unused
    newtio.c_cc[VMIN] = 0;  // Blocking read until 5 chars received

    // VTIME e VMIN should be changed in order to protect with a
    // timeout the reception of the following character(s)

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred to
    // by fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    // Loop for input
    unsigned char newbuf[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char
	unsigned char buf[1] = {0};
	
	int snd_a;
	int snd_c;
	
    while (state != SET_STOP)
    {
        // Returns after 5 chars have been input
        int bytes = read(fd, buf, 1);
        //buf[bytes] = '\0'; // Set end of string to '\0', so we can printf
        if(bytes >0){
			switch(state){
				case SET_START:
					if (buf[0] == FLAG)
						state = SET_FLAG_RCV;
					
					break;
					
				case SET_FLAG_RCV:
					if(buf[0] == SND_A){
						state = SET_A_RCV;
						snd_a = buf[0];
					} else if (buf[0] == FLAG){
						break;
					} else {
						state = SET_START;
					}
					break;
						
				case SET_A_RCV:
					if(buf[0] == SET){
						snd_c = buf[0];
						state = SET_C_RCV;
					} else if (buf[0] == FLAG){
						state = SET_FLAG_RCV;
					} else {
						state = SET_START;
					}			
					break;
							
				case SET_C_RCV:
					if(buf[0] == snd_a^snd_c){
						state = SET_BCC_OK;
					} else if (buf[0] == FLAG){
						state = SET_FLAG_RCV;
					} else {
						state = SET_START;
					}			
					break;
					
				case SET_BCC_OK:
					if(buf[0] == FLAG){
						state = SET_STOP;
					} else {
						state = SET_START;
					}		
					break;
				
				default:
					
					break;
			}
		}		

    }
    
    printf("Received SET information.\n");
    
	
	newbuf[0]=FLAG;
	newbuf[1]=RCV_A;
	newbuf[2]=UA;
	newbuf[3]=newbuf[1]^newbuf[2];
	newbuf[4]=FLAG;
	
	int bytes = write(fd, newbuf, BUF_SIZE);
    printf("%d bytes written\n", bytes);
	

    // The while() cycle should be changed in order to respect the specifications
    // of the protocol indicated in the Lab guide

    // Restore the old port settings
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);

    return 0;
}
