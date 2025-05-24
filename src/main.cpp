#include <Arduino.h>
#include <Hotspot.h>
#include <UdpListener.h>
#include <PIDController.h>
#include <GyroScoper.h>
#include <I2CScanner.h>
#include <MotorController.h>


Hotspot hotspot("NodeMCU_IOT", "123456789");
UdpListener udpListener;
// Kp, Ki, Kd, dt (en secondes)  
PIDController rollPid(1.0, 0.5, 0.2, 0.01); // Rotation sur x
PIDController pitchPid(1.0, 0.5, 0.2, 0.01); // Rotation sur y
PIDController yawPid(1.0, 0.5, 0.2, 0.01); // Rotation sur z
PIDController throttlePid(1.0, 0.5, 0.2, 0.01); // Altitude

GyroScoper gyroScoper;

#define M1 D5 // Avant Gauche
#define M2 D6 // Avant Droite
#define M3 D7 // Arriere Gauche
#define M4 D8 // Arriere Droite
MotorController motorController(D5, D6, D7 ,D8);

void setup() {
  Serial.begin(9600);
  while (!Serial)
    delay(10);

  if (hotspot.begin()) {
    Serial.print("Hotspot démarré à : ");
    Serial.println(hotspot.getIP());
  } else {
    Serial.println("Échec du démarrage du hotspot");
  }

  if (udpListener.begin()) {
    Serial.println("UDP en écoute !");
  }

  rollPid.setOutputLimits(-200, 200);

  gyroScoper.begin();

  motorController.stopAll();
}


void loop()
{
  udpListener.listen();
  gyroScoper.printGyro();

  int i = 1;
  if (i % 2 == 1) {
    motorController.setSpeed(100, 100, 100, 100);
  } else {
    motorController.stopAll();
  }

  delay(1000);
}