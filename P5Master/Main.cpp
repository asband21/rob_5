
// This is the Master script for the P5 project. This will connect the arduino and the EPOS4 systems.
// This will also contain all control structure for the robotic system.
// This version is by Hector Fabricius

#include "serialFunctions.h"
#include <iostream>

int main() {

 connectSerial(11); //Change COMM port here

   std::cout << "beef" << std::endl;

    while(true) // The main loop
    {
      //  std::cout << getAngle << std::endl;
    }


  return 0;
}
