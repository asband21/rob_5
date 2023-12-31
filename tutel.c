#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include "rull.h"
#include "PID_regulering.h"

volatile int sharedInt = 0;  // Shared variable of the desired angle position
volatile int sharedpwm = 0;  // Shared variable for the input value
volatile double error = 0;   // Shared variable for the input value

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

rolling_average *ra = NULL;
PIDController pid;
double gammel_vinkrl = 0;
double gammel_err = 0;
struct timespec tid_ny, tid_gammel;

double faa_tids_delta()
{
	clock_gettime(CLOCK_REALTIME, &tid_ny);
	long tid_delta = tid_ny.tv_nsec - tid_gammel.tv_nsec;
	tid_gammel = tid_ny; 

	//The system has an x timestamp which restest. the system therefore has be compensated by adding 1000000000 
		//else it would have found a huge error between delta time (tid_delta)
	if(tid_delta < 0)
	{
		tid_delta = tid_delta + 1000000000;
		//printf("%ld\n",tid_delta);
	}
	return tid_delta;
}

// Gets the angle velocity by dividing the angle with the time
double faa_vinkel_hastihed(double vinkel)
{
	double delta_vinkel = vinkel - gammel_vinkrl;
	gammel_vinkrl = vinkel;
	double tid_delta = faa_tids_delta();
	return add_value_get_average(ra,delta_vinkel/(tid_delta/1000000000));
}

//establishes a value with its maximum and minimum limitations as an all-purpose clamp function
double clamp(double value, double min, double max) {
    if(value < min)
	    return min;
    if(value > max)
	    return max;
    return value;
}

// Computes the difference between practical and theoretical angular velocity 
double faa_error_hastihed(double err)
{
	double delta_vinkel = err - gammel_err;
	gammel_err = err;
	double tid_delta = faa_tids_delta();
	return delta_vinkel/(tid_delta/1000000000); 
} 
void kontroller(double vinkel)
{
	double err_lok = (sharedInt - vinkel);
	double tid_del = faa_tids_delta()/1000000000;
	sharedpwm = (int)(updatePID(&pid, (double)sharedInt, vinkel, tid_del));
	printf("vinkel\t%f\terror\t%f\tønsked vinkel\t%d\tshardpwm\t%d\ttid delat\t%f\n",vinkel,err_lok, sharedInt,sharedpwm,tid_del);
}

void ned_telle(int sek)
{
	for(int i = 0; i < sek; i++)
	{
		printf("starter om %d sek\n", sek-i);
		usleep(1000000);
	}
}

void* vinkeThreadFunc(void* arg)
{
	int fd = ((ThreadArgs*)arg)->fd;
	set_interface_attribs(fd, B9600);
	double p = 5.5;
	double i = 0.711;
	double d = 0.00;
	double min = -255;
	double max = 255;
	initPID(&pid, p, i ,d, min , max);
	printf("p\t%f\ti\t%f\td\t%f\tmin\t%f\tmax\t%f\n", p, i ,d, min , max);
	ra = init_rolling_average(10);
	ned_telle(10);


	while (1)
	{
		char buf[256];
		int n;
		int total_read = 0;

		do 
		{
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
			kontroller(value);
		}
		else
			printf(":(");
	}

	close(fd);
}


void* sin_beveag()
{       
	const double maxAmplitude = 100.0; // Peak value
        const double period = 10.0; // Time for one cycle (peak to peak) in seconds
        const double pi = acos(-1.0); // Pi constant
        const int stepsPerSecond = 10; // Resolution: number of steps per
        for (int t = 0; ; ++t)
	{
                double time = (double)t / stepsPerSecond;
                sharedInt = (int)(maxAmplitude * (sin(2 * pi * time / period) / 2.0 + 0.5)) + 5;
                usleep(1000000 / stepsPerSecond); // Wait for the next step (in microseconds)
        }  
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
                sharedInt = (int)clamp(value,0,115);
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
		usleep(5000); // Wait for 10 milliseconds
		tcflush(fd, TCIFLUSH); // Flush input buffer to remove stale data
	}
	close(fd);
}

int main()
{
	char portName[100] = "/dev/ttyACM0"; // Set default port
	int fd_moter = get_serial_port(portName, "morteren", "/dev/ttyACM0");
	int fd_vinke = get_serial_port(portName, "vinke", "/dev/ttyACM1");

	pthread_t inputThread;
	//if (pthread_create(&inputThread, NULL, inputThreadFunc, NULL))
	if (pthread_create(&inputThread, NULL, sin_beveag, NULL))
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
