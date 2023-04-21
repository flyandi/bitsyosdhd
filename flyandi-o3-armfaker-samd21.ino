// O3 Arm Faker
// Inspired by https://github.com/ramiss/arduino_DJI_03_RC_ARM
// This is using the power of the SAMD21 to make the O3 arm in
// full mode. Also includes a PAN servo.

// Writing
// SAMDX1
// A9 SBUS
// A8 Servo
// TX/RX DJI

// Libraries
#include <ReefwingMSP.h>
#include <ReefwingSBUS.h>
#include <Servo.h>

// XAMD Serial2 on A10/A9
#define PIN_SERIAL2_TX (10ul)  // TX on A10
#define PIN_SERIAL2_RX (9ul)   // RX on A9
#define PAD_SERIAL2_TX (UART_TX_PAD_2)
#define PAD_SERIAL2_RX (SERCOM_RX_PAD_1)
Uart Serial2(&sercom0, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);

// Debug
//#define DEBUG

// Definitions
#define MIN_PULSEWIDTH 1000
#define IDLE_PULSEWIDTH 1100
#define MAX_PULSEWIDTH 2000
#define MIN_SBUS 172
#define MAX_SBUS 1811
#define SWITCH_ON 1400
#define MSP_STATUS_EX 150  //out message		 For OSD ‘Fly mode', For OSD ‘Disarmed’

// Locals
ReefwingMSP msp;
SBUS receiver(Serial2);
Servo pan;

// msp def
msp_api_version_t api;
msp_ident_t identReply;
msp_packet_t packet;
msp_fc_variant_t variant;

struct msp_status_DJI_t {
  uint16_t cycleTime;
  uint16_t i2cErrorCounter;
  uint16_t sensor;
  uint32_t flightModeFlags;
  uint8_t configProfileIndex;
  uint16_t averageSystemLoadPercent;       // 0...100
  uint16_t armingFlags;                    //0x0103 or 0x0301
  uint8_t accCalibrationAxisFlags;         //0
  uint8_t DJI_ARMING_DISABLE_FLAGS_COUNT;  //25
  uint32_t djiPackArmingDisabledFlags;     //(1 << 24)
} __attribute__((packed));

uint32_t flightModeFlags = 0x00000002;
msp_status_DJI_t status_DJI = { 0 };

uint32_t update_t = 0;
uint16_t channels[16];
uint16_t rxArm = 0, rxPan = 0;
bool failSafe;
bool lostFrame;
bool armed = false;


void setup() {

  // Start Stuff
  #ifdef DEBUG
    Serial.begin(115200);
  #endif
  Serial1.begin(115200);
  msp.begin(Serial1);
  receiver.begin();
  pan.attach(A8);

  // msp stuff
  identReply.multiWiiVersion = 0;
  identReply.multiType = QUADX;
  identReply.mspVersion = MSP_PROTOCOL_VERSION;
  identReply.capability = MSP_FEATURE_VBAT;
  strcpy(variant.flightControlIdentifier, REEFWING_IDENTIFIER);

  // led
  pinMode(LED_BUILTIN, OUTPUT);

  // lets wait
  delay(1500);
}

void loop() {
  if (receiver.read(&channels[0], &failSafe, &lostFrame)) {

    rxPan = map(channels[5], MIN_SBUS, MAX_SBUS, MIN_PULSEWIDTH, MAX_PULSEWIDTH);
    rxArm = channels[6];
    armed = rxArm >= SWITCH_ON;
    pan.write(map(rxPan, MIN_PULSEWIDTH, MAX_PULSEWIDTH, 0, 180));  

    //  Display SBUS Data
    #ifdef DEBUG
      Serial.print(rxPan);
      Serial.print("\t");
      Serial.print(rxArm);
      Serial.print("\t");
      Serial.println(armed ? "ARMED" : "N/A");
    #endif
  }

  if (armed) {
    flightModeFlags = 0x00000003;     // armed
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED on
  } else if (armed == 0) {
    flightModeFlags = 0x00000002;    // disarmed
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED off
  }

  if(millis() - update_t > 100) {
    status_DJI.flightModeFlags = flightModeFlags;
    status_DJI.armingFlags = 0x0303;
    msp.send(MSP_STATUS_EX, &status_DJI, sizeof(status_DJI));
    status_DJI.armingFlags = 0x0000;
    msp.send(MSP_STATUS, &status_DJI, sizeof(status_DJI));
    update_t = millis();
  }
}


void SERCOM0_Handler(void) {
  Serial2.IrqHandler();
}