#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

int main() {
    // Open the serial port

    //                                   SKAL SÆTTES
    //                                        |
    //                                        |
    //                              !!!!!     V      !!!!!
    HANDLE serialHandle = CreateFile("\\\\.\\COM7", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (serialHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening serial port." << std::endl;
        return 1;
    }

    DCB serialParams = {0};
    serialParams.DCBlength = sizeof(serialParams);

    if (!GetCommState(serialHandle, &serialParams)) {
        std::cerr << "Error getting serial port state." << std::endl;
        CloseHandle(serialHandle);
        return 1;
    }

    serialParams.BaudRate = CBR_115200; // Set the baud rate to match the Arduino sketch
    serialParams.ByteSize = 8;
    serialParams.StopBits = ONESTOPBIT;
    serialParams.Parity = NOPARITY;

    if (!SetCommState(serialHandle, &serialParams)) {
        std::cerr << "Error setting serial port state." << std::endl;
        CloseHandle(serialHandle);
        return 1;
    }

    // Continuous loop to read data from the Arduino
    while (true) {
        char buffer[256];
        DWORD bytesRead;
        if (ReadFile(serialHandle, buffer, sizeof(buffer), &bytesRead, NULL)) {
            if (bytesRead > 0) {
                std::cout << "" << buffer << std::endl;
            }
        }

        // Clear input buffer to avoid accumulating old data
        PurgeComm(serialHandle, PURGE_RXCLEAR);

        // Adjust this delay based on your application's requirements
        Sleep(100);

    }

    // Close the serial port (Note: This won't be reached in this example as the loop runs indefinitely)
    CloseHandle(serialHandle);

    return 0;
}


/*
PS C:\Users\fabri\Desktop\HelloWorldCpp\.vscode> cd ..
PS C:\Users\fabri\Desktop\HelloWorldCpp> bash .\run.bash

g++ Main.cpp -o main && ./main.exe
*/