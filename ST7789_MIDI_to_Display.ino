//
// Version 1.2
//
#include <Arduino.h>
#include <i2c_driver.h>
#include <i2c_driver_wire.h>
#include <MIDI.h>
#include "Constants.h"
#include "MidiCC.h"
#include "Parameters.h"
#include "HWcontrols.h"
#include <Adafruit_GFX.h>  // Core graphics library
#include <ST7789_t3.h>     // Hardware-specific library
#include <SPI.h>
#include <ShiftRegister74HC595.h>

// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.

#define TFT_RST 28
#define TFT_RST1 41
#define TFT_RST2 40
#define TFT_RST_DUMMY 0xff

#define TFT_CS0 2
#define TFT_CS1 3
#define TFT_CS2 4
#define TFT_CS3 5
#define TFT_CS4 6
#define TFT_CS5 7
#define TFT_CS6 8
#define TFT_CS7 9
#define TFT_CS8 23

#define TFT_DC0 19
#define TFT_DC1 18
#define TFT_DC2 17
#define TFT_DC3 16
#define TFT_DC4 15
#define TFT_DC5 14
#define TFT_DC6 20
#define TFT_DC7 21
#define TFT_DC8 22

// For 1.14", 1.3", 1.54", 1.69", and 2.0" TFT with ST7789:
ST7789_t3 tft0 = ST7789_t3(TFT_CS0, TFT_DC0, 11, 13, TFT_RST_DUMMY);
ST7789_t3 tft1 = ST7789_t3(TFT_CS1, TFT_DC1, 11, 13, TFT_RST_DUMMY);
ST7789_t3 tft2 = ST7789_t3(TFT_CS2, TFT_DC2, 11, 13, TFT_RST_DUMMY);
ST7789_t3 tft3 = ST7789_t3(TFT_CS3, TFT_DC3, 11, 13, TFT_RST_DUMMY);
ST7789_t3 tft4 = ST7789_t3(TFT_CS4, TFT_DC4, 26, 27, TFT_RST_DUMMY);
ST7789_t3 tft5 = ST7789_t3(TFT_CS5, TFT_DC5, 26, 27, TFT_RST_DUMMY);
ST7789_t3 tft6 = ST7789_t3(TFT_CS6, TFT_DC6, 26, 27, TFT_RST_DUMMY);
ST7789_t3 tft7 = ST7789_t3(TFT_CS7, TFT_DC7, 26, 27, TFT_RST_DUMMY);
ST7789_t3 tft8 = ST7789_t3(TFT_CS8, TFT_DC8, 26, 27, TFT_RST_DUMMY);


#define PARAMETER 0      //The main page for displaying the current patch and control (parameter) changes
#define RECALL 1         //Patches list
#define SAVE 2           //Save patch page
#define REINITIALISE 3   // Reinitialise message
#define PATCH 4          // Show current patch bypassing PARAMETER
#define PATCHNAMING 5    // Patch naming page
#define DELETE 6         //Delete patch page
#define DELETEMSG 7      //Delete patch message page
#define SETTINGS 8       //Settings page
#define SETTINGSVALUE 9  //Settings page

unsigned int state = PARAMETER;

//MIDI 5 Pin DIN
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

ShiftRegister74HC595<3> srp(35, 34, 33);

#include "ST7789Display.h"

EXTMEM uint16_t buf[4 * 1024 * 1024] __attribute__((aligned(32)));

// Buffer size for each TFT device (example)
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define BUFFER_SIZE (TFT_WIDTH * TFT_HEIGHT * 2)

// Calculate buffer offsets for each TFT device
#define BUFFER0_OFFSET 0
#define BUFFER1_OFFSET (BUFFER0_OFFSET + BUFFER_SIZE)
#define BUFFER2_OFFSET (BUFFER1_OFFSET + BUFFER_SIZE)
#define BUFFER3_OFFSET (BUFFER2_OFFSET + BUFFER_SIZE)
#define BUFFER4_OFFSET (BUFFER3_OFFSET + BUFFER_SIZE)
#define BUFFER5_OFFSET (BUFFER4_OFFSET + BUFFER_SIZE)
#define BUFFER6_OFFSET (BUFFER5_OFFSET + BUFFER_SIZE)
#define BUFFER7_OFFSET (BUFFER6_OFFSET + BUFFER_SIZE)
#define BUFFER8_OFFSET (BUFFER7_OFFSET + BUFFER_SIZE)

int led = 54;

