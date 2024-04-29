// https://www.aranacorp.com/fr/utilisation-dun-arduino-cnc-shield-v3/

const int enPin=8;
const int stepXPin = 2; //X.STEP
const int dirXPin = 5; // X.DIR
const int stepYPin = 3; //Y.STEP
const int dirYPin = 6; // Y.DIR
const int stepZPin = 4; //Z.STEP
const int dirZPin = 7; // Z.DIR

const int stepsPerRev=200;
int pulseWidthMicros = 100; 	// microseconds
int millisBtwnSteps = 10000;


void setup() {
 	Serial.begin(9600);
 	pinMode(enPin, OUTPUT);
 	digitalWrite(enPin, LOW);

  // setup X motor
 	pinMode(stepXPin, OUTPUT);
 	pinMode(dirXPin, OUTPUT);

  // setup Y motor
 	pinMode(stepYPin, OUTPUT);
 	pinMode(dirYPin, OUTPUT);

  // setup print
 	Serial.println(F("CNC Shield Initialized"));
}
void loop() {
 	Serial.println(F("Running clockwise"));
 	digitalWrite(dirXPin, HIGH); // Enables the motor to move in a particular direction
 	digitalWrite(dirYPin, HIGH); // Enables the motor to move in a particular direction
 	// Makes 200 pulses for making one full cycle rotation
 	for (int i = 0; i < stepsPerRev; i++) {
 			digitalWrite(stepXPin, HIGH);
 			digitalWrite(stepYPin, HIGH);
 			delayMicroseconds(pulseWidthMicros);
 			digitalWrite(stepXPin, LOW);
 			digitalWrite(stepYPin, LOW);
 			delayMicroseconds(millisBtwnSteps);
 	}
 	delay(1000); // One second delay
 	Serial.println(F("Running counter-clockwise"));
 	digitalWrite(dirXPin, LOW); //Changes the rotations direction
 	digitalWrite(dirYPin, LOW); //Changes the rotations direction
 	// Makes 400 pulses for making two full cycle rotation
 	for (int i = 0; i < 2*stepsPerRev; i++) {
 			digitalWrite(stepXPin, HIGH);
 			digitalWrite(stepYPin, HIGH);
 			delayMicroseconds(pulseWidthMicros);
 			digitalWrite(stepXPin, LOW);
 			digitalWrite(stepYPin, LOW);
 			delayMicroseconds(millisBtwnSteps);
 	}
 	delay(1000);
}