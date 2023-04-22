/*
 * O3 Arm Faker+ for SAMD21
 *
 * Inspired by https://github.com/ramiss/arduino_DJI_03_RC_ARM this firmware
 * aims to produce a way to get most out of your O3 without having an FC.
 * Great for your fixed wing aircraft or simple builds.
 *
 *
 * @author FLYANDI (flyandi@yahoo.com)
 */

// Writing
// SAMDX1
// A9 SBUS
// A8 Servo
// TX/RX DJI
#include "def.h"

// ----------------------------------------------------------------
// Configuration
// ----------------------------------------------------------------

// If enabled shows serial messages over USB
#define DEBUG

// Receiver Input
#define RX_INPUT RX_INPUT_SBUS

#define RX_CHANNEL_ARM 6 // RX Channel for ARM (it's channel-1)
#define RX_CHANNEL_PAN 5 // RX Channel for PAN

// Battery Output
#define VBAT_OUTPUT
#define VBAT_MODIFIER 30.49 // this is the modifer for the vbat
#define VBAT_SAMPLE_SIZE 10

// Pan Servo Output
#define PAN_OUTPUT // comment out to disable

#ifdef PAN_OUTPUT
#define PAN_PIN A8
#endif

..

// ----------------------------------------------------------------
// DO NOT CHANGE ANYTHING AFTER HERE EXCEPT YOU KNOW WHAT YOU DOING
// ----------------------------------------------------------------

#include <ReefwingMSP.h>
    ReefwingMSP msp;

#if RX_INPUT == RX_INPUT_SBUS
#include <ReefwingSBUS.h>
SBUS receiver(Serial2);
#endif

#ifdef PAN_OUTPUT
#include <Servo.h>
Servo pan;
uint16_t rxPan = 0;
#endif

// msp def
msp_api_version_t api;
msp_ident_t identReply;
msp_packet_t packet;
msp_fc_variant_t variant;
msp_status_DJI_t status_DJI = {0};
msp_battery_DJI_t status_battery = {0};

uint32_t flightModeFlags = 0x00000002;
uint32_t update_t = 0;
uint16_t channels[16];
uint16_t rxArm = 0;
bool failSafe;
bool lostFrame;
bool armed = false;

#ifdef VBAT_OUTPUT
float vbat = 0;
float vbat_sum = 0;
uint8_t vbat_count = 0;
#endif

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
#endif
  Serial1.begin(115200);
  msp.begin(Serial1);
#if
#if RX_INPUT == RX_INPUT_SBUS
  receiver.begin();
#endif
#ifdef PAN_OUTPUT
  pan.attach(PAN_PIN);
#endif

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

void loop()
{
#if RX_INPUT == RX_INPUT_SBUS
  if (receiver.read(&channels[0], &failSafe, &lostFrame))
  {

    rxArm = channels[RX_CHANNEL_ARM];
    armed = rxArm >= SWITCH_ON;

#ifdef DEBUG
    Serial.printf("[ARM] VALUE=%d ARMED=%d\n", rxArm, armed);
#endif

#ifdef PAN_OUTPUT
    rxPan = map(channels[RX_CHANNEL_PAN], MIN_SBUS, MAX_SBUS, MIN_PULSEWIDTH, MAX_PULSEWIDTH);
    pan.write(map(rxPan, MIN_PULSEWIDTH, MAX_PULSEWIDTH, 0, 180));
#ifdef DEBUG
    Serial.printf("[PAN] VALUE=%d\n", rxPan);
#endif
#endif
  }

  if (armed)
  {
    flightModeFlags = 0x00000003;   // armed
    digitalWrite(LED_BUILTIN, LOW); // turn the LED on
  }
  else if (armed == 0)
  {
    flightModeFlags = 0x00000002;    // disarmed
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED off
  }

  if (millis() - update_t > 100)
  {
    // flight modes
    status_DJI.flightModeFlags = flightModeFlags;
    status_DJI.armingFlags = 0x0303;
    msp.send(MSP_STATUS_EX, &status_DJI, sizeof(status_DJI));
    status_DJI.armingFlags = 0x0000;
    msp.send(MSP_STATUS, &status_DJI, sizeof(status_DJI));

    // battery
#ifdef VBAT_OUTPUT
    vbat_sum += analogRead(A2) / VBAT_MODIFIER;
    vbat_count++;
    if (vbat_count >= VBAT_SAMPLE_SIZE)
    {
      vbat = vbat_sum / vbat_count;
      status_battery.voltage = constrain(vbat / 0.1, 0, 255);
      status_battery.extvolts = vbat;
#ifdef DEBUG
      Serial.printf("[VBAT] VBAT_RAW=%f, VBAT_MSP_VALUE=%d\n", vbat, status_battery.voltage);
#endif
      msp.send(MSP_BATTERY, &status_battery, sizeof(status_battery));
      vbat_count = 0;
      vbat_sum = 0;
#endif
    }
    update_t = millis();
  }
}
