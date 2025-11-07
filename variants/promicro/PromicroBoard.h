#pragma once

#include <MeshCore.h>
#include <Arduino.h>

#define P_LORA_NSS 13 //P1.13 45
#define P_LORA_DIO_1 11 //P0.10 10
#define P_LORA_RESET 10 //P0.09 9
#define P_LORA_BUSY  16 //P0.29 29
#define P_LORA_MISO  15 //P0.02 2
#define P_LORA_SCLK  12 //P1.11 43
#define P_LORA_MOSI  14 //P1.15 47
#define SX126X_POWER_EN 21 //P0.13 13
#define SX126X_RXEN 2 //P0.17
#define SX126X_TXEN RADIOLIB_NC
#define SX126X_DIO2_AS_RF_SWITCH  true
#define SX126X_DIO3_TCXO_VOLTAGE (1.8f)

#define PIN_VBAT_READ     17

// set the resolution of the ADC to 12 bits
// number of steps in a 12 bit sample is 2**12 (4096)
// the ADC reference voltage by default is 3.6V (nRF52 0.6V gain * 6)
// calculate millivolts per raw step:
//    convert volts to mV (V * 1000)
//    divide by number of steps = mV/step 
#define ADC_RES         12
#define ADC_STEPS       4096
#define ADC_REF_V       (3.6f)
#define ADC_MV_PER_STEP ((1000 * ADC_REF_V) / ADC_STEPS)

// dependent on voltage divider resistors.
// default to a 2:1 voltage divider (2.0) but allow overrides
// in platformio.ini

#ifndef ADC_MULTIPLIER
  #define ADC_MULTIPLIER (2.0f)
#endif

// scaled mV per step

#define MV_PER_STEP (ADC_MULTIPLIER * ADC_MV_PER_STEP)

class PromicroBoard : public mesh::MainBoard {
protected:
  uint8_t startup_reason;
  uint8_t btn_prev_state;

public:
  void begin();

  uint8_t getStartupReason() const override { return startup_reason; }

  #define BATTERY_SAMPLES 8

  uint16_t getBattMilliVolts() override {
    // be sure we're using the 3.6V reference 
    analogReference(AR_INTERNAL);
    analogReadResolution(ADC_RES);

    // yeet a sample, ADC isn't reliable right now after setting
    // reference voltage and resolution but will be after the next
    // sample

    analogRead(PIN_VBAT_READ);

    uint32_t raw = 0;
    for (int i = 0; i < BATTERY_SAMPLES; i++) {
      raw += analogRead(PIN_VBAT_READ);
    }

    // calculate the average here to avoid integer precision error
    return (MV_PER_STEP * raw / BATTERY_SAMPLES);
  }

  const char* getManufacturerName() const override {
    #ifdef VARIANT_NAME
      return VARIANT_NAME;
    #else
      return "ProMicro DIY";
    #endif
  }

  int buttonStateChanged() {
    #ifdef BUTTON_PIN
      uint8_t v = digitalRead(BUTTON_PIN);
      if (v != btn_prev_state) {
        btn_prev_state = v;
        return (v == LOW) ? 1 : -1;
      }
    #endif
      return 0;
  }

  void reboot() override {
    NVIC_SystemReset();
  }
  
  void powerOff() override {
    sd_power_system_off();
  }

  bool startOTAUpdate(const char* id, char reply[]) override;
};
