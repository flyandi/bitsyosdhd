/**
   ___  _ __           ____  _______ 
  / _ )(_) /____ __ __/ __ \/ __/ _ \
 / _  / / __(_-</ // / /_/ /\ \/ // /
/____/_/\__/___/\_, /\____/___/____/ HD 
               /___/             
 * 
 * This file is part of BitsyOSD-HD (github.com/flyandi/bitsyosdhd)
 **/


#ifndef _BO_MSP_
#define _BO_MSP_


#endif

// RX Definitions
#define RX_INPUT_SBUS 0
#define RX_INPUT_PPM 1


// SMAD21 Serial2
#define PIN_SERIAL2_TX (10ul)  // TX on A10
#define PIN_SERIAL2_RX (9ul)   // RX on A9
#define PAD_SERIAL2_TX (UART_TX_PAD_2)
#define PAD_SERIAL2_RX (SERCOM_RX_PAD_1)
Uart Serial2(&sercom0, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);

// Servo Stuff
#define MIN_PULSEWIDTH 1000
#define IDLE_PULSEWIDTH 1100
#define MAX_PULSEWIDTH 2000
#define MIN_SBUS 172
#define MAX_SBUS 1811


#define SWITCH_ON 1400
#define MSP_STATUS_EX 150
#define MSP_BATTERY 130
#define VBAT_MAX_PER_CELL 4.2
#define VBAT_MIN_PER_CELL 3.0


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


struct msp_battery_DJI_t {
  uint8_t cells;
  uint16_t capacity;
  uint8_t voltage;
  uint16_t mahdrawn;
  uint16_t amps;
  uint8_t state;
  uint16_t extvolts;
} __attribute__((packed));

void SERCOM0_Handler(void)
{
  Serial2.IrqHandler();
}