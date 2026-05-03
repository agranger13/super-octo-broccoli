#ifndef MPU6050_Example_h
#define MPU6050_Example_h

#include <Wire.h> /**< Bibliothèque pour la communication I2C. */
#include <Adafruit_MPU6050.h> /**< Bibliothèque pour le MPU6050. */
#include <Adafruit_Sensor.h> /**< Bibliothèque pour les capteurs Adafruit. */

/**
 * @brief Structure pour stocker les données du gyroscope.
 *
 * Contient les valeurs de l'accéléromètre et du gyroscope.
 * Ces valeurs sont utilisées pour calculer les angles de pitch et de roll.
 * Les valeurs sont stockées sous forme d'entiers 16 bits pour une meilleure précision.
 */
struct GyroData {
  int16_t ax, ay, az; /**< Valeurs de l'accéléromètre sur les axes X, Y et Z. */
  int16_t gx, gy, gz; /**< Valeurs du gyroscope sur les axes X, Y et Z. */
};

/**
 * @brief Classe pour gérer le gyroscope MPU6050.
 *
 * Cette classe permet d'initialiser, de mettre à jour et d'afficher les données du gyroscope MPU6050.
 * Elle utilise un filtre complémentaire pour fusionner les données de l'accéléromètre et du gyroscope.
 * Le filtre complémentaire permet de réduire le bruit et d'améliorer la précision des mesures.
 * Cette classe est conçue pour être utilisée avec des drones et autres systèmes de stabilisation.
 */
class GyroScoper {
public:
  sensors_event_t a, g, temp; /**< Événements de l'accéléromètre, du gyroscope et de la température. */
  float pitch; /**< Angle de pitch en degrés. */
  float roll;  /**< Angle de roll en degrés. */
  void begin(); /**< Initialise le gyroscope et configure les paramètres. */
  void updateGyro(); /**< Met à jour les données du gyroscope et calcule les angles de pitch et de roll. */
  void printGyro(); /**< Affiche les données du gyroscope sur le port série. */

private:
  Adafruit_MPU6050 mpu; /**< Instance du MPU6050. */
  float pitchAcc, rollAcc; /**< Angles de pitch et roll calculés à partir de l'accéléromètre. */
  float pitchGyro, rollGyro; /**< Angles de pitch et roll calculés à partir du gyroscope. */
  float alpha = 0.96; /**< Coefficient du filtre complémentaire pour fusionner les données de l'accéléromètre et du gyroscope. */
  unsigned long lastTime; /**< Dernière fois que les données ont été mises à jour. */
  float computePitch(float ax, float ay, float az);
  float computeRoll(float ax, float ay, float az);
  void scanI2C();
  /**< Calcule l'angle de roll à partir des données de l'accéléromètre. */
};

#endif /**< Fin de la définition de la classe GyroScoper. */