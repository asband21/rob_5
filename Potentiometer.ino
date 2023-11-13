// Potentiometer Script with angle calibration
// By Hector Gabriel Fabricius

//Pin connections
#define buttonPin 6 
int potPin = A0;

//Variables
long samples = 0; int angle = 0; int offset = 0;

//User settings
int accuracy = 200; // Increasing accuracy decreases sample rate, but reduces flickering in the data. 
                    // Accuracy to mean sample rate: 10000 ~ 0.8Hz 1000 ~ 8Hz, 100 ~ 80Hz, 10 ~ 800Hz, 1 ~ 8000Hz

void setup() {
  pinMode(buttonPin, INPUT);}
void loop() {
  
 for( int i = 0; i < accuracy; i++) {
  samples = samples + analogRead(potPin);
 }
 samples = samples / accuracy;
 angle = map(samples, 0, 1023, 0.00, 120.00);

 if(digitalRead(buttonPin) == HIGH) {
  offset = angle;
  Serial.print("[Calibration mode]    ");
  Serial.print("Current offset: "); Serial.println(offset);
  delay(700);
 }
 else{
  angle = angle - offset;
  Serial.print("Current angle: "); Serial.println(angle);
 }
 samples = 0;
}
