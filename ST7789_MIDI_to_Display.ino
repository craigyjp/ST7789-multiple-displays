#include <MIDI.h>
#include "Constants.h"
#include "MidiCC.h"
#include "Parameters.h"
#include "HWcontrols.h"
#include <Adafruit_GFX.h>  // Core graphics library
#include <ST7789_t3.h>     // Hardware-specific library
#include <SPI.h>
#include <RoxMux.h>

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

#define SRP_TOTAL 3
Rox74HC595<SRP_TOTAL> srp;

// pins for 74HC595
#define LED_DATA 35   // pin 14 on 74HC595 (DATA)
#define LED_CLK 34    // pin 11 on 74HC595 (CLK)
#define LED_LATCH 33  // pin 12 on 74HC595 (LATCH)
#define LED_PWM -1    // pin 13 on 74HC595

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

void setup() {

  Serial.begin(115200);
  SPI.begin();

  srp.begin(LED_DATA, LED_LATCH, LED_CLK, LED_PWM);

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
  MIDI.setHandleNoteOn(DinHandleNoteOn);
  MIDI.setHandleNoteOff(DinHandleNoteOff);
  MIDI.turnThruOn(midi::Thru::Mode::Off);
  Serial.println("MIDI In DIN Listening");

  usbMIDI.setHandleControlChange(myConvertControlChange);
  usbMIDI.setHandleSystemExclusive(onSysExMessage);
  Serial.println("MIDI In USB Listening");

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

void DinHandleNoteOn(byte channel, byte note, byte velocity) {
  if (timerRunning && millis() - timerStart < timerDuration) {
    // Reset the timer
    timerStart = millis();
    Serial.println("Note On received, timer reset.");
  }
}

void DinHandleNoteOff(byte channel, byte note, byte velocity) {
  if (note == 0) {
    // Start the 500ms timer
    timerStart = millis();
    timerRunning = true;
    Serial.println("Note Off received, timer started.");
  }
}

void myConvertControlChange(byte channel, byte number, byte value) {

  if (channel == 1) {
    //Serial.println("Received on channel 1 at the display");
    int newvalue = (value << 3);
    myControlChange(channel, number, newvalue);
  }

  if (channel == 2) {
    //Serial.println("Received on channel 2 at the display");
    int newvalue = value;
    myLEDupdate(channel, number, newvalue);
  }

}

void onSysExMessage(byte *data, unsigned length) {
  Serial.print("Received SysEx message with length: ");
  Serial.println(length);

  // Check if the received data length matches our expected length
  if (length == sysexDataLength + 3) {  // 3 bytes for Manufacturer ID

    for (int i = 0; i < (sysexDataLength - 1); i++) {
      panelData[i - 1] = map(data[i + 3], 0, 127, 0, 1023);  // Skip the Manufacturer ID bytes
    }

  } else {
    Serial.println("Received SysEx message of unexpected length");
  }
}

// void updateglideSW() {
//   parameterGroup = 0;
//   if (panelData[P_glideSW]) {
//     midiCCOut(CCglideSW, 127);
//   } else if (!panelData[P_glideSW]) {
//     midiCCOut(CCglideSW, 0);
//   }
// }

// void updateupperSW() {
//   parameterGroup = 0;
//   if (upperSW) {
//     midiCCOut(CCupperSW, 127);
//   } else if (!upperSW) {
//     midiCCOut(CCupperSW, 0);
//   }
// }

// void updatelowerSW() {
//   parameterGroup = 0;
//   if (lowerSW) {
//     midiCCOut(CClowerSW, 127);
//   } else if (!lowerSW) {
//     midiCCOut(CClowerSW, 0);
//   }
// }

// void updatechordHoldSW() {
//   parameterGroup = 0;
//   if (chordHoldSW) {
//     midiCCOut(CCchordHoldSW, 127);
//   } else if (!panelData[P_glideSW]) {
//     midiCCOut(CCchordHoldSW, 0);
//   }
// }

// void updatelfoAlt() {
//   if (panelData[P_lfoAlt]) {
//     midiCCOut(CClfoAlt, 127);
//   } else if (!panelData[P_lfoAlt]) {
//     midiCCOut(CClfoAlt, 0);
//   }
// }

// void updatefilterPoleSwitch() {
//   if (panelData[P_filterPoleSW]) {
//     midiCCOut(CCfilterPoleSW, 127);
//   } else if (!panelData[P_filterPoleSW]) {
//     midiCCOut(CCfilterPoleSW, 0);
//   }
// }

// void updatefilterEGinv() {
//   if (panelData[P_filterEGinv]) {
//     midiCCOut(CCfilterEGinv, 0);
//   } else if (!panelData[P_filterEGinv]) {
//     midiCCOut(CCfilterEGinv, 127);
//   }
// }

// void updatesyncSwitch() {
//   if (syncSW) {
//     midiCCOut(CCsyncSW, 127);
//   } else if (!syncSW) {
//     midiCCOut(CCsyncSW, 0);
//   }
// }

// void updateFilterType() {
//   switch (panelData[P_filterType]) {
//     case 0:
//       midiCCOut(CCfilterType, 0);
//       break;

//     case 1:
//       midiCCOut(CCfilterType, 8);
//       break;

//     case 2:
//       midiCCOut(CCfilterType, 16);
//       break;

//     case 3:
//       midiCCOut(CCfilterType, 24);
//       break;

//     case 4:
//       midiCCOut(CCfilterType, 32);
//       break;

//     case 5:
//       midiCCOut(CCfilterType, 40);
//       break;

//     case 6:
//       midiCCOut(CCfilterType, 48);
//       break;

//     case 7:
//       midiCCOut(CCfilterType, 56);
//       break;
//   }
// }

// void updateStratusLFOWaveform() {
//   switch (panelData[P_LFOWaveform]) {
//     case 0:
//       midiCCOut(CCLFOWaveform, 0);
//       break;

//     case 1:
//       midiCCOut(CCLFOWaveform, 8);
//       break;

//     case 2:
//       midiCCOut(CCLFOWaveform, 16);
//       break;

//     case 3:
//       midiCCOut(CCLFOWaveform, 24);
//       break;

//     case 4:
//       midiCCOut(CCLFOWaveform, 32);
//       break;

//     case 5:
//       midiCCOut(CCLFOWaveform, 40);
//       break;

//     case 6:
//       midiCCOut(CCLFOWaveform, 48);
//       break;

//     case 7:
//       midiCCOut(CCLFOWaveform, 56);
//       break;
//   }
// }

// void updatekeyboardMode() {
//   parameterGroup = 5;
//   switch (keyboardMode) {
//     case 0:
//       // srp.writePin(POLY_MODE_RED_LED, LOW);
//       // srp.writePin(POLY_MODE_GREEN_LED, LOW);
//       //allNotesOff();
//       break;

//     case 1:
//       // srp.writePin(POLY_MODE_RED_LED, HIGH);
//       // srp.writePin(POLY_MODE_GREEN_LED, LOW);
//       //allNotesOff();
//       break;

//     case 2:
//       // srp.writePin(POLY_MODE_RED_LED, LOW);
//       // srp.writePin(POLY_MODE_GREEN_LED, HIGH);
//       //allNotesOff();
//       break;

//     case 3:
//       // srp.writePin(POLY_MODE_RED_LED, HIGH);
//       // srp.writePin(POLY_MODE_GREEN_LED, HIGH);
//       //allNotesOff();
//       break;
//   }
// }

void myLEDupdate(byte channel, byte control, int value) {
  switch (control) {

    case CCLFOWaveform:
      panelData[P_LFOWaveform] = value;
      if (dumpCompleteSW == 0) {
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
      }
      break;

    case CCupperSW:
      if (value == 127) {
        srp.writePin(UPPER_LED, HIGH);
        srp.writePin(LOWER_LED, LOW);
        upperSW = 1;
        lowerSW = 0;
      } else if (value == 0) {
        srp.writePin(UPPER_LED, LOW);
        srp.writePin(UPPER_LED, HIGH);
        upperSW = 0;
        lowerSW = 1;
      }
      if (dumpCompleteSW == 0) {
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
        tft0.updateScreen();
      }
      break;

    case CClowerSW:
      if (value == 127) {
        srp.writePin(UPPER_LED, LOW);
        srp.writePin(LOWER_LED, HIGH);
        upperSW = 0;
        lowerSW = 1;
      } else if (value == 0) {
        srp.writePin(UPPER_LED, LOW);
        srp.writePin(UPPER_LED, HIGH);
        upperSW = 1;
        lowerSW = 0;
      }
      if (dumpCompleteSW == 0) {
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
        tft0.updateScreen();
      }
      break;

    case CCglideSW:
      if (value == 127) {
        srp.writePin(GLIDE_SW_LED, HIGH);
        panelData[P_glideSW] = 1;
      } else if (value == 0) {
        srp.writePin(GLIDE_SW_LED, LOW);
        panelData[P_glideSW] = 0;
      }
      if (dumpCompleteSW == 0) {
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
      }
      break;

    case CCchordHoldSW:
      if (value == 127) {
        srp.writePin(CHORD_HOLD_LED, HIGH);
        chordHoldSW = 1;
      } else if (value == 0) {
        srp.writePin(CHORD_HOLD_LED, LOW);
        chordHoldSW = 0;
      }
      if (dumpCompleteSW == 0) {
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
      }
      break;

    case CCfilterPoleSW:
      if (value == 127) {
        srp.writePin(FILTER_POLE_LED, HIGH);
        panelData[P_filterPoleSW] = 1;
        tft3.fillRoundRect(240, 10, 70, 30, 5, ST7735_RED);
      } else if (value == 0) {
        srp.writePin(FILTER_POLE_LED, LOW);
        panelData[P_filterPoleSW] = 0;
        tft3.fillRoundRect(240, 10, 70, 30, 5, ST7735_GREEN);  // Green box for off
      }
      if (dumpCompleteSW == 0) {
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
      }
      break;

    case CCsyncSW:
      if (value == 127) {
        srp.writePin(SYNC_LED, HIGH);
        syncSW = 1;
      } else if (value == 0) {
        srp.writePin(SYNC_LED, LOW);
        syncSW = 0;
      }
      if (dumpCompleteSW == 0) {
        if (syncSW == 1) {
          tft2.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);
        } else {
          tft2.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);
        }
        tft2.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
        tft2.setCursor(30, 18);
        tft2.print(syncSW == 0 ? "Sync Off" : "Sync On");
        tft2.updateScreen();
      }
      break;

    case CClfoAlt:
      if (value == 127) {
        srp.writePin(LFO_ALT_LED, HIGH);
        panelData[P_lfoAlt] = 1;
      } else if (value == 0) {
        srp.writePin(LFO_ALT_LED, LOW);
        panelData[P_lfoAlt] = 0;
      }
      if (dumpCompleteSW == 0) {
        if (panelData[P_lfoAlt] == 1) {
          tft6.fillRoundRect(180, 50, 130, 30, 5, ST7735_RED);
        } else {
          tft6.fillRoundRect(180, 50, 130, 30, 5, ST7735_GREEN);
        }
        tft6.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
        tft6.setCursor(195, 58);
        tft6.print(panelData[P_lfoAlt] == 0 ? "Alt Off" : "Alt On");
        tft6.updateScreen();
      }
      break;

    case CCfilterType:
      panelData[P_filterType] = value;
      if (dumpCompleteSW == 0) {
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
      }
      break;

    case CCosc1Oct:
      panelData[P_osc1Range] = value;
      if (dumpCompleteSW == 0) {
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
      }
      break;

    case CCosc2Oct:
      panelData[P_osc2Range] = value;
      if (dumpCompleteSW == 0) {
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
      }
      break;

    case CCfilterEGinv:
      if (value == 127) {
        srp.writePin(EG_INVERT_LED, HIGH);
        panelData[P_filterEGinv] = 1;
      } else if (value == 0) {
        srp.writePin(EG_INVERT_LED, LOW);
        panelData[P_filterEGinv] = 0;
      }
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(4);
      // }
      break;
  }
}

