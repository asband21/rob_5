#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

int receiveSerial(int COMM_PORT) {
    // Convert the USB_port to a char
    std::string portStr = std::to_string(COMM_PORT);
    const char* portChar = ("\\\\.\\COM" + portStr).c_str();


    HANDLE serialHandle = CreateFile(portChar, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

//Error type 1
    if (serialHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening serial port." << std::endl;
        return 1;
    }

    DCB serialParams = {0};
    serialParams.DCBlength = sizeof(serialParams);

//Error type 2
    if (!GetCommState(serialHandle, &serialParams)) {
        std::cerr << "Error getting serial port state." << std::endl;
        CloseHandle(serialHandle);
        return 1;
    }

//Comm settings
    serialParams.BaudRate = CBR_115200;
    serialParams.ByteSize = 8;
    serialParams.StopBits = ONESTOPBIT;
    serialParams.Parity = NOPARITY;

//Error type 3
    if (!SetCommState(serialHandle, &serialParams)) {
        std::cerr << "Error setting serial port state." << std::endl;
        CloseHandle(serialHandle);
        return 1;
    }

    // Main loop for recieving serial
    while (true) {
        char buffer[256];
        DWORD bytesRead;
        if (ReadFile(serialHandle, buffer, sizeof(buffer), &bytesRead, NULL)) {
            if (bytesRead > 0) {
                std::cout << buffer << std::endl;
            }
        }
            // Every 5 seconds we clear the serial queue to avoid build up
                PurgeComm(serialHandle, PURGE_RXCLEAR);
                Sleep(5);
    }
}

//float interpretSerial(){
//    1+1;
//}