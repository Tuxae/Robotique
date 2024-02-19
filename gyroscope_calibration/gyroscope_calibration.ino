#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Pause pour attendre l'ouverture du moniteur série
  }

  // Tentative d'initialisation !
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");

  float somme_ax = 0.0;
  float somme_ay = 0.0;
  float somme_az = 0.0;
  float min_ax = 100;
  float min_ay = 100;
  float min_az = 100;
  float max_ax = -100;
  float max_ay = -100;
  float max_az = -100;
  const int nombre_valeurs = 50;

  for (int i = 0; i < nombre_valeurs; i++) {
    // Lecture des données du gyroscope
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Calcul des valeurs d'accélération (en mm/s²)
    float ax = a.acceleration.x;
    float ay = a.acceleration.y;
    float az = a.acceleration.z;

    somme_ax += ax;
    somme_ay += ay;
    somme_az += az;

    if (min_ax > ax) {
      min_ax = ax;
      }
    if (min_ay > ay) {
      min_ay = ay;
      }
    if (min_az > az) {
      min_az = az;
      }
    if (max_ax < ax) {
      max_ax = ax;
      }
    if (max_ay < ay) {
      max_ay = ay;
      }
    if (max_az < az) {
      max_az = az;
      }

    delay(50);
  }

  // Calcul de la moyenne
  float moy_ax = somme_ax / nombre_valeurs;
  float moy_ay = somme_ay / nombre_valeurs;
  float moy_az = somme_az / nombre_valeurs;

  // Affichage des moyennes
  Serial.print("Moyenne ax : ");
  Serial.println(moy_ax);
  Serial.print("Moyenne ay : ");
  Serial.println(moy_ay);
  Serial.print("Moyenne az : ");
  Serial.println(moy_az);

  Serial.print("min_ax : ");
  Serial.print(min_ax);
  Serial.print(", max_ax : ");
  Serial.println(max_ax);

  Serial.print("min_ay : ");
  Serial.print(min_ay);
  Serial.print(", max_ay : ");
  Serial.println(max_ay);

  Serial.print("min_az : ");
  Serial.print(min_az);
  Serial.print(", max_az : ");
  Serial.println(max_az);
}

void loop() {

}