void myControlChange(byte channel, byte control, int value) {
  switch (control) {

      // TFT 0

    case CCglideTime:
      panelData[P_glideTime] = value;
      if (!dumpCompleteSW) {
        drawBar0(6, panelData[P_glideTime], NUM_STEPS, STEP_HEIGHT);
        tft0.updateScreen();
      }
      break;

      // TFT 1
    case CCosc1PW:
      panelData[P_osc1PW] = value;
      if (dumpCompleteSW == 0) {
        drawPWIndicator1(6, panelData[P_osc1PW]);
        tft1.updateScreen();
      }
      break;

    case CCosc1PWM:
      panelData[P_osc1PWM] = value;
      if (dumpCompleteSW == 0) {
        drawBar1(52, panelData[P_osc1PWM], NUM_STEPS, STEP_HEIGHT);
        tft1.updateScreen();
      }
      break;

    case CCosc1SawLevel:
      panelData[P_osc1SawLevel] = value;
      if (dumpCompleteSW == 0) {
        drawBar1(192, panelData[P_osc1SawLevel], NUM_STEPS, STEP_HEIGHT);
        tft1.updateScreen();
      }
      break;

    case CCosc1PulseLevel:
      panelData[P_osc1PulseLevel] = value;
      if (dumpCompleteSW == 0) {
        drawBar1(236, panelData[P_osc1PulseLevel], NUM_STEPS, STEP_HEIGHT);
        tft1.updateScreen();
      }
      break;

    case CCosc1SubLevel:
      panelData[P_osc1SubLevel] = value;
      if (dumpCompleteSW == 0) {
        drawBar1(282, panelData[P_osc1SubLevel], NUM_STEPS, STEP_HEIGHT);
        tft1.updateScreen();
      }
      break;

    case CCoscfmDepth:
      panelData[P_fmDepth] = value;
      if (dumpCompleteSW == 0) {
        drawBar1(98, panelData[P_fmDepth], NUM_STEPS, STEP_HEIGHT);
        tft1.updateScreen();
      }
      break;

      // TFT 2

    case CCosc2PW:
      panelData[P_osc2PW] = value;
      if (dumpCompleteSW == 0) {
        drawPWIndicator2(6, panelData[P_osc2PW]);
        tft2.updateScreen();
      }
      break;

    case CCosc2PWM:
      panelData[P_osc2PWM] = value;
      if (dumpCompleteSW == 0) {
        drawBar2(52, panelData[P_osc2PWM], NUM_STEPS, STEP_HEIGHT);
        tft2.updateScreen();
      }
      break;

    case CCosc2Detune:
      panelData[P_osc2Detune] = value;
      if (dumpCompleteSW == 0) {
        drawBar2(98, panelData[P_osc2Detune], NUM_STEPS, STEP_HEIGHT);
        tft2.updateScreen();
      }
      break;

    case CCosc2Interval:
      panelData[P_osc2Interval] = value;
      if (dumpCompleteSW == 0) {
        drawBar2(144, panelData[P_osc2Interval], DET_STEPS, DET_STEP_HEIGHT);
        tft2.updateScreen();
      }
      break;

    case CCosc2sawLevel:
      panelData[P_osc2SawLevel] = value;
      if (dumpCompleteSW == 0) {
        drawBar2(192, panelData[P_osc2SawLevel], NUM_STEPS, STEP_HEIGHT);
        tft2.updateScreen();
      }
      break;

    case CCosc2pulseLevel:
      panelData[P_osc2PulseLevel] = value;
      if (dumpCompleteSW == 0) {
        drawBar2(236, panelData[P_osc2PulseLevel], NUM_STEPS, STEP_HEIGHT);
        tft2.updateScreen();
      }
      break;

    case CCosc2triangleLevel:
      panelData[P_osc2TriangleLevel] = value;
      if (dumpCompleteSW == 0) {
        drawBar2(282, panelData[P_osc2TriangleLevel], NUM_STEPS, STEP_HEIGHT);
        tft2.updateScreen();
      }
      break;

      // TFT 3

    case CCKeyTrack:
      panelData[P_keytrack] = value;
      if (dumpCompleteSW == 0) {
        drawBar3(144, panelData[P_keytrack], NUM_STEPS, STEP_HEIGHT);
        tft3.updateScreen();
      }
      break;

    case CCfilterCutoff:
      panelData[P_filterCutoff] = value;
      if (dumpCompleteSW == 0) {
        drawBar3(6, panelData[P_filterCutoff], NUM_STEPS, STEP_HEIGHT);
        tft3.updateScreen();
      }
      break;

    case CCfilterLFO:
      panelData[P_filterLFO] = value;
      if (dumpCompleteSW == 0) {
        drawBar3(192, panelData[P_filterLFO], NUM_STEPS, STEP_HEIGHT);
        tft3.updateScreen();
      }
      break;

    case CCfilterRes:
      panelData[P_filterRes] = value;
      if (dumpCompleteSW == 0) {
        drawBar3(52, panelData[P_filterRes], NUM_STEPS, STEP_HEIGHT);
        tft3.updateScreen();
      }
      break;

    case CCfilterEGlevel:
      panelData[P_filterEGlevel] = value;
      if (dumpCompleteSW == 0) {
        drawBar3(98, panelData[P_filterEGlevel], NUM_STEPS, STEP_HEIGHT);
        tft3.updateScreen();
      }
      break;

    case CCeffect1:
      panelData[P_effectPot1] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(7);
      // }
      break;

    case CCeffect2:
      panelData[P_effectPot2] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(7);
      // }
      break;

    case CCeffect3:
      panelData[P_effectPot3] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(7);
      // }
      break;

    case CCeffectMix:
      panelData[P_effectsMix] = map(value, 0, readRes, 0, readRes);
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(7);
      // }
      break;

    case CCnoiseLevel:
      panelData[P_noiseLevel] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(8);
      // }
      break;

    case CCLFORate:
      panelData[P_LFORate] = value;
      if (dumpCompleteSW == 0) {
        drawBar6(6, panelData[P_LFORate], NUM_STEPS, STEP_HEIGHT);
        tft6.updateScreen();
      }
      break;

    case CCfilterAttack:
      panelData[P_filterAttack] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(4);
      // }
      break;

    case CCfilterDecay:
      panelData[P_filterDecay] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(4);
      // }
      break;

    case CCfilterSustain:
      panelData[P_filterSustain] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(4);
      // }
      break;

    case CCfilterRelease:
      panelData[P_filterRelease] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(4);
      // }
      break;

    case CCampAttack:
      panelData[P_ampAttack] = value;
      panelData[P_oldampAttack] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(5);
      // }
      break;

    case CCampDecay:
      panelData[P_ampDecay] = value;
      panelData[P_oldampDecay] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(5);
      // }
      break;

    case CCampSustain:
      panelData[P_ampSustain] = value;
      panelData[P_oldampSustain] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(5);
      // }
      break;

    case CCampRelease:
      panelData[P_ampRelease] = value;
      panelData[P_oldampRelease] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(5);
      // }
      break;

    case CCvolumeControl:
      panelData[P_volumeControl] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(8);
      // }
      break;

    case CCLFODelay:
      panelData[P_LFODelay] = value;
      if (dumpCompleteSW == 0) {
        drawBar6(52, panelData[P_LFODelay], NUM_STEPS, STEP_HEIGHT);
        tft6.updateScreen();
      }
      break;

    case CCpwLFO:
      panelData[P_pwLFO] = value;
      if (dumpCompleteSW == 0) {
        drawBar6(98, panelData[P_pwLFO], NUM_STEPS, STEP_HEIGHT);
        tft6.updateScreen();
      }
      break;

    case CCmodWheelDepth:
      panelData[P_modWheelDepth] = value;
      if (dumpCompleteSW == 0) {
        drawBar6(144, panelData[P_modWheelDepth], NUM_STEPS, STEP_HEIGHT);
        tft6.updateScreen();
      }
      break;

    case CCamDepth:
      panelData[P_amDepth] = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(8);
      // }
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

      // Switches ////////////////////////////////////////////////

    case CCfilterType:
      panelData[P_filterType] = value;
      //updateFilterType();
      break;

    case CCLFOWaveform:
      panelData[P_LFOWaveform] = value;
      //updateStratusLFOWaveform();
      break;

    case CCglideSW:
      value > 0 ? panelData[P_glideSW] = 1 : panelData[P_glideSW] = 0;
      //updateglideSW();
      break;

    case CCupperSW:
      upperSW = 1;
      lowerSW = 0;
      //updateupperSW();
      break;

    case CClowerSW:
      lowerSW = 1;
      upperSW = 0;
      //updatelowerSW();
      break;

    case CCchordHoldSW:
      value > 0 ? chordHoldSW = 1 : chordHoldSW = 0;
      //updatechordHoldSW();
      break;

    case CCfilterPoleSW:
      value > 0 ? panelData[P_filterPoleSW] = 1 : panelData[P_filterPoleSW] = 0;
      //updatefilterPoleSwitch();
      break;

    case CCfilterEGinv:
      value > 0 ? panelData[P_filterEGinv] = 1 : panelData[P_filterEGinv] = 0;
      //updatefilterEGinv();
      break;

    case CCsyncSW:
      value > 0 ? syncSW = 1 : syncSW = 0;
      //updatesyncSwitch();
      break;

    // case CCosc1Oct:
    //   switch (panelData[P_osc1Range]) {
    //     case 0:
    //       midiCCOut(CCosc1Oct, 0);
    //       break;

    //     case 1:
    //       midiCCOut(CCosc1Oct, 64);
    //       break;

    //     case 2:
    //       midiCCOut(CCosc1Oct, 127);
    //       break;
    //   }
    //   break;

    // case CCosc2Oct:
    //   switch (panelData[P_osc2Range]) {
    //     case 0:
    //       midiCCOut(CCosc2Oct, 0);
    //       break;

    //     case 1:
    //       midiCCOut(CCosc2Oct, 64);
    //       break;

    //     case 2:
    //       midiCCOut(CCosc2Oct, 127);
    //       break;
    //   }
    //   break;


    case CClfoAlt:
      value > 0 ? panelData[P_lfoAlt] = 1 : panelData[P_lfoAlt] = 0;
      //updatelfoAlt();
      break;

      // case CClfoMult:
      //   updatelfoMult();
      //   break;

      // case CCfilterVelSW:
      //   value > 0 ? panelData[P_filterVel] = 1 : panelData[P_filterVel] = 0;
      //   updatefilterVelSW();
      //   break;

      // case CCampVelSW:
      //   value > 0 ? panelData[P_vcaVel] = 1 : panelData[P_vcaVel] = 0;
      //   updateampVelSW();
      //   break;

      // case CCFilterLoop:
      //   updateFilterLoop();
      //   break;

      // case CCAmpLoop:
      //   updateAmpLoop();
      //   break;

      // case CCAmpGatedSW:
      //   value > 0 ? panelData[P_vcaGate] = 1 : panelData[P_vcaGate] = 0;
      //   updateAmpGatedSW();
      //   break;

      // case CCeffectBankSW:
      //   updateeffectBankSW();
      //   break;

      // case CCeffectNumSW:
      //   updateeffectNumSW();
      //   break;

      // case CCenvLinLogSW:
      //   value > 0 ? envLinLogSW = 1 : envLinLogSW = 0;
      //   updateenvLinLogSW();
      //   break;

    case CCkeyboardMode:
      //updatekeyboardMode();
      break;

      // case CCNotePriority:
      //   updateNotePriority();
      //   break;

      // case CCmonoMultiSW:
      //   value > 0 ? monoMultiSW = 1 : monoMultiSW = 0;
      //   updatemonoMultiSW();
      //   break;

      // case CCallnotesoff:
      //   allNotesOff();
      //   break;
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

  MIDI.read(MIDI_CHANNEL_OMNI);

  srp.update();         // update all the LEDs in the buttons

  // Check if the timer has expired
  if (timerRunning && millis() - timerStart >= timerDuration) {
    // Timer expired, do something

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

    Serial.println("Timer expired.");
    timerRunning = false;
  }
}
