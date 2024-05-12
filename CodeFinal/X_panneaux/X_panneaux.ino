#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h> // pour utiliser PI
#include <Arduino.h>
#include <HC_SR04.h>
// AccelStepper https://www.youtube.com/watch?v=QRCvC5xhJCw
// Pour éviter la surchauffe des moteurs Vref https://www.youtube.com/watch?v=BV-ouxhZamI
// Taille de roue 76mm https://www.decathlonpro.fr/paire-roues-oxeloboard-roulements-id-8127600.html

// Mesures
const int roue = 76; // diamètre de la roue en mm
const int essieu = 203; // distance entre les deux roues en mm (vraie distance : 200, mais ajustée pour que le robot tourne du bon angle)
const int xmax = 3000;
const int ymax = 2000;
const int pas_par_tour = 200; // nombre de pas à effectuer pour réaliser un tour

/*
STEP, DIR
X : 2, 5 (Left)
Y: 3, 6 (Right)
Z: 4, 7 (Left)
*/

const int enPin = 8; // enable Pin pour allumer le shield
AccelStepper stepper_L(1, 4, 7);  // 1=using_controller, pull, dir
AccelStepper stepper_R(1, 3, 6);


// Pour les étapes
int etape = 0;
int etape_decalage_panneau = 0;
unsigned long startTime = 0;

// Define Ultrason
#define TRIG_PIN 5
#define ECHO_PIN 2
HC_SR04<ECHO_PIN> sensor(TRIG_PIN);   // sensor with echo and trigger pin
signed long distance;
unsigned long lastUltra = 0;
unsigned long ultraTime = 0;
// unsigned long interval_ultrason;

// Choix équipe
const int readPin = 11; // Z-limit switch
int team = 0;



void setup() {
 	Serial.begin(9600);
  // Allumer le shield
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);
  // setup Left
  stepper_L.setMaxSpeed(200.0);
  stepper_L.setAcceleration(200.0);
  // setup Right
  stepper_R.setMaxSpeed(200.0);
  stepper_R.setAcceleration(200.0);

  // Initalize Ultrason
  sensor.beginAsync();  
  sensor.startAsync(100000);        // start first measurement
  ultraTime=millis();
  
  // pinMode(TRIG_PIN, OUTPUT);
  // pinMode(ECHO_PIN, INPUT);
  // interval_ultrason = micros();
  distance = -1;
  // Serial.println(distance);
  // Serial.println("Setup");

  // Choix équipe
  pinMode(readPin, INPUT_PULLUP);
  team = digitalRead(readPin);
}

void loop() {
  ultrason_nonbloquant();
  // Pause execution for 1 second (1000 milliseconds)  
  // Serial.println();
  // Serial.print("Etape : ");
  // Serial.println(etape);
  if (etape==0) {
    // Cache Ultrason
    if (millis() - ultraTime > 3000) {
      if (distance != -1 && distance >10) {
        etape = 1;
        startTime=micros();
      }
    }
  } else if (etape == 1) {  // *** Premiere avance ***
    // Avancer 1.7m soit 1700 mm
    avance(2000);
    etape = 2;
  } else if (etape == 2) {
    // Avancer ok
    if (stepper_L.distanceToGo() == 0 && stepper_R.distanceToGo() == 0 ) {
      etape = 3;
    }
  } else if (etape == 3) {  // *** 2e panneau ***
    // Decalage panneau 2
    decalage_panneau();
    if (etape_decalage_panneau == 0) {
      etape = 4;
    }
  } else if (etape == 4) {   // *** 3e panneau ***
    // Decalage panneau 2
    decalage_panneau();
    if (etape_decalage_panneau == 0) {
      etape = 5;
    }
  } else if (etape == 5) {  // *** Retour Base ***
    // Recule
    avance(-430);
    etape = 6;
  } else if (etape == 6) {
    // Avancer ok
    if (stepper_L.distanceToGo() == 0 && stepper_R.distanceToGo() == 0 ) {
      etape = 7;
    }
  } else if (etape == 7) {
    // Rot 2 Align
    turn(80);
    etape = 8;
  } else if (etape == 8) {
    // Check Rot 2 Align
    if (stepper_L.distanceToGo() == 0 && stepper_R.distanceToGo() == 0 ) {
      etape = 9;
    }
  } else if (etape == 9) {
    // Avancer final
    avance(1000);
    etape = 10;
  } else if (etape == 10) {
    // Check Avancer final
    if (stepper_L.distanceToGo() == 0 && stepper_R.distanceToGo() == 0 ) {
      etape = 11;
    }
  } else if (etape == 100) {
    // Desactivation des moteurs
    digitalWrite(enPin, HIGH);
  } else {
    etape=100;
  }
  
  bool run = true;
  if (distance != -1 && distance < 20) {
    // Serial.println("Obstacle detecté");
    run = false;
  }
  if (startTime != 0) {
    if (micros()-startTime > 95000000) {// 95secondes
      run = false;
      etape = 100;
    }
  }
  // Run motor
  if (run) {
  stepper_L.run();
  stepper_R.run();
  }
}

