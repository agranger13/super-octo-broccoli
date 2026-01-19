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
 * Vérifie également la connexion au MPU6050 et affiche un message de succès.
 */
void GyroScoper::begin() {
  Serial.println("=== MPU6050 INITIALIZATION ===");
  
  // Test avec différentes configurations I2C
  bool i2cSuccess = false;
  
  // Configuration 1: Broches par défaut
  Serial.println("Testing default I2C pins...");
  Wire.begin(SDA, SCL);
  scanI2C();
  
  if (!mpu.begin()) {
    Serial.println("Failed with default pins. Trying alternative configurations...");
    
    // Configuration 2: Autres broches courantes sur ESP32
    Serial.println("Testing alternative I2C pins (D3, D5)...");
    Wire.begin(D3, D5);
    scanI2C();
    
    if (!mpu.begin()) {
      Serial.println("Failed with alternative pins. Trying another configuration...");
      
      // Configuration 3: Autres broches
      Serial.println("Testing another I2C configuration (D6, D7)...");
      Wire.begin(D6, D7);
      scanI2C();
      
      if (!mpu.begin()) {
        Serial.println("CRITICAL: MPU6050 not found on any configuration!");
        Serial.println("Please check:");
        Serial.println("1. Physical wiring (SDA, SCL, VCC, GND)");
        Serial.println("2. Power supply (try both 3.3V and 5V)");
        Serial.println("3. I2C pull-up resistors (4.7K ohm between SDA/SCL and VCC)");
        Serial.println("4. MPU6050 module functionality (try on another board)");
        Serial.println("5. Correct I2C address (should be 0x68 or 0x69)");
        while (true) delay(10);
      } else {
        i2cSuccess = true;
        Serial.println("SUCCESS: MPU6050 found on alternative pins D6, D7");
      }
    } else {
      i2cSuccess = true;
      Serial.println("SUCCESS: MPU6050 found on alternative pins D4, D5");
    }
  } else {
    i2cSuccess = true;
    Serial.println("SUCCESS: MPU6050 found on default pins");
  }

  if (i2cSuccess) {
    Serial.println("Configuring MPU6050...");
    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    lastTime = millis();
    
    // Lecture initiale pour initialiser pitch et roll
    mpu.getEvent(&a, &g, &temp);
    pitch = computePitch(a.acceleration.x, a.acceleration.y, a.acceleration.z);
    roll = computeRoll(a.acceleration.x, a.acceleration.y, a.acceleration.z);
    
    Serial.println("MPU6050 initialized successfully!");
    Serial.println("=== END MPU6050 INITIALIZATION ===\n");
  }
}

void::GyroScoper::scanI2C() {
  byte error, address;
  int nDevices = 0;

  Serial.println("=== I2C SCAN DEBUG ===");
  Serial.println("Scanning I2C bus for devices...");
  Serial.print("Using SDA: "); Serial.print(SDA);
  Serial.print(", SCL: "); Serial.println(SCL);
  
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      Serial.println(address, HEX);
      
      // Vérification spécifique pour MPU6050 (adresse par défaut 0x68 ou 0x69)
      if (address == 0x68 || address == 0x69) {
        Serial.print("  -> MPU6050 detected at 0x");
        Serial.println(address, HEX);
      }
      nDevices++;
    } else if (error == 2) {
      // Erreur 2 = NACK reçu (périphérique présent mais pas de réponse)
      // Cela peut indiquer un problème d'alimentation ou de pull-up
    } else if (error == 3) {
      // Erreur 3 = problème de transmission
      Serial.print("  -> Transmission error at 0x");
      Serial.println(address, HEX);
    } else if (error == 4) {
      // Erreur 4 = autre erreur
      Serial.print("  -> Other error at 0x");
      Serial.println(address, HEX);
    }
  }

  Serial.print("Scan completed. Found ");
  Serial.print(nDevices);
  Serial.println(" device(s).");
  
  if (nDevices == 0) {
    Serial.println("NO I2C DEVICES FOUND!");
    Serial.println("Possible issues:");
    Serial.println("1. Check power supply (3.3V or 5V)");
    Serial.println("2. Check SDA/SCL connections");
    Serial.println("3. Check I2C pull-up resistors (4.7K ohm)");
    Serial.println("4. Try different I2C pins");
    Serial.println("5. Check if MPU6050 is properly seated");
  }
  Serial.println("=== END I2C SCAN ===\n");
}


/**
 * @brief Mise à jour des données du gyroscope.
 *
 * Lit les données du gyroscope via la méthode getEvent() de la classe
 * Adafruit_MPU6050, puis met à jour les valeurs de pitch (inclinaison) et de
 * roll (roulis) en radians.
 * Vérifie également que les données sont correctement lues et affiche un message d'erreur si nécessaire.
 */
void GyroScoper::updateGyro() {
    if (!mpu.getEvent(&a, &g, &temp)) {
        return;
    }
    
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

/**
 * @brief Calcule l'angle de pitch à partir des données de l'accéléromètre.
 *
 * Utilise la fonction atan2 pour calculer l'angle de pitch en degrés.
 * @param ax Valeur de l'accéléromètre sur l'axe X.
 * @param ay Valeur de l'accéléromètre sur l'axe Y.
 * @param az Valeur de l'accéléromètre sur l'axe Z.
 * @return Angle de pitch en degrés.
 */
float GyroScoper::computePitch(int16_t ax, int16_t ay, int16_t az) {
  return atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
}

/**
 * @brief Calcule l'angle de roll à partir des données de l'accéléromètre.
 *
 * Utilise la fonction atan2 pour calculer l'angle de roll en degrés.
 * @param ax Valeur de l'accéléromètre sur l'axe X.
 * @param ay Valeur de l'accéléromètre sur l'axe Y.
 * @param az Valeur de l'accéléromètre sur l'axe Z.
 * @return Angle de roll en degrés.
 */
float GyroScoper::computeRoll(int16_t ax, int16_t ay, int16_t az) {
  return atan2(ay, az) * 180.0 / PI;
}

/**
 * @brief Affiche les données du gyroscope.
 *
 * Met à jour les données du gyroscope et affiche les valeurs de l'accéléromètre,
 * du gyroscope, du pitch, du roll et de la température.
 * Affiche également un message de succès pour indiquer que les données ont été mises à jour.
 */
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
  
  Serial.println("MPU6050 data updated successfully");
}