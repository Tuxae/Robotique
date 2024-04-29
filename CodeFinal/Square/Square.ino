#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h> // pour utiliser PI
// AccelStepper https://www.youtube.com/watch?v=QRCvC5xhJCw
// Pour éviter la surchauffe des moteurs Vref https://www.youtube.com/watch?v=BV-ouxhZamI
// Taille de roue 76mm https://www.decathlonpro.fr/paire-roues-oxeloboard-roulements-id-8127600.html

// Mesures
const int roue = 76; // diamètre de la roue en mm
const int essieu = 450; // distance entre les deux roues en mm
const int xmax = 3000;
const int ymax = 2000;
const int pas_par_tour = 200; // nombre de pas à effectuer pour réaliser un tour

bool line = true;

/*
STEP, DIR
X : 2, 5 (Left)
Y: 3, 6 (Right)
Z: 4, 7
*/

const int enPin = 8; // enable Pin pour allumer le shield
AccelStepper stepper_L(1, 2, 5);  // 1=using_controller, pull, dir
AccelStepper stepper_R(1, 3, 6);

// Pour le gyroscope

# define DT 0.1
unsigned long previousMillis = 0; // Temps du dernier update rotation
float mean_err_gz = 0.0;

// Initialisation de l'objet MPU6050
Adafruit_MPU6050 mpu;



void setup() {
 	Serial.begin(9600);
  // Allumer le shield
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);
  // setup Left
  stepper_L.setMaxSpeed(100.0);
  stepper_L.setAcceleration(50.0);
  // setup Right
  stepper_R.setMaxSpeed(100.0);
  stepper_R.setAcceleration(50.0);

  // Initalize serial monitor
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  // Try to initialize gyroscope
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(100);

  Serial.println("Gyro initialisé");
}

void loop() {
  // Pause execution for 1 second (1000 milliseconds)  
  if (stepper_L.distanceToGo() == 0 && stepper_R.distanceToGo() == 0 ) {
    delay(1000);
    if (line) {
    stepper_L.moveTo(stepper_L.currentPosition() - 400);
    stepper_R.moveTo(stepper_R.currentPosition() - 400);
    line = false;

    } else {
    compute_error_gyro(); // peut-être à mettre dans la void setup
    turn(PI/2);
    line = true;

    }

  }

  unsigned long currentMillis = millis();

  // Run motor
  stepper_L.run();
  stepper_R.run();
  if (!line) {
    float delay = currentMillis - previousMillis
    // Exécute la tâche de rotation toutes les DT millisecondes
    if (delay >= DT) {
      compute_angle_gyro(delay);
      previousMillis = currentMillis;
  }
}



void turn(float angle) { 
  // left = +-1, si = +1, alors le moteur gauche recule et le droit avance pour tourner à gauche

  // Calcul du nombre de pas à effectuer
  long steps = pas_par_tour * (angle / (2*PI)) * (essieu / roue);
  Serial.println("New turn");
  Serial.println(stepper_L.currentPosition());
  Serial.println(stepper_R.currentPosition());
  // Move both stepper motors simultaneously
    stepper_L.moveTo(stepper_L.currentPosition() + steps);
    stepper_R.moveTo(stepper_R.currentPosition() - steps);
}

void compute_error_gyro(){
  float sum_err_gz = 0.0;
  const int n = 1000;
  for (int i = 0; i < n; i++) {
    // Lecture des données du gyroscope
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    // Calcul de la vitesse de rotation (en radian/s)
    float err_gz = g.gyro.z;
    sum_err_gz += err_gz;
    delay(1);
  }
  // Calcul de la moyenne
  mean_err_gz = sum_err_gz / n;
}


void compute_angle_gyro(delay) {
  float angle_gyro = 0.0;
  // Lecture des données du gyroscope
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Lecture des données de rotation (en rad/s)
  float gz = g.gyro.z;
  angle_gyro += (gz-mean_err_gz) * (delay/1000); // Mise à jour de la rotation

  // Affichage de la rotation
  Serial.print(angle_gyro); // Affichage de la rotation
  Serial.print(" ; ");
  Serial.println(angle_gyro * 180 / M_PI); // Affichage de la rotation en degrés
}