void setup() {

  // pinMode(led, OUTPUT);
  Wire2.begin(8);                 // join i2c bus with address #8
  Wire2.setClock(400000);         // Set I2C speed to 400 kHz
  Wire2.onReceive(receiveEvent);  // register event

  Serial.begin(115200);

  SPI.begin();

  //srp.begin(LED_DATA, LED_LATCH, LED_CLK, LED_PWM);

  /* reset the displays manually */
  pinMode(TFT_RST, OUTPUT);
  pinMode(TFT_RST1, OUTPUT);
  pinMode(TFT_RST2, OUTPUT);

  digitalWriteFast(TFT_RST, HIGH);
  digitalWriteFast(TFT_RST1, HIGH);
  digitalWriteFast(TFT_RST2, HIGH);
  delay(200);
  digitalWriteFast(TFT_RST, LOW);
  digitalWriteFast(TFT_RST1, LOW);
  digitalWriteFast(TFT_RST2, LOW);
  delay(100);
  digitalWriteFast(TFT_RST, HIGH);
  digitalWriteFast(TFT_RST1, HIGH);
  digitalWriteFast(TFT_RST2, HIGH);
  delay(500);


  /* initializer using a 2.0" 320x240 tft */

  tft0.init(240, 320, SPI_MODE3);
  tft0.setFrameBuffer(&buf[BUFFER0_OFFSET]);
  tft0.useFrameBuffer(true);
  tft0.setRotation(3);
  tft0.invertDisplay(true);

  tft1.init(240, 320, SPI_MODE3);
  tft1.setFrameBuffer(&buf[BUFFER1_OFFSET]);
  tft1.useFrameBuffer(true);
  tft1.setRotation(3);
  tft1.invertDisplay(true);

  tft2.init(240, 320, SPI_MODE3);
  tft2.setFrameBuffer(&buf[BUFFER2_OFFSET]);
  tft2.useFrameBuffer(true);
  tft2.setRotation(3);
  tft2.invertDisplay(true);

  tft3.init(240, 320, SPI_MODE3);
  tft3.setFrameBuffer(&buf[BUFFER3_OFFSET]);
  tft3.useFrameBuffer(true);
  tft3.setRotation(3);
  tft3.invertDisplay(true);

  tft4.init(240, 320, SPI_MODE3);
  tft4.setFrameBuffer(&buf[BUFFER4_OFFSET]);
  tft4.useFrameBuffer(true);
  tft4.setRotation(3);
  tft4.invertDisplay(true);

  tft5.init(240, 320, SPI_MODE3);
  tft5.setFrameBuffer(&buf[BUFFER5_OFFSET]);
  tft5.useFrameBuffer(true);
  tft5.setRotation(3);
  tft5.invertDisplay(true);

  tft6.init(240, 320, SPI_MODE3);
  tft6.setFrameBuffer(&buf[BUFFER6_OFFSET]);
  tft6.useFrameBuffer(true);
  tft6.setRotation(3);
  tft6.invertDisplay(true);

  tft7.init(240, 320, SPI_MODE3);
  tft7.setFrameBuffer(&buf[BUFFER7_OFFSET]);
  tft7.useFrameBuffer(true);
  tft7.setRotation(3);
  tft7.invertDisplay(true);

  tft8.init(240, 320, SPI_MODE3);
  tft8.setFrameBuffer(&buf[BUFFER8_OFFSET]);
  tft8.useFrameBuffer(true);
  tft8.setRotation(3);
  tft8.invertDisplay(true);

#ifdef USE_FRAME_BUFFER
  if (!tft0.useFrameBuffer(true)) {  // lets try using a frame buffer.
    Serial.println("TFT0 failed to create Frame buffer");
  }
#endif
#ifdef USE_FRAME_BUFFER
  if (!tft1.useFrameBuffer(true)) {  // lets try using a frame buffer.
    Serial.println("TFT1 failed to create Frame buffer");
  }
#endif
#ifdef USE_FRAME_BUFFER
  if (!tft2.useFrameBuffer(true)) {  // lets try using a frame buffer.
    Serial.println("TFT2 failed to create Frame buffer");
  }
#endif
#ifdef USE_FRAME_BUFFER
  if (!tft3.useFrameBuffer(true)) {  // lets try using a frame buffer.
    Serial.println("TFT3 failed to create Frame buffer");
  }
#endif

#ifdef USE_FRAME_BUFFER
  if (!tft4.useFrameBuffer(true)) {  // lets try using a frame buffer.
    Serial.println("TFT4 failed to create Frame buffer");
  }
#endif
#ifdef USE_FRAME_BUFFER
  if (!tft5.useFrameBuffer(true)) {  // lets try using a frame buffer.
    Serial.println("TFT5 failed to create Frame buffer");
  }
#endif
#ifdef USE_FRAME_BUFFER
  if (!tft6.useFrameBuffer(true)) {  // lets try using a frame buffer.
    Serial.println("TFT6 failed to create Frame buffer");
  }
#endif
#ifdef USE_FRAME_BUFFER
  if (!tft7.useFrameBuffer(true)) {  // lets try using a frame buffer.
    Serial.println("TFT7 failed to create Frame buffer");
  }
#endif
#ifdef USE_FRAME_BUFFER
  if (!tft8.useFrameBuffer(true)) {  // lets try using a frame buffer.
    Serial.println("TFT8 failed to create Frame buffer");
  }
#endif

  delay(100);

  tft0.fillScreen(ST77XX_BLACK);
  tft0.updateScreen();
  tft1.fillScreen(ST77XX_BLACK);
  tft1.updateScreen();
  tft2.fillScreen(ST77XX_BLACK);
  tft2.updateScreen();
  tft3.fillScreen(ST77XX_BLACK);
  tft3.updateScreen();
  tft4.fillScreen(ST77XX_BLACK);
  tft4.updateScreen();
  tft5.fillScreen(ST77XX_BLACK);
  tft5.updateScreen();
  tft6.fillScreen(ST77XX_BLACK);
  tft6.updateScreen();
  tft7.fillScreen(ST77XX_BLACK);
  tft7.updateScreen();
  tft8.fillScreen(ST77XX_BLACK);
  tft8.updateScreen();

  setupDisplay();

  MIDI.begin();
  MIDI.setHandleControlChange(myConvertControlChange);
  MIDI.setHandleSystemExclusive(onSysExMessage);
  MIDI.turnThruOn(midi::Thru::Mode::Off);
  Serial.println("MIDI In DIN Listening");

  // usbMIDI.setHandleControlChange(myConvertControlChange);
  // usbMIDI.setHandleSystemExclusive(onSysExMessage);
  // Serial.println("MIDI In USB Listening");

  lowerSW = 1;
  srp.set(UPPER_LED, LOW);
  srp.set(LOWER_LED, HIGH);
  playMode = 0;
  srp.set(KEYBOARD_RED_LED, LOW);
  srp.set(KEYBOARD_GREEN_LED, LOW);


  renderCurrentPatchPage(0);
  tft0.updateScreen();
  renderCurrentPatchPage(1);
  tft1.updateScreen();
  renderCurrentPatchPage(2);
  tft2.updateScreen();
  renderCurrentPatchPage(3);
  tft3.updateScreen();
  renderCurrentPatchPage(4);
  tft4.updateScreen();
  renderCurrentPatchPage(5);
  tft5.updateScreen();
  renderCurrentPatchPage(6);
  tft6.updateScreen();
  renderCurrentPatchPage(7);
  tft7.updateScreen();
  renderCurrentPatchPage(8);
  tft8.updateScreen();
}

void receiveEvent(int howMany) {
  static int i2cByteNumber = 0;  // Keep track of where to store incoming data

  while (Wire2.available() >= 2 && i2cByteNumber < 76) {  // Ensure at least two bytes are available and we have space
    uint8_t highByte = Wire2.read();                      // Read the high byte
    uint8_t lowByte = Wire2.read();                       // Read the low byte

    panelData[i2cByteNumber] = (highByte << 8) | lowByte;  // Reassemble 16-bit integer
    i2cByteNumber++;
  }

  // Check if all expected data has been received
  if (i2cByteNumber >= 76) {

    if (panelData[P_sysex]) {
      memcpy(upperData, panelData, sizeof(panelData));
    } else {
      memcpy(lowerData, panelData, sizeof(panelData));
    }

    processSysExData();
    i2cByteNumber = 0;  // Reset index for next transmission
  }
}



void myConvertControlChange(byte channel, byte number, byte value) {

  if (channel == 1) {
    Serial.println("Received on channel 1 at the display");
    int newvalue = (value << 3);
    myControlChange(channel, number, newvalue);
  }

  if (channel == 2) {
    Serial.println("Received on channel 2 at the display");
    int newvalue = value;
    myLEDupdate(channel, number, newvalue);
  }
}

