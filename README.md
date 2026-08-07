# Drone ESP Firmware

## Cartes supportées

Le brochage est centralisé dans `include/BoardPins.h` et sélectionné à la
compilation selon la carte :

```bash
platformio run -e firebeetle-esps3   # DFRobot FireBeetle 2 ESP32-S3
platformio run -e xiao-esp32s3       # Seeed XIAO ESP32-S3
```

## Mapping des pins — Seeed XIAO ESP32-S3

### Moteurs (PWM LEDC, 20 kHz, 8 bits)

| Moteur | Position | Sens | Broche carte | GPIO |
|--------|----------|------|--------------|------|
| M1 | Avant Gauche | Horaire | D0 | 1 |
| M2 | Avant Droite | Anti-horaire | D1 | 2 |
| M3 | Arrière Droite | Horaire | D2 | 3 |
| M4 | Arrière Gauche | Anti-horaire | D3 | 4 |

D0–D3 sont contigus, sans fonction de boot, et n'entrent en conflit ni avec
l'I2C ni avec le SPI.

### Capteur MPU6050 (I2C)

| Signal | Broche carte | GPIO | Remarque |
|--------|--------------|------|----------|
| SDA | D4 | 5 | Bus I2C par défaut |
| SCL | D5 | 6 | Bus I2C par défaut |
| VCC | 3V3 | — | Alimentation 3.3 V |
| GND | GND | — | Masse commune avec les ESC |

Repli de diagnostic : D8/D9 (GPIO 7/8).

> ⚠️ **Ne pas utiliser D6/D7 (GPIO 43/44) pour les moteurs** : ce sont TX/RX de
> l'UART0. Les logs de boot du S3 y génèrent des impulsions qui seraient
> interprétées par les ESC.

### Table complète XIAO ESP32-S3

| Dx | GPIO | Dx | GPIO |
|----|------|----|------|
| D0 | 1 | D6 | 43 (TX) |
| D1 | 2 | D7 | 44 (RX) |
| D2 | 3 | D8 | 7 (SCK) |
| D3 | 4 | D9 | 8 (MISO) |
| D4 | 5 (SDA) | D10 | 9 (MOSI) |
| D5 | 6 (SCL) | LED | 21 |

Seules 11 broches sont exposées : après moteurs + I2C, il ne reste que D6–D10.

## Mapping des pins — FireBeetle 2 ESP32-S3

Correspondance entre les broches sérigraphiées sur la carte (`Dx`) et les GPIO de
l'ESP32-S3, telle que définie par la variante `dfrobot_firebeetle2_esp32s3`.

### Moteurs (PWM LEDC, 20 kHz, 8 bits)

| Moteur | Position          | Sens        | Broche carte | GPIO |
|--------|-------------------|-------------|--------------|------|
| M1     | Avant Gauche      | Horaire     | D2           | 3    |
| M2     | Avant Droite      | Anti-horaire| D3           | 38   |
| M3     | Arrière Droite    | Horaire     | D6           | 18   |
| M4     | Arrière Gauche    | Anti-horaire| D5           | 7    |

```
     AVANT
   M1     M2
     \   /
      \ /
      / \
     /   \
   M4     M3
    ARRIERE
```

### Capteur MPU6050 (I2C)

| Signal | Broche carte | GPIO | Remarque                                   |
|--------|--------------|------|--------------------------------------------|
| SDA    | SDA          | 1    | Bus I2C par défaut                         |
| SCL    | SCL          | 2    | Bus I2C par défaut                         |
| VCC    | 3V3          | —    | Alimentation 3.3 V                         |
| GND    | GND          | —    | Masse commune avec les ESC                 |

Des résistances de pull-up de 4.7 kΩ entre SDA/SCL et VCC sont recommandées.

`GyroScoper::begin()` teste d'abord les broches I2C par défaut, puis bascule sur
le repli de diagnostic `D10/D11` (GPIO 14/13), choisi pour ne pas entrer en
conflit avec les sorties moteur.

### Autres broches utiles

| Fonction        | Broche carte | GPIO |
|-----------------|--------------|------|
| UART TX         | TX           | 43   |
| UART RX         | RX           | 44   |
| LED intégrée    | D13          | 21   |
| SPI SCK         | SCK          | 17   |
| SPI MOSI        | MOSI         | 15   |
| SPI MISO        | MISO         | 16   |
| SPI SS          | SS           | 10   |

### Table complète Dx → GPIO

| Dx   | GPIO | Dx   | GPIO |
|------|------|------|------|
| D2   | 3    | D10  | 14   |
| D3   | 38   | D11  | 13   |
| D5   | 7    | D12  | 12   |
| D6   | 18   | D13  | 21   |
| D7   | 9    | D14  | 47   |
| D9   | 0    |      |      |

| Ax   | GPIO | Ax   | GPIO |
|------|------|------|------|
| A0   | 4    | A3   | 8    |
| A1   | 5    | A4   | 10   |
| A2   | 6    | A5   | 11   |

Source : `~/.platformio/packages/framework-arduinoespressif32/variants/dfrobot_firebeetle2_esp32s3/pins_arduino.h`
