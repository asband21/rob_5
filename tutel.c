#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

volatile int sharedInt = 0;  // Shared variable for the input value
volatile int sharedpwm = 0;  // Shared variable for the input value
volatile double error = 0;  // Shared variable for the input value

typedef struct {
    int fd;
} ThreadArgs;
			    
int set_interface_attribs(int fd, int speed)
{
	struct termios tty;

	if (tcgetattr(fd, &tty) < 0) {
		printf("Error from tcgetattr: %s\n", strerror(errno));
		return -1;
	}

	cfsetospeed(&tty, (speed_t)speed);
	cfsetispeed(&tty, (speed_t)speed);

	tty.c_cflag |= (CLOCAL | CREAD);    
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;         
	tty.c_cflag &= ~PARENB;     
	tty.c_cflag &= ~CSTOPB;     
	tty.c_cflag &= ~CRTSCTS;    

	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;

	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 1;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("Error from tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

int get_serial_port(char *portName, char * enhed, char *standart)
{	
	printf("Enter the serial port of the %s (default %s ):", enhed, standart);
	fgets(portName, sizeof(portName), stdin);

	// Check for newline only input and retain default if found
	if (portName[0] == '\n')
	{
		strcpy(portName, standart);
	}
	else
	{
		portName[strcspn(portName, "\n")] = 0; // Remove newline character
	}

	int fd = open(portName, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
		printf("Error opening %s: %s\n", portName, strerror(errno));
		exit(1);
	}
	return fd;
}

void kontroller(double err)
{
	sharedpwm = (int)(err*2);
	printf("error %d ønsked vinkel %d \n",(int)err, sharedInt );
}

void* vinkeThreadFunc(void* arg)
{
	int fd = ((ThreadArgs*)arg)->fd;
	set_interface_attribs(fd, B9600);

	while (1)
	{
		char buf[256];
		int n;
		int total_read = 0;

		do {
			n = read(fd, buf + total_read, sizeof(buf) - 1 - total_read);
			if (n > 0) total_read += n;
		} while (buf[total_read-1] != '\n' && total_read < sizeof(buf) - 1);

		buf[total_read] = 0; // Null terminate
		char* endPtr;
		errno = 0;  // Reset errno before the conversion

		float value = strtof(buf, &endPtr);

		if ( !(endPtr == buf || errno != 0 ))
		{
			//printf("error er: %f\n", sharedInt-value);
			kontroller((float)sharedInt-value);
		}
		else
			printf(":(");
	}

	close(fd);
}


void* inputThreadFunc(void* arg)
{
    char input[256];

    while (1)
    {
        printf("Enter an integer: ");
        if (fgets(input, sizeof(input), stdin) != NULL)
	{
            char* endPtr;
            long value = strtol(input, &endPtr, 10);
            // Check if the conversion was successful and if the whole input is a number
            if (endPtr != input && (*endPtr == '\n' || *endPtr == '\0'))
                sharedInt = (int)value;
	    else
                printf("Invalid input ignored.\n");
        }
    }
    return NULL;
}

void skriv_til_moter_bloker(int fd)
{
	set_interface_attribs(fd, B9600);

	while (1)
	{
		char input[256];
		sprintf(input, "%d", sharedpwm-255);
		//printf("%s\n",input);
		write(fd, input, strlen(input));

		usleep(10000); // Wait for 10 milliseconds
		//

		tcflush(fd, TCIFLUSH); // Flush input buffer to remove stale data

		/*
		// Read confirmation from Arduino
		char buf[256];
		int n;
		int total_read = 0;

		// Read until we get a newline
		do {
			n = read(fd, buf + total_read, sizeof(buf) - 1 - total_read);
			if (n > 0) total_read += n;
		} while (buf[total_read-1] != '\n' && total_read < sizeof(buf) - 1);

		buf[total_read] = 0; // Null terminate
		printf(" PPM Pulsus with:%s", buf); // buf already contains newline
		*/
	}
	close(fd);
}

int main()
{
	char portName[100] = "/dev/ttyACM0"; // Set default port
	int fd_moter = get_serial_port(portName, "morteren", "/dev/ttyACM0");
	int fd_vinke = get_serial_port(portName, "vinke", "/dev/ttyACM1");


	pthread_t inputThread;
	// Create a new thread for input
	if (pthread_create(&inputThread, NULL, inputThreadFunc, NULL))
	{
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

	ThreadArgs args = {fd_vinke};
	pthread_t angelThread;
	// Create a new thread for input
	if (pthread_create(&angelThread, NULL, vinkeThreadFunc, &args))
	{
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

	skriv_til_moter_bloker(fd_moter);

	pthread_join(inputThread, NULL);
	
	return 0;
}


