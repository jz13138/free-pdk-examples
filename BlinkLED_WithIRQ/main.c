/*
  BlinkLED_WithIRQ

  Turns an LED on for one second, then off for one second, repeatedly.
  Uses a timer interrupt for delays.
*/

#include <stdint.h>
#include <pdk/device.h>
#include "sysclock.h"
#include "millis.h"

// Note: millis.h uses timer16 (T16) interrupts for timing.

// LED is placed on PA4 (current sink configuration)
#define LED_PIN             4

#define turnLedOn()         PA &= ~(1 << LED_PIN)
#define turnLedOff()        PA |= (1 << LED_PIN)
#define toggleLed()         PA ^= (1 << LED_PIN)

uint32_t previousMillis;

void interrupt(void) __interrupt(0) {
  if (INTRQ & INTRQ_T16) {        // T16 interrupt request?
    INTRQ &= ~INTRQ_T16;          // Mark T16 interrupt request processed
    millis_irq_handler();
  }
}

// Main program.
void main() {
  // Initialize hardware
  PAC |= (1 << LED_PIN);          // Set LED_PIN as output (all pins are input by default)
  turnLedOff();

  millis_setup();

  INTRQ = 0;
  __engint();                     // Enable global interrupts

  // Main processing loop.
  while(1) {
    uint32_t currentMillis = millis();
    if (currentMillis - previousMillis >= 1000) {
      toggleLed();
      previousMillis += 1000;
    }
  }
}

// Startup code - Setup/calibrate system clock.
unsigned char _sdcc_external_startup(void) {
  // Modify F_CPU in the Makefile to change frequencies
  // ...or... Replace these with the more specific PDK_SET_SYSCLOCK(...) / EASY_PDK_CALIBRATE_IHRC(...) macros.
  // See pdk/sysclock.h for details.
  FREE_PDK_INIT_SYSCLOCK();
  FREE_PDK_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);
  return 0;
}