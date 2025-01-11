/*
 Board: D1 Mini V3 Nodemcu with ESP8266-12F WLAN module compatible with Arduino, https://www.az-delivery.de/en/products/d1-mini-v3
 Board selection in Arduino: Generic ESP8266


 JR, 08-2023 - 08-2024

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board" e.g. Generic ESP8266 Module
*/
#include <Arduino.h>
// IR-Remote Control
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Define Pin Mapping, comments refer to ESP8266-ESP32-IoT-Board_V2
#define D0 16 // D0P (PWR-OUTPUT) / D0 (INPUT) (also used during Programming (USB only, not when using OTA)
#define D1  5 // D1P (PWR-OUTPUT) / Stepper Coil 1
#define D2  4 // D2P (PWR-OUTPUT) / Stepper Coil 2
#define D3  0 // SCL (also used during programming (USB + OTA)
#define D4  2 // SDA / LEDBUILT_IN (also used during programming (USB + OTA)
#define D5 14 // D5P (PWR-OUTPUT) / Stepper Coil 3
#define D6 12 // D6P (PWR-OUTPUT) / Stepper Coil 4
#define D7 13 // D7P (PWR-OUTPUT) / IR-Sender
#define D8 15 // D8P (PWR-OUTPUT) / Servo-PWM
#define RX  3 // unused
#define TX  1  // unused

// Stepper Motor defines, referring to ESP8266-ESP32-IoT-Board_V2
#define COIL1 D1
#define COIL2 D2
#define COIL3 D5
#define COIL4 D6

// I2C defines, referring to ESP8266-ESP32-IoT-Board_V2
#define I2C_SCL D3
#define I2C_SDA D4

// IR defines, referring to ESP8266-ESP32-IoT-Board_V2
#define IR_LED  D7

// Servo define, referring to ESP8266-ESP32-IoT-Board_V2
#define SERVO_ENABLE  D0
#define SERVO_PWM     D8

// IR Raw Data Buffer
#define RAW_DATA_BUFFER_SIZE 67
//uint16_t rawDataBuff[] = {0};
uint16_t rawDataBuff[] = {441, 1341, 1361, 422, 441, 1342, 1334, 448, 441, 1342, 1335, 448, 441, 1343, 1333, 449, 440, 1343, 1334, 448, 439, 1345, 1334, 448, 441, 1343, 1334, 449, 466, 1317, 1334, 448, 438, 1344, 442, 1341, 466, 1317, 443, 1340, 467, 1315, 441, 1342, 1334, 448, 440, 1340, 467, 13309, 437, 1344, 1332, 451, 460, 1324, 1355, 428, 439, 1344, 1355, 427, 462, 1321, 1353, 430, 464, 1319, 1354, 429, 461, 1321, 1355, 429, 461, 1322, 1353, 429, 459, 1325, 1351, 432, 460, 1323, 458, 1323, 458, 1325, 457, 1326, 457, 1326, 455, 1327, 1324, 460, 455, 1324, 454, 13321};

// Init Objects
IRsend irsend(IR_LED); // Setting up IRsend object

// Standard Setup
void setup()
{
  Serial.begin(115200);
  while (!Serial);

  // Start the ir-sender object
  irsend.begin();
  irsend.sendRaw(rawDataBuff, sizeof(rawDataBuff)/2, 38);
}

//Repeated Loop
void loop()
{

}
