// Potentiometer Script with angle calibration and rolling average
// By Hector Gabriel Fabricius

#include <Arduino.h>

// Pin connections
#define buttonPin 6 
float potPin = A0;

// User settings
int accuracy = 500;
float samples[500];  // Must be set to the same

// Variables
int currentIndex = 0;  // Current index in the samples array
float angle = 0;
float offset = 0;
float sum = 0;


void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
  // Initialize the samples array to 0
  for (int i = 0; i < accuracy; i++) {
    samples[i] = 0;
  }
}

void loop() {
  // Sum of the last 100 samples    
    // Subtract the oldest value from the sum
    sum -= samples[currentIndex];
    
    // Read the latest value from the potentiometer
    samples[currentIndex] = analogRead(potPin);
    
    // Add the latest value to the sum
    sum += samples[currentIndex];

    // Move to the next index in the samples array
    currentIndex = (currentIndex + 1) % accuracy;

  // Calculate the average of the last 100 samples
  angle = sum / accuracy * 120;
  angle = angle / 1023 ;

  if (digitalRead(buttonPin) == HIGH) {
    offset = angle;
    Serial.print("[Calibration mode]    ");
    Serial.print("Current offset: "); Serial.println(offset);
    delay(700);
  } else {
    angle = angle - offset;
    if (offset > 90) angle = -angle; // Note that any offset angle above 90 will flip the orientation of the angle system.
    if (angle>=0) {
      if (angle<100) Serial.print('0');
      if (angle<10) Serial.print('0');
      Serial.println(angle);
    } else {
      if (angle>-100) Serial.print('-');
      if (angle>-10) Serial.print('0');
      Serial.println(angle * (-1));
    }
  }
 // delay(1);
}