void myLEDupdate(byte channel, byte control, int value) {
  switch (control) {

    case CCLFOWaveform:
      panelData[P_LFOWaveform] = value;
      switch (panelData[P_lfoAlt]) {
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(lfo02[panelData[P_LFOWaveform]])));
          break;
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(lfo01[panelData[P_LFOWaveform]])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(lfoDisplay, str_ptr);
      }
      tft6.setFont(&FreeSans12pt7b);

      // Set range label and value inside a box along the top
      tft6.fillRoundRect(10, 10, 200, 30, 5, ST7735_YELLOW);  // Background box for range
      tft6.setCursor(40, 18);
      tft6.setTextColor(ST7735_BLACK);
      tft6.print(lfoDisplay);
      tft6.updateScreen();
      break;

    case CCupperSW:
      upperSW = value;
      if (upperSW) {
        srp.set(UPPER_LED, HIGH);
        srp.set(LOWER_LED, LOW);
        lowerSW = 0;
      }
      // upper/lower indicator box along the top
      tft0.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft0.setFont(&FreeSans12pt7b);
      if (upperSW == 0) {
        tft0.fillRoundRect(180, 50, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft0.fillRoundRect(180, 50, 130, 30, 5, ST7735_RED);  // Green box for off
      }
      tft0.setCursor(195, 58);
      tft0.print(upperSW == 0 ? "     " : "Upper");

      if (lowerSW == 0) {
        tft0.fillRoundRect(180, 90, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft0.fillRoundRect(180, 90, 130, 30, 5, ST7735_RED);  // Green box for off
      }
      tft0.setCursor(195, 98);
      tft0.print(lowerSW == 0 ? "     " : "Lower");
      //tft0.updateScreen();
      memcpy(panelData, upperData, 71);
      processSysExData();
      break;

    case CClowerSW:
      lowerSW = value;
      if (lowerSW) {
        srp.set(UPPER_LED, LOW);
        srp.set(LOWER_LED, HIGH);
        upperSW = 0;
      }
      // upper/lower indicator box along the top
      tft0.setFont(&FreeSans12pt7b);
      if (upperSW == 0) {
        tft0.fillRoundRect(180, 50, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft0.fillRoundRect(180, 50, 130, 30, 5, ST7735_RED);  // Green box for off
      }
      tft0.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft0.setCursor(195, 58);
      tft0.print(upperSW == 0 ? "     " : "Upper");

      if (lowerSW == 0) {
        tft0.fillRoundRect(180, 90, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft0.fillRoundRect(180, 90, 130, 30, 5, ST7735_RED);  // Green box for off
      }
      tft0.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft0.setCursor(195, 98);
      tft0.print(lowerSW == 0 ? "     " : "Lower");
      memcpy(panelData, lowerData, 71);
      processSysExData();
      break;

    case CCglideSW:
      if (value == 1) {
        srp.set(GLIDE_SW_LED, HIGH);
        panelData[P_glideSW] = 1;
      } else if (value == 0) {
        srp.set(GLIDE_SW_LED, LOW);
        panelData[P_glideSW] = 0;
      }
      // if (upperSW) {
      //   upperData[P_glideSW] = panelData[P_glideSW];
      // } else {
      //   lowerData[P_glideSW] = panelData[P_glideSW];
      // }
      tft0.setFont(&FreeSans12pt7b);
      if (panelData[P_glideSW] == 0) {
        tft0.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft0.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft0.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft0.setCursor(30, 18);
      tft0.print(panelData[P_glideSW] == 0 ? "Glide Off" : "Glide On");
      tft0.updateScreen();
      break;

    case CCchordHoldSW:
      if (value == 127) {
        srp.set(CHORD_HOLD_LED, HIGH);
        chordHoldSW = 1;
      } else if (value == 0) {
        srp.set(CHORD_HOLD_LED, LOW);
        chordHoldSW = 0;
      }
      tft0.setFont(&FreeSans12pt7b);
      if (chordHoldSW == 0) {
        tft0.fillRoundRect(180, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft0.fillRoundRect(180, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft0.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft0.setCursor(195, 18);
      tft0.print(chordHoldSW == 0 ? "Hold Off" : "Hold On");
      tft0.updateScreen();
      break;

    case CCfilterPoleSW:
      if (value == 127) {
        srp.set(FILTER_POLE_LED, HIGH);
        panelData[P_filterPoleSW] = 1;
        tft3.fillRoundRect(240, 10, 70, 30, 5, ST7735_RED);
      } else if (value == 0) {
        srp.set(FILTER_POLE_LED, LOW);
        panelData[P_filterPoleSW] = 0;
        tft3.fillRoundRect(240, 10, 70, 30, 5, ST7735_GREEN);  // Green box for off
      }
      tft3.setFont(&FreeSans12pt7b);
      switch (panelData[P_filterPoleSW]) {
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(filter01[panelData[P_filterType]])));
          break;
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(filter02[panelData[P_filterType]])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(filterDisplay, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }
      tft3.setFont(&FreeSans12pt7b);

      tft3.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft3.setCursor(260, 18);
      tft3.print(panelData[P_filterPoleSW] == 0 ? "Off" : "On");

      // Set range label and value inside a box along the top
      tft3.fillRoundRect(10, 10, 200, 30, 5, ST7735_YELLOW);  // Background box for range
      tft3.setCursor(40, 18);
      tft3.setTextColor(ST7735_BLACK);
      tft3.print(filterDisplay);
      tft3.updateScreen();
      break;

    case CCsyncSW:
      panelData[P_sync] = value;
      if (panelData[P_sync]) {
        srp.set(SYNC_LED, HIGH);
      } else if (!panelData[P_sync]) {
        srp.set(SYNC_LED, LOW);
      }
      if (panelData[P_sync]) {
        tft2.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);
      } else {
        tft2.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);
      }
      tft2.setCursor(30, 18);
      tft2.print(panelData[P_sync] == 0 ? "Sync Off" : "Sync On");
      tft2.updateScreen();
      break;

    case CCpmDestDCO1SW:
      panelData[P_pmDestDCO1] = value;
      if (panelData[P_pmDestDCO1]) {
        srp.set(PM_DCO1_DEST_LED, HIGH);
      } else if (!panelData[P_pmDestDCO1]) {
        srp.set(PM_DCO1_DEST_LED, LOW);
      }
      if (panelData[P_pmDestDCO1]) {
        tft8.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);
      } else {
        tft8.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);
      }
      tft8.setCursor(20, 18);
      tft8.print(panelData[P_pmDestDCO1] == 0 ? "DCO1 Off" : "DCO1 On");
      tft8.updateScreen();
      break;

    case CCpmDestFilterSW:
      panelData[P_pmDestFilter] = value;
      if (panelData[P_pmDestFilter]) {
        srp.set(PM_FILT_ENV_DEST_LED, HIGH);
      } else if (!panelData[P_pmDestFilter]) {
        srp.set(PM_FILT_ENV_DEST_LED, LOW);
      }
      if (panelData[P_pmDestFilter]) {
        tft8.fillRoundRect(180, 10, 130, 30, 5, ST7735_RED);
      } else {
        tft8.fillRoundRect(180, 10, 130, 30, 5, ST7735_GREEN);
      }
      tft8.setCursor(200, 18);
      tft8.print(panelData[P_pmDestFilter] == 0 ? "Filter Off" : "Filter On");
      tft8.updateScreen();
      break;

    case CClfoAlt:
      panelData[P_lfoAlt] = value;
      if (panelData[P_lfoAlt]) {
        srp.set(LFO_ALT_LED, HIGH);
      } else if (!panelData[P_lfoAlt]) {
        srp.set(LFO_ALT_LED, LOW);
      }
      if (panelData[P_lfoAlt]) {
        tft6.fillRoundRect(180, 50, 130, 30, 5, ST7735_RED);
      } else {
        tft6.fillRoundRect(180, 50, 130, 30, 5, ST7735_GREEN);
      }
      tft6.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft6.setCursor(200, 58);
      tft6.print(panelData[P_lfoAlt] == 0 ? "Alt Off" : "Alt On");
      tft6.updateScreen();
      break;

    case CCfilterType:
      panelData[P_filterType] = value;
      switch (panelData[P_filterPoleSW]) {
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(filter01[panelData[P_filterType]])));
          break;
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(filter02[panelData[P_filterType]])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(filterDisplay, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }
      tft3.setFont(&FreeSans12pt7b);
      if (panelData[P_filterPoleSW] == 0) {
        tft3.fillRoundRect(240, 10, 70, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft3.fillRoundRect(240, 10, 70, 30, 5, ST7735_RED);  // Red box for on
      }
      tft3.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft3.setCursor(260, 18);
      tft3.print(panelData[P_filterPoleSW] == 0 ? "Off" : "On");

      // Set range label and value inside a box along the top
      tft3.fillRoundRect(10, 10, 200, 30, 5, ST7735_YELLOW);  // Background box for range
      tft3.setCursor(40, 18);
      tft3.setTextColor(ST7735_BLACK);
      tft3.print(filterDisplay);
      tft3.updateScreen();
      break;

    case CCosc1Oct:
      panelData[P_osc1Range] = value;
      tft1.setFont(&FreeSans12pt7b);
      // Set range label and value inside a box along the top
      tft1.fillRoundRect(180, 10, 130, 30, 5, ST7735_YELLOW);  // Background box for range
      tft1.setTextColor(ST7735_BLACK);
      tft1.setCursor(195, 18);
      switch (panelData[P_osc1Range]) {
        case 0:
          tft1.print("Range 32");
          break;
        case 1:
          tft1.print("Range 16");
          break;
        case 2:
          tft1.print("Range 8");
          break;
      }
      tft1.updateScreen();
      break;

    case CCosc2Oct:
      panelData[P_osc2Range] = value;
      tft2.setFont(&FreeSans12pt7b);
      // Set range label and value inside a box along the top
      tft2.fillRoundRect(180, 10, 130, 30, 5, ST7735_YELLOW);  // Background box for range
      tft2.setTextColor(ST7735_BLACK);
      tft2.setCursor(195, 18);
      switch (panelData[P_osc2Range]) {
        case 0:
          tft2.print("Range 32");
          break;
        case 1:
          tft2.print("Range 16");
          break;
        case 2:
          tft2.print("Range 8");
          break;
      }
      tft2.updateScreen();
      break;

    case CCNotePriority:
      panelData[P_NotePriority] = value;
      tft0.fillRoundRect(50, 130, 120, 30, 5, ST7735_CYAN);  // Cyan box
      tft0.setCursor(60, 138);
      tft0.print("Priority");
      tft0.fillRoundRect(180, 130, 130, 30, 5, ST7735_YELLOW);  // Yellow box
      switch (panelData[P_NotePriority]) {
        case 0:
          srp.set(PRIORITY_RED_LED, HIGH);
          srp.set(PRIORITY_GREEN_LED, LOW);
          tft0.setCursor(195, 138);
          tft0.print("Top");
          break;

        case 1:
          srp.set(PRIORITY_RED_LED, LOW);
          srp.set(PRIORITY_GREEN_LED, HIGH);
          tft0.setCursor(195, 138);
          tft0.print("Bottom");
          break;

        case 2:
          srp.set(PRIORITY_RED_LED, HIGH);
          srp.set(PRIORITY_GREEN_LED, HIGH);
          tft0.setCursor(195, 138);
          tft0.print("Last");
          break;
      }
      tft0.updateScreen();
      break;

    case CCplayMode:
      playMode = value;
      tft0.fillRoundRect(50, 170, 120, 30, 5, ST7735_CYAN);  // Cyan box
      tft0.setCursor(60, 178);
      tft0.print("Keyboard");
      tft0.fillRoundRect(180, 170, 130, 30, 5, ST7735_YELLOW);  // Yellow box
      switch (playMode) {
        case 0:
          tft0.setCursor(195, 178);
          tft0.print("Whole");
          srp.set(KEYBOARD_RED_LED, LOW);
          srp.set(KEYBOARD_GREEN_LED, LOW);
          break;

        case 1:
          tft0.setCursor(195, 178);
          tft0.print("Dual");
          srp.set(KEYBOARD_RED_LED, LOW);
          srp.set(KEYBOARD_GREEN_LED, HIGH);
          break;

        case 2:
          tft0.setCursor(195, 178);
          tft0.print("Split");
          srp.set(KEYBOARD_RED_LED, HIGH);
          srp.set(KEYBOARD_GREEN_LED, LOW);
          break;
      }
      tft0.updateScreen();
      break;

    case CClfoMult:
      panelData[P_lfoMultiplier] = value;
      tft6.fillRoundRect(180, 130, 130, 30, 5, ST7735_YELLOW);  // Background box for range
      tft6.setCursor(200, 138);
      tft6.print("Mult");
      tft6.setCursor(260, 138);
      switch (panelData[P_lfoMultiplier]) {
        case 0:
          tft6.print("x0.5");
          break;
        case 1:
          tft6.print("x1.0");
          break;
        case 2:
          tft6.print("x1.5");
          break;
        case 3:
          tft6.print("x2.0");
          break;
        case 4:
          tft6.print("x2.5");
          break;
      }
      tft6.updateScreen();
      break;

    case CCeffectNumSW:
      panelData[P_effectNum] = value;
      switch (panelData[P_effectBank]) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name01[panelData[P_effectNum]])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name11[panelData[P_effectNum]])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name21[panelData[P_effectNum]])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name31[panelData[P_effectNum]])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf1, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }

      switch (panelData[P_effectBank]) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name02[panelData[P_effectNum]])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name12[panelData[P_effectNum]])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name22[panelData[P_effectNum]])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name32[panelData[P_effectNum]])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf2, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }

      switch (panelData[P_effectBank]) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name03[panelData[P_effectNum]])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name13[panelData[P_effectNum]])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name23[panelData[P_effectNum]])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name33[panelData[P_effectNum]])));
          break;
      }

      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf3, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }

      switch (panelData[P_effectBank]) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name04[panelData[P_effectNum]])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name14[panelData[P_effectNum]])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name24[panelData[P_effectNum]])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name34[panelData[P_effectNum]])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf4, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }

      switch (panelData[P_effectBank]) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name05[panelData[P_effectNum]])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name15[panelData[P_effectNum]])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name25[panelData[P_effectNum]])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name35[panelData[P_effectNum]])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf5, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }

      tft7.fillRoundRect(10, 10, 300, 30, 5, ST7735_YELLOW);
      tft7.setCursor(50, 18);
      tft7.print(buf1);
      tft7.setCursor(180, 18);
      tft7.print(buf2);


      tft7.fillRoundRect(180, 50, 130, 30, 5, ST7735_YELLOW);  // Green box for off
      tft7.setCursor(200, 58);
      tft7.print("Bank:");
      tft7.setCursor(280, 58);
      tft7.print(panelData[P_effectBank] + 1);

      tft7.fillRoundRect(180, 90, 130, 30, 5, ST7735_YELLOW);  // Green box for off
      tft7.setCursor(200, 98);
      tft7.print("Prog:");
      tft7.setCursor(280, 98);
      tft7.print(panelData[P_effectNum] + 1);

      tft7.setFont(&FreeSans9pt7b);

      tft7.fillRoundRect(180, 130, 130, 30, 5, ST7735_CYAN);  // Green box for off
      // tft7.setCursor(200, 138);
      // tft7.print("P1");
      tft7.setCursor(190, 138);
      tft7.print(buf3);

      tft7.fillRoundRect(180, 170, 130, 30, 5, ST7735_CYAN);  // Green box for off
      // tft7.setCursor(200, 178);
      // tft7.print("P2");
      tft7.setCursor(190, 178);
      tft7.print(buf4);

      tft7.fillRoundRect(180, 210, 130, 30, 5, ST7735_CYAN);  // Green box for off
      // tft7.setCursor(200, 218);
      // tft7.print("P3");
      tft7.setCursor(190, 218);
      tft7.print(buf5);

      tft7.setFont(&FreeSans12pt7b);

      tft7.updateScreen();
      break;

    case CCeffectBankSW:
      panelData[P_effectBank] = value;

      switch (panelData[P_effectBank]) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name01[panelData[P_effectNum]])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name11[panelData[P_effectNum]])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name21[panelData[P_effectNum]])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name31[panelData[P_effectNum]])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf1, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }

      switch (panelData[P_effectBank]) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name02[panelData[P_effectNum]])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name12[panelData[P_effectNum]])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name22[panelData[P_effectNum]])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name32[panelData[P_effectNum]])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf2, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }

      switch (panelData[P_effectBank]) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name03[panelData[P_effectNum]])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name13[panelData[P_effectNum]])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name23[panelData[P_effectNum]])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name33[panelData[P_effectNum]])));
          break;
      }

      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf3, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }

      switch (panelData[P_effectBank]) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name04[panelData[P_effectNum]])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name14[panelData[P_effectNum]])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name24[panelData[P_effectNum]])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name34[panelData[P_effectNum]])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf4, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }

      switch (panelData[P_effectBank]) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name05[panelData[P_effectNum]])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name15[panelData[P_effectNum]])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name25[panelData[P_effectNum]])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name35[panelData[P_effectNum]])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf5, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }

      tft7.fillRoundRect(10, 10, 300, 30, 5, ST7735_YELLOW);
      tft7.setCursor(50, 18);
      tft7.print(buf1);
      tft7.setCursor(180, 18);
      tft7.print(buf2);

      tft7.fillRoundRect(180, 50, 130, 30, 5, ST7735_YELLOW);  // Green box for off
      tft7.setCursor(200, 58);
      tft7.print("Bank:");
      tft7.setCursor(280, 58);
      tft7.print(panelData[P_effectBank] + 1);

      tft7.fillRoundRect(180, 90, 130, 30, 5, ST7735_YELLOW);  // Green box for off
      tft7.setCursor(200, 98);
      tft7.print("Prog:");
      tft7.setCursor(280, 98);
      tft7.print(panelData[P_effectNum] + 1);

      tft7.setFont(&FreeSans9pt7b);

      tft7.fillRoundRect(180, 130, 130, 30, 5, ST7735_CYAN);  // Green box for off
      // tft7.setCursor(200, 138);
      // tft7.print("P1");
      tft7.setCursor(190, 138);
      tft7.print(buf3);

      tft7.fillRoundRect(180, 170, 130, 30, 5, ST7735_CYAN);  // Green box for off
      // tft7.setCursor(200, 178);
      // tft7.print("P2");
      tft7.setCursor(190, 178);
      tft7.print(buf4);

      tft7.fillRoundRect(180, 210, 130, 30, 5, ST7735_CYAN);  // Green box for off
      // tft7.setCursor(200, 218);
      // tft7.print("P3");
      tft7.setCursor(190, 218);
      tft7.print(buf5);

      tft7.setFont(&FreeSans12pt7b);

      tft7.updateScreen();

      break;

    case CCkeyboardMode:
      panelData[P_keyboardMode] = value;
      tft0.fillRoundRect(50, 210, 120, 30, 5, ST7735_CYAN);  // Cyan box
      tft0.setCursor(60, 218);
      tft0.print("Key Mode");
      tft0.fillRoundRect(180, 210, 130, 30, 5, ST7735_YELLOW);  // Yellow box
      switch (panelData[P_keyboardMode]) {
        case 0:
          tft0.setCursor(195, 218);
          tft0.print("Poly 1");
          srp.set(POLY1_LED, HIGH);
          srp.set(POLY2_LED, LOW);
          srp.set(UNISON_LED, LOW);
          srp.set(MONO_LED, LOW);
          break;

        case 1:
          tft0.setCursor(195, 218);
          tft0.print("Poly 2");
          srp.set(POLY1_LED, LOW);
          srp.set(POLY2_LED, HIGH);
          srp.set(UNISON_LED, LOW);
          srp.set(MONO_LED, LOW);
          break;

        case 2:
          tft0.setCursor(195, 218);
          tft0.print("Unison");
          srp.set(POLY1_LED, LOW);
          srp.set(POLY2_LED, LOW);
          srp.set(UNISON_LED, HIGH);
          srp.set(MONO_LED, LOW);
          break;

        case 3:
          tft0.setCursor(195, 218);
          tft0.print("Mono");
          srp.set(POLY1_LED, LOW);
          srp.set(POLY2_LED, LOW);
          srp.set(UNISON_LED, LOW);
          srp.set(MONO_LED, HIGH);
          break;
      }
      tft0.updateScreen();
      break;

    case CCmonoMulti:
      panelData[P_monoMulti] = value;
      if (panelData[P_monoMulti]) {
        srp.set(LFO_MULTI_MONO_LED, HIGH);
        tft6.fillRoundRect(180, 90, 130, 30, 5, ST7735_RED);
      } else {
        srp.set(LFO_MULTI_MONO_LED, LOW);
        tft6.fillRoundRect(180, 90, 130, 30, 5, ST7735_GREEN);
      }
      tft6.setCursor(200, 98);
      tft6.print(panelData[P_monoMulti] == 0 ? "Trig Off" : "Trig On");
      tft6.updateScreen();
      break;

    case CCfilterEGinv:
      if (value == 127) {
        srp.set(EG_INVERT_LED, HIGH);
        panelData[P_filterEGinv] = 1;
      } else if (value == 0) {
        srp.set(EG_INVERT_LED, LOW);
        panelData[P_filterEGinv] = 0;
      }
      if (panelData[P_filterEGinv] == 0) {
        tft4.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft4.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft4.setCursor(30, 18);
      tft4.print(panelData[P_filterEGinv] == 0 ? "EG Pos" : "Eg Neg");
      tft4.updateScreen();
      break;

    case CCfilterVel:
      panelData[P_filterVel] = value;
      if (panelData[P_filterVel] == 0) {
        tft4.fillRoundRect(180, 170, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft4.fillRoundRect(180, 170, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft4.setCursor(210, 178);
      tft4.print(panelData[P_filterVel] == 0 ? "Vel Off" : "Vel On");
      tft4.updateScreen();
      break;

    case CCvcaVel:
      panelData[P_vcaVel] = value;
      if (!panelData[P_vcaVel]) {
        tft5.fillRoundRect(180, 170, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft5.fillRoundRect(180, 170, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft5.setCursor(210, 178);
      tft5.print(panelData[P_vcaVel] == 0 ? "Vel Off" : "Vel On");
      tft5.updateScreen();
      break;

    case CCfilterenvLinLogSW:
      panelData[P_filterLogLin] = value;
      if (!panelData[P_filterLogLin]) {
        tft4.fillRoundRect(180, 90, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft4.fillRoundRect(180, 90, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft4.setCursor(210, 98);
      tft4.print(panelData[P_filterLogLin] == 0 ? "Env Lin" : "Env Log");
      tft4.updateScreen();
      break;

    case CCampenvLinLogSW:
      panelData[P_ampLogLin] = value;
      if (!panelData[P_ampLogLin]) {
        tft5.fillRoundRect(180, 90, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft5.fillRoundRect(180, 90, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft5.setCursor(210, 98);
      tft5.print(panelData[P_ampLogLin] == 0 ? "Env Lin" : "Env Log");
      tft5.updateScreen();
      break;

    case CCvcaGate:
      panelData[P_vcaGate] = value;
      if (panelData[P_vcaGate] == 0) {
        srp.set(AMP_GATED_LED, LOW);
        panelData[P_ampAttack] = panelData[P_oldampAttack];
        panelData[P_ampDecay] = panelData[P_oldampDecay];
        panelData[P_ampSustain] = panelData[P_oldampSustain];
        panelData[P_ampRelease] = panelData[P_oldampRelease];
        tft5.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        srp.set(AMP_GATED_LED, HIGH);
        panelData[P_ampAttack] = 0;
        panelData[P_ampDecay] = 0;
        panelData[P_ampSustain] = 1023;
        panelData[P_ampRelease] = 0;
        tft5.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft5.setCursor(30, 18);
      tft5.print(panelData[P_vcaGate] == 0 ? "Gate Off" : "Gate On");

      drawBar5(6, panelData[P_ampAttack], NUM_STEPS, STEP_HEIGHT);
      drawBar5(52, panelData[P_ampDecay], NUM_STEPS, STEP_HEIGHT);
      drawBar5(98, panelData[P_ampSustain], NUM_STEPS, STEP_HEIGHT);
      drawBar5(144, panelData[P_ampRelease], NUM_STEPS, STEP_HEIGHT);

      tft5.updateScreen();
      break;

    case CCFilterLoop:
      panelData[P_filterLoop] = value;
      tft4.fillRoundRect(180, 10, 130, 30, 5, ST7735_YELLOW);  // Green box for off
      switch (panelData[P_filterLoop]) {
        case 0:
          tft4.setCursor(195, 18);
          tft4.print("Loop Off");
          break;
        case 1:
          tft4.setCursor(183, 18);
          tft4.print("Loop Gated");
          break;
        case 2:
          tft4.setCursor(195, 18);
          tft4.print("Loop LFO");
          break;
      }
      tft4.updateScreen();
      break;

    case CCAmpLoop:
      panelData[P_vcaLoop] = value;
      tft5.fillRoundRect(180, 10, 130, 30, 5, ST7735_YELLOW);  // Green box for off
      switch (panelData[P_vcaLoop]) {
        case 0:
          tft5.setCursor(195, 18);
          tft5.print("Loop Off");
          break;
        case 1:
          tft5.setCursor(183, 18);
          tft5.print("Loop Gated");
          break;
        case 2:
          tft5.setCursor(195, 18);
          tft5.print("Loop LFO");
          break;
      }
      tft5.updateScreen();
      break;
  }
}

void myControlChange(byte channel, byte control, int value) {
  switch (control) {

      // TFT 0

    case CCglideTime:
      panelData[P_glideTime] = value;
      drawBar0(6, panelData[P_glideTime], NUM_STEPS, STEP_HEIGHT);
      tft0.updateScreen();
      break;

      // TFT 1 oscillator 1

    case CCosc1PW:
      panelData[P_osc1PW] = value;
      Serial.println(panelData[P_osc1PW]);
      drawPWIndicator1(6, panelData[P_osc1PW]);
      tft1.updateScreen();
      break;

    case CCosc1PWM:
      panelData[P_osc1PWM] = value;
      drawBar1(52, panelData[P_osc1PWM], NUM_STEPS, STEP_HEIGHT);
      tft1.updateScreen();
      break;

    case CCosc1SawLevel:
      panelData[P_osc1SawLevel] = value;
      drawBar1(192, panelData[P_osc1SawLevel], NUM_STEPS, STEP_HEIGHT);
      tft1.updateScreen();
      break;

    case CCosc1PulseLevel:
      panelData[P_osc1PulseLevel] = value;
      drawBar1(236, panelData[P_osc1PulseLevel], NUM_STEPS, STEP_HEIGHT);
      tft1.updateScreen();
      break;

    case CCosc1SubLevel:
      panelData[P_osc1SubLevel] = value;
      drawBar1(282, panelData[P_osc1SubLevel], NUM_STEPS, STEP_HEIGHT);
      tft1.updateScreen();
      break;

    case CCoscfmDepth:
      panelData[P_fmDepth] = value;
      drawBar1(98, panelData[P_fmDepth], NUM_STEPS, STEP_HEIGHT);
      tft1.updateScreen();
      break;

      // TFT 2 oscillator 2

    case CCosc2PW:
      panelData[P_osc2PW] = value;
      drawPWIndicator2(6, panelData[P_osc2PW]);
      tft2.updateScreen();
      break;

    case CCosc2PWM:
      panelData[P_osc2PWM] = value;
      drawBar2(52, panelData[P_osc2PWM], NUM_STEPS, STEP_HEIGHT);
      tft2.updateScreen();
      break;

    case CCosc2Detune:
      panelData[P_osc2Detune] = value;
      drawBar2(98, panelData[P_osc2Detune], NUM_STEPS, STEP_HEIGHT);
      tft2.updateScreen();
      break;

    case CCosc2Interval:
      panelData[P_osc2Interval] = value;
      drawBar2(144, panelData[P_osc2Interval], DET_STEPS, DET_STEP_HEIGHT);
      tft2.updateScreen();
      break;

    case CCosc2sawLevel:
      panelData[P_osc2SawLevel] = value;
      drawBar2(192, panelData[P_osc2SawLevel], NUM_STEPS, STEP_HEIGHT);
      tft2.updateScreen();
      break;

    case CCosc2pulseLevel:
      panelData[P_osc2PulseLevel] = value;
      drawBar2(236, panelData[P_osc2PulseLevel], NUM_STEPS, STEP_HEIGHT);
      tft2.updateScreen();
      break;

    case CCosc2triangleLevel:
      panelData[P_osc2TriangleLevel] = value;
      drawBar2(282, panelData[P_osc2TriangleLevel], NUM_STEPS, STEP_HEIGHT);
      tft2.updateScreen();
      break;

      // TFT 3

    case CCKeyTrack:
      panelData[P_keytrack] = value;
      drawBar3(144, panelData[P_keytrack], NUM_STEPS, STEP_HEIGHT);
      tft3.updateScreen();
      break;

    case CCfilterCutoff:
      panelData[P_filterCutoff] = value;
      drawBar3(6, panelData[P_filterCutoff], NUM_STEPS, STEP_HEIGHT);
      tft3.updateScreen();
      break;

    case CCfilterLFO:
      panelData[P_filterLFO] = value;
      drawBar3(192, panelData[P_filterLFO], NUM_STEPS, STEP_HEIGHT);
      tft3.updateScreen();
      break;

    case CCfilterRes:
      panelData[P_filterRes] = value;
      drawBar3(52, panelData[P_filterRes], NUM_STEPS, STEP_HEIGHT);
      tft3.updateScreen();
      break;

    case CCfilterEGlevel:
      panelData[P_filterEGlevel] = value;
      drawBar3(98, panelData[P_filterEGlevel], NUM_STEPS, STEP_HEIGHT);
      tft3.updateScreen();
      break;

      // TFT 4 Filter ADSR

    case CCfilterAttack:
      panelData[P_filterAttack] = value;
      drawBar4(6, panelData[P_filterAttack], NUM_STEPS, STEP_HEIGHT);
      tft4.updateScreen();
      break;

    case CCfilterDecay:
      panelData[P_filterDecay] = value;
      drawBar4(52, panelData[P_filterDecay], NUM_STEPS, STEP_HEIGHT);
      tft4.updateScreen();
      break;

    case CCfilterSustain:
      panelData[P_filterSustain] = value;
      drawBar4(98, panelData[P_filterSustain], NUM_STEPS, STEP_HEIGHT);
      tft4.updateScreen();
      break;

    case CCfilterRelease:
      panelData[P_filterRelease] = value;
      drawBar4(144, panelData[P_filterRelease], NUM_STEPS, STEP_HEIGHT);
      tft4.updateScreen();
      break;

      // TFT 5 Amp ADSR

    case CCampAttack:
      panelData[P_ampAttack] = value;
      panelData[P_oldampAttack] = value;
      drawBar5(6, panelData[P_ampAttack], NUM_STEPS, STEP_HEIGHT);
      tft5.updateScreen();
      break;

    case CCampDecay:
      panelData[P_ampDecay] = value;
      panelData[P_oldampDecay] = value;
      drawBar5(52, panelData[P_ampDecay], NUM_STEPS, STEP_HEIGHT);
      tft5.updateScreen();
      break;

    case CCampSustain:
      panelData[P_ampSustain] = value;
      panelData[P_oldampSustain] = value;
      drawBar5(98, panelData[P_ampSustain], NUM_STEPS, STEP_HEIGHT);
      tft5.updateScreen();
      break;

    case CCampRelease:
      panelData[P_ampRelease] = value;
      panelData[P_oldampRelease] = value;
      drawBar5(144, panelData[P_ampRelease], NUM_STEPS, STEP_HEIGHT);
      tft5.updateScreen();
      break;

      // TFT 6 LFO

    case CCLFORate:
      panelData[P_LFORate] = value;
      drawBar6(6, panelData[P_LFORate], NUM_STEPS, STEP_HEIGHT);
      tft6.updateScreen();
      break;

    case CCLFODelay:
      panelData[P_LFODelay] = value;
      drawBar6(52, panelData[P_LFODelay], NUM_STEPS, STEP_HEIGHT);
      tft6.updateScreen();
      break;

    case CCpwLFO:
      panelData[P_pwLFO] = value;
      drawBar6(98, panelData[P_pwLFO], NUM_STEPS, STEP_HEIGHT);
      tft6.updateScreen();
      break;

    case CCmodWheelDepth:
      panelData[P_modWheelDepth] = value;
      drawBar6(144, panelData[P_modWheelDepth], NUM_STEPS, STEP_HEIGHT);
      tft6.updateScreen();
      break;

      // TFT 7 effects

    case CCeffect1:
      panelData[P_effectPot1] = value;
      drawBar7(6, panelData[P_effectPot1], NUM_STEPS, STEP_HEIGHT);
      tft7.updateScreen();
      break;

    case CCeffect2:
      panelData[P_effectPot2] = value;
      drawBar7(52, panelData[P_effectPot2], NUM_STEPS, STEP_HEIGHT);
      tft7.updateScreen();
      break;

    case CCeffect3:
      panelData[P_effectPot3] = value;
      drawBar7(98, panelData[P_effectPot3], NUM_STEPS, STEP_HEIGHT);
      tft7.updateScreen();
      break;

    case CCeffectMix:
      panelData[P_effectsMix] = map(value, 0, readRes, 0, readRes);
      drawPWIndicator7(144, panelData[P_effectsMix]);
      tft7.updateScreen();
      break;

      // TFT 8 Polymod etc

    case CCPM_DCO2:
      panelData[P_pmDCO2] = value;
      drawBar8(6, panelData[P_pmDCO2], NUM_STEPS, STEP_HEIGHT);
      tft8.updateScreen();
      break;

    case CCPM_FilterEnv:
      panelData[P_pmFilterEnv] = value;
      drawBar8(52, panelData[P_pmFilterEnv], NUM_STEPS, STEP_HEIGHT);
      tft8.updateScreen();
      break;

    case CCnoiseLevel:
      panelData[P_noiseLevel] = value;
      drawPWIndicator8(192, panelData[P_noiseLevel]);
      tft8.updateScreen();
      break;

    case CCamDepth:
      panelData[P_amDepth] = value;
      drawBar8(144, panelData[P_amDepth], NUM_STEPS, STEP_HEIGHT);
      tft8.updateScreen();
      break;

    case CCvolumeControl:
      panelData[P_volumeControl] = value;
      drawBar8(98, panelData[P_volumeControl], NUM_STEPS, STEP_HEIGHT);
      tft8.updateScreen();
      break;

    case CCmodwheel:
      switch (panelData[P_modWheelDepth]) {

        case 0:
          panelData[P_fmDepth] = 0;
          break;

        case 1:
          panelData[P_modWheelLevel] = (value / 5);
          panelData[P_fmDepth] = (int(panelData[P_modWheelLevel]));
          break;

        case 2:
          panelData[P_modWheelLevel] = (value / 4);
          panelData[P_fmDepth] = (int(panelData[P_modWheelLevel]));
          break;

        case 3:
          panelData[P_modWheelLevel] = (value / 3.5);
          panelData[P_fmDepth] = (int(panelData[P_modWheelLevel]));
          break;

        case 4:
          panelData[P_modWheelLevel] = (value / 3);
          panelData[P_fmDepth] = (int(panelData[P_modWheelLevel]));
          break;

        case 5:
          panelData[P_modWheelLevel] = (value / 2.5);
          panelData[P_fmDepth] = (int(panelData[P_modWheelLevel]));
          break;

        case 6:
          panelData[P_modWheelLevel] = (value / 2);
          panelData[P_fmDepth] = (int(panelData[P_modWheelLevel]));
          break;

        case 7:
          panelData[P_modWheelLevel] = (value / 1.75);
          panelData[P_fmDepth] = (int(panelData[P_modWheelLevel]));
          break;

        case 8:
          panelData[P_modWheelLevel] = (value / 1.5);
          panelData[P_fmDepth] = (int(panelData[P_modWheelLevel]));
          break;

        case 9:
          panelData[P_modWheelLevel] = (value / 1.25);
          panelData[P_fmDepth] = (int(panelData[P_modWheelLevel]));
          break;

        case 10:
          panelData[P_modWheelLevel] = value;
          panelData[P_fmDepth] = (int(panelData[P_modWheelLevel]));
          break;
      }
      break;
  }
}

void onSysExMessage(byte *data, unsigned length) {

  lastSysExByteTime = millis();

  Serial.print("Received SysEx message with length: ");
  Serial.println(length);

  // Check if the received data length matches our expected length
  if (length == sysexDataLength + 3) {  // 3 bytes for Manufacturer ID
    for (int i = 2; i < 72; i++) {
      byte highByte = data[i * 2];
      // Serial.print(highByte);
      // Serial.print(" ");
      byte lowByte = data[i * 2 + 1];
      // Serial.print(lowByte);
      // Serial.print(" ");
      panelData[i - 2] = (uint16_t)(highByte << 8) | lowByte;
      if (panelData[0] == 0) {
        memcpy(lowerData, panelData, 76);
      } else {
        memcpy(upperData, panelData, 76);
      }
    }
    //Serial.println(" ");

  } else {
    Serial.println("Received SysEx message of unexpected length");
  }
}

void processSysExData() {
  renderCurrentPatchPage(0);
  tft0.updateScreen();
  renderCurrentPatchPage(1);
  tft1.updateScreen();
  renderCurrentPatchPage(2);
  tft2.updateScreen();
  renderCurrentPatchPage(3);
  tft3.updateScreen();
  renderCurrentPatchPage(4);
  tft4.updateScreen();
  renderCurrentPatchPage(5);
  tft5.updateScreen();
  renderCurrentPatchPage(6);
  tft6.updateScreen();
  renderCurrentPatchPage(7);
  tft7.updateScreen();
  renderCurrentPatchPage(8);
  tft8.updateScreen();
  updateLEDs();
}

void updateLEDs() {
  switch (panelData[P_keyboardMode]) {
    case 0:
      srp.set(POLY1_LED, HIGH);
      srp.set(POLY2_LED, LOW);
      srp.set(UNISON_LED, LOW);
      srp.set(MONO_LED, LOW);
      break;

    case 1:
      srp.set(POLY1_LED, LOW);
      srp.set(POLY2_LED, HIGH);
      srp.set(UNISON_LED, LOW);
      srp.set(MONO_LED, LOW);
      break;

    case 2:
      srp.set(POLY1_LED, LOW);
      srp.set(POLY2_LED, LOW);
      srp.set(UNISON_LED, HIGH);
      srp.set(MONO_LED, LOW);
      break;

    case 3:
      srp.set(POLY1_LED, LOW);
      srp.set(POLY2_LED, LOW);
      srp.set(UNISON_LED, LOW);
      srp.set(MONO_LED, HIGH);
      break;
  }

  switch (panelData[P_NotePriority]) {
    case 0:
      srp.set(PRIORITY_RED_LED, HIGH);
      srp.set(PRIORITY_GREEN_LED, LOW);
      break;

    case 1:
      srp.set(PRIORITY_RED_LED, LOW);
      srp.set(PRIORITY_GREEN_LED, HIGH);
      break;

    case 2:
      srp.set(PRIORITY_RED_LED, HIGH);
      srp.set(PRIORITY_GREEN_LED, HIGH);
      break;
  }

  switch (upperSW) {
    case 0:
      srp.set(UPPER_LED, LOW);
      break;
    case 1:
      srp.set(LOWER_LED, HIGH);
      break;
  }

  switch (upperSW) {
    case 0:
      srp.set(UPPER_LED, LOW);
      srp.set(LOWER_LED, HIGH);
      break;
    case 1:
      srp.set(UPPER_LED, HIGH);
      srp.set(LOWER_LED, LOW);
      break;
  }

  switch (lowerSW) {
    case 0:
      srp.set(LOWER_LED, LOW);
      srp.set(UPPER_LED, HIGH);
      break;
    case 1:
      srp.set(LOWER_LED, HIGH);
      srp.set(UPPER_LED, LOW);
      break;
  }

  switch (panelData[P_glideSW]) {
    case 0:
      srp.set(GLIDE_SW_LED, LOW);
      break;
    case 1:
      srp.set(GLIDE_SW_LED, HIGH);
      break;
  }

  switch (panelData[P_sync]) {
    case 0:
      srp.set(SYNC_LED, LOW);
      break;
    case 1:
      srp.set(SYNC_LED, HIGH);
      break;
  }

  switch (panelData[P_filterPoleSW]) {
    case 0:
      srp.set(FILTER_POLE_LED, LOW);
      break;
    case 1:
      srp.set(FILTER_POLE_LED, HIGH);
      break;
  }

  switch (panelData[P_filterEGinv]) {
    case 0:
      srp.set(EG_INVERT_LED, LOW);
      break;
    case 1:
      srp.set(EG_INVERT_LED, HIGH);
      break;
  }

  switch (panelData[P_lfoAlt]) {
    case 0:
      srp.set(LFO_ALT_LED, LOW);
      break;
    case 1:
      srp.set(LFO_ALT_LED, HIGH);
      break;
  }

  switch (panelData[P_monoMulti]) {
    case 0:
      srp.set(LFO_MULTI_MONO_LED, LOW);
      break;
    case 1:
      srp.set(LFO_MULTI_MONO_LED, HIGH);
      break;
  }

  switch (panelData[P_vcaGate]) {
    case 0:
      srp.set(AMP_GATED_LED, LOW);
      break;
    case 1:
      srp.set(AMP_GATED_LED, HIGH);
      break;
  }

  switch (panelData[P_pmDestDCO1]) {
    case 0:
      srp.set(PM_DCO1_DEST_LED, LOW);
      break;
    case 1:
      srp.set(PM_DCO1_DEST_LED, HIGH);
      break;
  }

  switch (panelData[P_pmDestFilter]) {
    case 0:
      srp.set(PM_FILT_ENV_DEST_LED, LOW);
      break;
    case 1:
      srp.set(PM_FILT_ENV_DEST_LED, HIGH);
      break;
  }
}

void midiCCOut(int CCnumberTosend, int value) {
  Serial.print("MIDI CC ");
  Serial.println(CCnumberTosend);
  Serial.print("CC value ");
  Serial.println(value);
  MIDI.sendControlChange(CCnumberTosend, value, 1);
}

void loop() {

  // Prioritize MIDI processing
  MIDI.read(MIDI_CHANNEL_OMNI);
  //srp.update();  // update all the LEDs in the buttons

  // Check for any other tasks that need to be done
  if (millis() - lastSysExByteTime < sysExTimeout) {
    // Handle other tasks, such as screen updates, here
    //srp.update();  // update all the LEDs in the buttons
    processSysExData();
  }
}
