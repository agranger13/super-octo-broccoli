#ifndef MPU6050_Example_h
#define MPU6050_Example_h

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

struct GyroData {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
};

class GyroScoper {
public:
  void begin();
  void updateGyro();
  void printGyro();

private:
  Adafruit_MPU6050 mpu;
  sensors_event_t a, g, temp;
  float pitch; // en degrés
  float roll;  // en degrés
  
  float computePitch(int16_t ax, int16_t ay, int16_t az);
  float computeRoll(int16_t ax, int16_t ay, int16_t az);
};

#endif