void decalage_panneau() {
  if (etape_decalage_panneau == 0) {
    etape_decalage_panneau = 1;
  } else if (etape_decalage_panneau == 1) {
    // Recule 1
    avance(-200);
    etape_decalage_panneau = 2;
  } else if (etape_decalage_panneau == 2) {
    // Check recule 1
    if (stepper_L.distanceToGo() == 0 && stepper_R.distanceToGo() == 0 ) {
      etape_decalage_panneau = 3;
    }
  } else if (etape_decalage_panneau == 3) {
    // Rot 1
    turn(60);
    etape_decalage_panneau = 4;
  } else if (etape_decalage_panneau == 4) {
    // Check Rot 1
    if (stepper_L.distanceToGo() == 0 && stepper_R.distanceToGo() == 0 ) {
      etape_decalage_panneau = 5;
    }
  } else if (etape_decalage_panneau == 5) {
    // Recule 2 Diag
    avance(-400);
    etape_decalage_panneau = 6;
  } else if (etape_decalage_panneau == 6) {
    // Check Recule 2 Diag
    if (stepper_L.distanceToGo() == 0 && stepper_R.distanceToGo() == 0 ) {
      etape_decalage_panneau = 7;
    }
  } else if (etape_decalage_panneau == 7) {
    // Rot 2 Align
    turn(-60);
    etape_decalage_panneau = 8;
  } else if (etape_decalage_panneau == 8) {
    // Check Rot 2 Align
    if (stepper_L.distanceToGo() == 0 && stepper_R.distanceToGo() == 0 ) {
      etape_decalage_panneau = 9;
    }
  } else if (etape_decalage_panneau == 9) {
    // Avancer final
    avance(600);
    etape_decalage_panneau = 10;
  } else if (etape_decalage_panneau == 10) {
    // Check Avancer final
    if (stepper_L.distanceToGo() == 0 && stepper_R.distanceToGo() == 0 ) {
      etape_decalage_panneau = 0;
    }
  }
}

void turn(float angle) { // angle est en degre
  // left = +-1, si = +1, alors le moteur gauche recule et le droit avance pour tourner à gauche
  if (team == 0) {
    angle = -angle;
  }
  // Calcul du nombre de pas à effectuer
  long steps = (pas_par_tour * angle*essieu) / (360*roue);
  Serial.println("New turn");
  // Move both stepper motors simultaneously
    stepper_L.moveTo(stepper_L.currentPosition() + steps);
    stepper_R.moveTo(stepper_R.currentPosition() - steps);
}

void avance(float distance) {
  long steps = (pas_par_tour * distance) / (PI*roue);
  Serial.println("New line");
  stepper_L.moveTo(stepper_L.currentPosition() + steps);
  stepper_R.moveTo(stepper_R.currentPosition() + steps);
}

void ultrason() {
  
  long duration;
  // Envoie une impulsion ultrasonique
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Mesure la durée de l'impulsion ultrasonique
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Convertit la durée en distance (en cm)
  distance = duration * 0.034 / 2;
  if (distance < 2 || distance > 400) {
    distance = -1;
  } 
  
  // Affiche la distance mesurée sur le moniteur série
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void ultrason_nonbloquant() {
  if (sensor.isFinished()) {
    if ((micros()-lastUltra) > 100000) {
      // On retire cette valeure
      sensor.getDist_cm();

    } else {distance = sensor.getDist_cm();
      if (distance < 2) {
        distance = -1;
      } 
      if (distance > 400) {
        distance = 400;
      }
      // Serial.print("Distance: ");
      // Serial.print(distance);
      // Serial.println(" cm");
    }
    
  // Serial.print("LastUltra: ");
  // Serial.print((lastUltra - micros())/1000000);
  // Serial.println(" ms");
  lastUltra = micros();
  sensor.startAsync(100000);
  }
}
