#include "GyroScoper.h"

/**
 * @brief Initialisation du gyroscope.
 *
 * Initialise la puce MPU6050 en configurant :
 * - le filtre passe-haut à 0.63 Hz
 * - le seuil de détection de mouvement à 1
 * - la durée de détection de mouvement à 20 ms
 * - la polarité et la rémanence de l'interruption sur les mouvements
 * - l'interruption sur les mouvements
 * Affiche également les valeurs de plage de l'accéléromètre et du gyroscope
 * ainsi que la bande passante du filtre.
 */
void GyroScoper::begin() {
  Wire.begin();
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (true) delay(10);
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }
 
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  lastTime = millis();
  
  // Lecture initiale pour initialiser pitch et roll
  mpu.getEvent(&a, &g, &temp);
  pitch = computePitch(a.acceleration.x, a.acceleration.y, a.acceleration.z);
  roll = computeRoll(a.acceleration.x, a.acceleration.y, a.acceleration.z);
}

/**
 * @brief Mise à jour des données du gyroscope.
 *
 * Lit les données du gyroscope via la méthode getEvent() de la classe
 * Adafruit_MPU6050, puis met à jour les valeurs de pitch (inclinaison) et de
 * roll (roulis) en radians.
 */
void GyroScoper::updateGyro() {
    mpu.getEvent(&a, &g, &temp);
    
    unsigned long currentTime = millis();
    float dt = (currentTime - lastTime) / 1000.0; // dt en secondes
    lastTime = currentTime;
    
    // Calcul des angles à partir de l'accéléromètre
    pitchAcc = computePitch(a.acceleration.x, a.acceleration.y, a.acceleration.z);
    rollAcc = computeRoll(a.acceleration.x, a.acceleration.y, a.acceleration.z);
    
    // Intégration du gyroscope
    pitchGyro = pitch + g.gyro.y * dt * 180.0 / PI; // Conversion rad/s vers deg
    rollGyro = roll + g.gyro.x * dt * 180.0 / PI;
    
    // Filtre complémentaire
    pitch = alpha * pitchGyro + (1 - alpha) * pitchAcc;
    roll = alpha * rollGyro + (1 - alpha) * rollAcc;
}

float GyroScoper::computePitch(int16_t ax, int16_t ay, int16_t az) {
  return atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
}

float GyroScoper::computeRoll(int16_t ax, int16_t ay, int16_t az) {
  return atan2(ay, az) * 180.0 / PI;
}

void GyroScoper::printGyro() {
  updateGyro();
  
  Serial.print("Acc: ");
  Serial.print(a.acceleration.x); Serial.print(" ");
  Serial.print(a.acceleration.y); Serial.print(" ");
  Serial.print(a.acceleration.z); Serial.print(" | ");

  Serial.print("Gyro: ");
  Serial.print(g.gyro.x); Serial.print(" ");
  Serial.print(g.gyro.y); Serial.print(" ");
  Serial.print(g.gyro.z); Serial.print(" | ");

  Serial.print("Pitch: ");
  Serial.print(pitch, 2); 
  Serial.print(" | Roll: ");
  Serial.println(roll, 2);

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");
}