#ifndef BOARD_PINS_H
#define BOARD_PINS_H

#include <Arduino.h>

/**
 * Brochage centralisé du drone, par carte cible.
 *
 * Deux cartes sont supportées :
 * - Seeed XIAO ESP32-S3          (env PlatformIO : xiao-esp32s3)
 * - DFRobot FireBeetle 2 ESP32-S3 (env PlatformIO : firebeetle-esps3)
 *
 * Les broches de repli I2C (MPU_SDA_ALT / MPU_SCL_ALT) servent uniquement au
 * diagnostic : elles ne doivent JAMAIS être des broches moteur, sinon le scan
 * I2C enverrait des impulsions parasites aux ESC.
 */

#if defined(ARDUINO_XIAO_ESP32S3)

// --- Seeed XIAO ESP32-S3 ---
// Moteurs sur D0..D3 (GPIO 1..4) : contigus, sans fonction de boot,
// et sans conflit avec l'I2C (D4/D5) ni le SPI (D8/D10).
#define MOTOR_M1_PIN D0 // GPIO1  - Avant Gauche   (sens horaire)
#define MOTOR_M2_PIN D1 // GPIO2  - Avant Droite   (sens anti-horaire)
#define MOTOR_M3_PIN D2 // GPIO3  - Arrière Droite (sens horaire)
#define MOTOR_M4_PIN D3 // GPIO4  - Arrière Gauche (sens anti-horaire)

// I2C matériel par défaut : D4 = GPIO5 (SDA), D5 = GPIO6 (SCL)
#define MPU_SDA_PIN SDA
#define MPU_SCL_PIN SCL

// Repli de diagnostic : D8/D9 (GPIO 7/8), broches SPI libres.
// D6/D7 (GPIO 43/44) sont volontairement exclues : ce sont TX/RX de l'UART0.
#define MPU_SDA_ALT D8 // GPIO7
#define MPU_SCL_ALT D9 // GPIO8

#define BOARD_NAME "Seeed XIAO ESP32-S3"

#else

// --- DFRobot FireBeetle 2 ESP32-S3 (defaut historique) ---
#define MOTOR_M1_PIN D2 // GPIO3  - Avant Gauche   (sens horaire)
#define MOTOR_M2_PIN D3 // GPIO38 - Avant Droite   (sens anti-horaire)
#define MOTOR_M3_PIN D6 // GPIO18 - Arrière Droite (sens horaire)
#define MOTOR_M4_PIN D5 // GPIO7  - Arrière Gauche (sens anti-horaire)

// I2C matériel par défaut : GPIO1 (SDA), GPIO2 (SCL)
#define MPU_SDA_PIN SDA
#define MPU_SCL_PIN SCL

// Repli de diagnostic : D10/D11 (GPIO 14/13), libres de tout moteur.
#define MPU_SDA_ALT D10 // GPIO14
#define MPU_SCL_ALT D11 // GPIO13

#define BOARD_NAME "DFRobot FireBeetle 2 ESP32-S3"

#endif

#endif /* BOARD_PINS_H */
