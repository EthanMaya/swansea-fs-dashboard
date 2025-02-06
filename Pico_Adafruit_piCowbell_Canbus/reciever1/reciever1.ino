/*
 * Adafruit MCP2515 FeatherWing CAN Receiver Example
 */

#include <Adafruit_MCP2515.h>
#include <Wire.h>

#ifdef ESP8266
   #define CS_PIN    2
#elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3)
   #define CS_PIN    14
#elif defined(TEENSYDUINO)
   #define CS_PIN    8
#elif defined(ARDUINO_STM32_FEATHER)
   #define CS_PIN    PC5
#elif defined(ARDUINO_NRF52832_FEATHER)  /* BSP 0.6.5 and higher! */
   #define CS_PIN    27
#elif defined(ARDUINO_MAX32620FTHR) || defined(ARDUINO_MAX32630FTHR)
   #define CS_PIN    P3_2
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
   #define CS_PIN    7
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040_CAN)
   #define CS_PIN    PIN_CAN_CS
#elif defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_RASPBERRY_PI_PICO_W) // PiCowbell CAN Bus
   #define CS_PIN    20
#else
    // Anything else, defaults!
   #define CS_PIN    5
#endif

//I2C
#define I2C_SLAVE_ADDR 0x08  // Address of the ESP32 (slave)
#define PICO_SDA 0           // GP0 (SDA)
#define PICO_SCL 1 

// Set CAN bus baud rate Should be 1000000 for Haltech ECU
#define CAN_BAUDRATE (1000000)

Adafruit_MCP2515 mcp(CS_PIN);
//My code
int i = 0;

void setup() {
  //I2C
  Wire.setSDA(PICO_SDA);     // Set SDA pin
  Wire.setSCL(PICO_SCL); 
  Wire.begin();
  //CANBUS
  Serial.begin(115200);
  while(!Serial) delay(10);

  Serial.println("MCP2515 Receiver test!");

  if (!mcp.begin(CAN_BAUDRATE)) {
    Serial.println("Error initializing MCP2515.");
    while(1) delay(10);
  }
  Serial.println("MCP2515 chip found");
}

void loop() {
  //I2C
  // Data to send (replace with your actual data)
  uint8_t dataToSend[] = {0x01, 0x02, 0x03, 0x04};
  size_t dataLength = sizeof(dataToSend);

  // Send data to the ESP32
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write(dataToSend, dataLength);
  Wire.endTransmission();

  Serial.println("Data sent to ESP32");

  delay(1000); 

  //CANBUS
  // try to parse packet
  int packetSize = mcp.parsePacket();

  //my code
  if (i%1000000==0) {
    Serial.println("This prints every 1000000 loops");
  }
  i++;


  if (packetSize) {
    // received a packet
    Serial.print("Received ");

    if (mcp.packetExtended()) {
      Serial.print("extended ");
    }

    if (mcp.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print("packet with id 0x");
    Serial.print(mcp.packetId(), HEX);
    Serial.println();

    if (mcp.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.println(mcp.packetDlc());
    } else {
      Serial.print(" and length ");
      Serial.println(packetSize);

      // only print packet data for non-RTR packets
      while (mcp.available()) {
        Serial.print((char)mcp.read());
      }
      Serial.println();
    }

    Serial.println();
  }
}