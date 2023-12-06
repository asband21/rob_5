#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h>
#include <termios.h>
#include <errno.h>

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
int main() {
	char portName[100] = "/dev/ttyACM1"; // Set default port
	printf("Enter the serial port (default /dev/ttyACM1): ");
	fgets(portName, sizeof(portName), stdin);

	// Check for newline only input and retain default if found
	if (portName[0] == '\n')
	{
		strcpy(portName, "/dev/ttyACM1");
	}
	else
	{
		portName[strcspn(portName, "\n")] = 0; // Remove newline character
	}

	int fd = open(portName, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
		printf("Error opening %s: %s\n", portName, strerror(errno));
		return -1;
	}
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
			printf("floten er: %f\n", value);
		}
		else
			printf(":(");
	}

	close(fd);
	return 0;
}
