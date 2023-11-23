
// This is the Master script for the P5 project. This will connect the arduino and the EPOS4 systems.
// This will also contain all control structure for the robotic system.
// This version is by Hector Fabricius

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <sstream>
#include <iomanip>

double potAngle = 0;
int COMM_PORT = 11; // Set COM port here

int main()
{
    // Setup Serial communication
    std::string portStr = std::to_string(COMM_PORT);
    const char *portChar = ("\\\\.\\COM" + portStr).c_str();
    HANDLE serialHandle = CreateFile(portChar, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    // Error type 1
    if (serialHandle == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error opening serial port." << std::endl;
        return 1;
    }

    DCB serialParams = {0};
    serialParams.DCBlength = sizeof(serialParams);

    // Error type 2
    if (!GetCommState(serialHandle, &serialParams))
    {
        std::cerr << "Error getting serial port state." << std::endl;
        CloseHandle(serialHandle);
        return 1;
    }

    // Comm settings
    serialParams.BaudRate = CBR_115200;
    serialParams.ByteSize = 8;
    serialParams.StopBits = ONESTOPBIT;
    serialParams.Parity = NOPARITY;

    // Error type 3
    if (!SetCommState(serialHandle, &serialParams))
    {
        std::cerr << "Error setting serial port state." << std::endl;
        CloseHandle(serialHandle);
        return 1;
    }
    while (true)
    {
        char buffer[60];
        char linebuffer[6];
        DWORD bytesRead;
        if (ReadFile(serialHandle, buffer, sizeof(buffer), &bytesRead, NULL))
        {
            if (bytesRead > 0)
            {
                // This is where the magic happens //
                for (int index = 0; index < sizeof(buffer); index++)
                {
                    if (buffer[index] == '.') // Finding the dot on individual strings to navigate from.
                    {
                        for (int i = 0; i < sizeof(linebuffer); i++) // Copying the relevant line onto a new buffer.
                        {
                            linebuffer[i] = buffer[index - 3 + i];
                        }

                        if (isdigit(linebuffer[5]) && (std::atof(linebuffer) != 0)) // These two tests are enough to remove all bad data.
                        {
                            potAngle = std::atof(linebuffer);
                        }
                        else
                        {
                            // std::cout << "Bad Data" << std::endl;
                        }
                    }
                }
            }
        }
        // Every 5 seconds we clear the serial queue to avoid build up
        PurgeComm(serialHandle, PURGE_RXCLEAR);
        Sleep(5);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////  END OF THE ARDUINO SECTION  //////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::cout << potAngle << std::endl; // Here the angles are printed. Remove this later so the terminal can be used for other stuff.

    } // This is where the main loop actually ends.
}