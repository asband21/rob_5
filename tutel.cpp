#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <cstring> 
#include <errno.h>

std::mutex dataMutex;
std::condition_variable dataCond;
std::queue<std::string> commandQueue;
bool exitFlag = false;

int set_interface_attribs(int fd, int speed)
{

	struct termios tty;

	if (tcgetattr(fd, &tty) < 0)
	{
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

	if (tcsetattr(fd, TCSANOW, &tty) != 0)
	{
		printf("Error from tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

void* userInputThread(void* arg)
{
	while (!exitFlag)
	{
		std::string input;
		std::cout << "Enter new blink rate in milliseconds: ";
		std::getline(std::cin, input);

		if (!input.empty())
		{
			std::lock_guard<std::mutex> lock(dataMutex);
			commandQueue.push(input);
			dataCond.notify_one();
		}
	}
	return nullptr;
}

int main() {
	pthread_t inputThread;
	pthread_create(&inputThread, nullptr, userInputThread, nullptr);

	int sensorFd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_SYNC); // Sensor Arduino
	int controlFd = open("/dev/ttyACM1", O_RDWR | O_NOCTTY | O_SYNC); // Control Arduino

	if (sensorFd < 0 || controlFd < 0)
	{
		std::cerr << "Error opening serial ports." << std::endl;
		return 1;
	}

	set_interface_attribs(sensorFd, B115200);
	set_interface_attribs(controlFd, B9600);

	while (!exitFlag)
	{
		// Read sensor data from the first Arduino
		char sensorBuffer[60];
		read(sensorFd, sensorBuffer, sizeof(sensorBuffer));
		// Process sensor data...

		// Check for user commands
		std::unique_lock<std::mutex> lock(dataMutex);
		if (!commandQueue.empty())
		{
			std::string command = commandQueue.front();
			commandQueue.pop();
			lock.unlock();

			// Send command to the control Arduino
			write(controlFd, command.c_str(), command.length());
			// Optionally read response...
		}
		else
		{
			lock.unlock();
		}

		usleep(10000); // Adjust as necessary
	}

	pthread_join(inputThread, nullptr);
	close(sensorFd);
	close(controlFd);
	return 0;
}
