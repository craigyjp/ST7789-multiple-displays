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

#define OCTO_TOTAL 4
#define BTN_DEBOUNCE 50
RoxOctoswitch<OCTO_TOTAL, BTN_DEBOUNCE> octoswitch;

// pins for 74HC165
#define PIN_DATA 30  // pin 9 on 74HC165 (DATA)
#define PIN_CLK 31   // pin 2 on 74HC165 (CLK))
#define PIN_LOAD 32  // pin 1 on 74HC165 (LOAD)

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

  octoswitch.begin(PIN_DATA, PIN_LOAD, PIN_CLK);
  octoswitch.setCallback(onButtonPress);

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
  MIDI.turnThruOn(midi::Thru::Mode::Off);
  Serial.println("MIDI In DIN Listening");

  usbMIDI.setHandleControlChange(myConvertControlChange);
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

  if (channel == 3) {
    //Serial.println("Received on channel 3 at the display");
    int newvalue = value;
    myPatchUpdate(channel, number, newvalue);
  }
}

void updateglideSW() {
  parameterGroup = 0;
  if (glideSW) {
    midiCCOut(CCglideSW, 127);
  } else if (!glideSW) {
    midiCCOut(CCglideSW, 0);
  }
}

void updateupperSW() {
  parameterGroup = 0;
  if (upperSW) {
    midiCCOut(CCupperSW, 127);
  } else if (!upperSW) {
    midiCCOut(CCupperSW, 0);
  }
}

void updatelowerSW() {
  parameterGroup = 0;
  if (lowerSW) {
    midiCCOut(CClowerSW, 127);
  } else if (!lowerSW) {
    midiCCOut(CClowerSW, 0);
  }
}

void updatechordHoldSW() {
  parameterGroup = 0;
  if (chordHoldSW) {
    midiCCOut(CCchordHoldSW, 127);
  } else if (!glideSW) {
    midiCCOut(CCchordHoldSW, 0);
  }
}

void updatelfoAlt() {
  parameterGroup = 6;
  if (lfoAlt) {
    midiCCOut(CClfoAlt, 127);
  } else if (!lfoAlt) {
    midiCCOut(CClfoAlt, 0);
  }
}

void updatefilterPoleSwitch() {
  parameterGroup = 3;
  if (filterPoleSW) {
    midiCCOut(CCfilterPoleSW, 127);
  } else if (!filterPoleSW) {
    midiCCOut(CCfilterPoleSW, 0);
  }
}

void updatefilterEGinv() {
  parameterGroup = 4;
  if (filterEGinv) {
    midiCCOut(CCfilterEGinv, 0);
  } else if (!filterEGinv) {
    midiCCOut(CCfilterEGinv, 127);
  }
}

void updatesyncSwitch() {
  if (syncSW) {
    midiCCOut(CCsyncSW, 127);
  } else if (!syncSW) {
    midiCCOut(CCsyncSW, 0);
  }
}

void updateFilterType() {
  parameterGroup = 3;
  switch (filterType) {
    case 0:
      midiCCOut(CCfilterType, filterType);
      break;

    case 1:
      midiCCOut(CCfilterType, filterType);
      break;

    case 2:
      midiCCOut(CCfilterType, filterType);
      break;

    case 3:
      midiCCOut(CCfilterType, filterType);
      break;

    case 4:
      midiCCOut(CCfilterType, filterType);
      break;

    case 5:
      midiCCOut(CCfilterType, filterType);
      break;

    case 6:
      midiCCOut(CCfilterType, filterType);
      break;

    case 7:
      midiCCOut(CCfilterType, filterType);
      break;
  }
}

void updateStratusLFOWaveform() {
  parameterGroup = 6;
  switch (LFOWaveform) {
    case 0:
      midiCCOut(CCLFOWaveform, 0);
      break;

    case 1:
      midiCCOut(CCLFOWaveform, 1);
      break;

    case 2:
      midiCCOut(CCLFOWaveform, 2);
      break;

    case 3:
      midiCCOut(CCLFOWaveform, 3);
      break;

    case 4:
      midiCCOut(CCLFOWaveform, 4);
      break;

    case 5:
      midiCCOut(CCLFOWaveform, 5);
      break;

    case 6:
      midiCCOut(CCLFOWaveform, 6);
      break;

    case 7:
      midiCCOut(CCLFOWaveform, 7);
      break;
  }
}

void updatekeyboardMode() {
  parameterGroup = 5;
  switch (keyboardMode) {
    case 0:
      // srp.writePin(POLY_MODE_RED_LED, LOW);
      // srp.writePin(POLY_MODE_GREEN_LED, LOW);
      //allNotesOff();
      break;

    case 1:
      // srp.writePin(POLY_MODE_RED_LED, HIGH);
      // srp.writePin(POLY_MODE_GREEN_LED, LOW);
      //allNotesOff();
      break;

    case 2:
      // srp.writePin(POLY_MODE_RED_LED, LOW);
      // srp.writePin(POLY_MODE_GREEN_LED, HIGH);
      //allNotesOff();
      break;

    case 3:
      // srp.writePin(POLY_MODE_RED_LED, HIGH);
      // srp.writePin(POLY_MODE_GREEN_LED, HIGH);
      //allNotesOff();
      break;
  }
}

void myPatchUpdate(byte channel, byte control, int value) {

  switch (control) {

    case CCdumpStartedSW:
      dumpCompleteSW = 1;
      break;

    case CCdumpCompleteSW:
      switch (value) {
        case 0:
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
          dumpCompleteSW = 0;
          break;

        case 1:
          dumpCompleteSW = 1;
          break;
      }
  }
}

void myLEDupdate(byte channel, byte control, int value) {
  switch (control) {

    case CCLFOWaveform:
      LFOWaveform = value;
      if (dumpCompleteSW == 0) {
        switch (lfoAlt) {
          case 1:
            str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(lfo02[LFOWaveform])));
            break;
          case 0:
            str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(lfo01[LFOWaveform])));
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
        glideSW = 1;
      } else if (value == 0) {
        srp.writePin(GLIDE_SW_LED, LOW);
        glideSW = 0;
      }
      if (dumpCompleteSW == 0) {
        tft0.setFont(&FreeSans12pt7b);
        if (glideSW == 0) {
          tft0.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
        } else {
          tft0.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);  // Red box for on
        }
        tft0.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
        tft0.setCursor(30, 18);
        tft0.print(glideSW == 0 ? "Glide Off" : "Glide On");
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
        filterPoleSW = 1;
        tft3.fillRoundRect(240, 10, 70, 30, 5, ST7735_RED);
      } else if (value == 0) {
        srp.writePin(FILTER_POLE_LED, LOW);
        filterPoleSW = 0;
        tft3.fillRoundRect(240, 10, 70, 30, 5, ST7735_GREEN);  // Green box for off
      }
      if (dumpCompleteSW == 0) {
        tft3.setFont(&FreeSans12pt7b);

        switch (filterPoleSW) {
          case 1:
            str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(filter01[filterType])));
            break;
          case 0:
            str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(filter02[filterType])));
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
        tft3.print(filterPoleSW == 0 ? "Off" : "On");

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
        lfoAlt = 1;
      } else if (value == 0) {
        srp.writePin(LFO_ALT_LED, LOW);
        lfoAlt = 0;
      }
      if (dumpCompleteSW == 0) {
        if (lfoAlt == 1) {
          tft6.fillRoundRect(180, 50, 130, 30, 5, ST7735_RED);
        } else {
          tft6.fillRoundRect(180, 50, 130, 30, 5, ST7735_GREEN);
        }
        tft6.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
        tft6.setCursor(195, 58);
        tft6.print(lfoAlt == 0 ? "Alt Off" : "Alt On");
        tft6.updateScreen();
      }
      break;

    case CCfilterType:
      filterType = value;
      if (dumpCompleteSW == 0) {
        switch (filterPoleSW) {
          case 1:
            str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(filter01[filterType])));
            break;
          case 0:
            str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(filter02[filterType])));
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
        if (filterPoleSW == 0) {
          tft3.fillRoundRect(240, 10, 70, 30, 5, ST7735_GREEN);  // Green box for off
        } else {
          tft3.fillRoundRect(240, 10, 70, 30, 5, ST7735_RED);  // Red box for on
        }
        tft3.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
        tft3.setCursor(260, 18);
        tft3.print(filterPoleSW == 0 ? "Off" : "On");

        // Set range label and value inside a box along the top
        tft3.fillRoundRect(10, 10, 200, 30, 5, ST7735_YELLOW);  // Background box for range
        tft3.setCursor(40, 18);
        tft3.setTextColor(ST7735_BLACK);
        tft3.print(filterDisplay);
        tft3.updateScreen();
      }
      break;

    case CCosc1Oct:
      oct1 = value;
      if (dumpCompleteSW == 0) {
        tft1.setFont(&FreeSans12pt7b);
        // Set range label and value inside a box along the top
        tft1.fillRoundRect(180, 10, 130, 30, 5, ST7735_YELLOW);  // Background box for range
        tft1.setTextColor(ST7735_BLACK);
        tft1.setCursor(195, 18);
        switch (oct1) {
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
      oct2 = value;
      if (dumpCompleteSW == 0) {
        tft2.setFont(&FreeSans12pt7b);
        // Set range label and value inside a box along the top
        tft2.fillRoundRect(180, 10, 130, 30, 5, ST7735_YELLOW);  // Background box for range
        tft2.setTextColor(ST7735_BLACK);
        tft2.setCursor(195, 18);
        switch (oct2) {
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
        filterEGinv = 1;
      } else if (value == 0) {
        srp.writePin(EG_INVERT_LED, LOW);
        filterEGinv = 0;
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
      glideTime = value;
      if (!dumpCompleteSW) {
        drawBar0(6, glideTime, NUM_STEPS, STEP_HEIGHT);
        tft0.updateScreen();
      }
      break;

      // TFT 1
    case CCosc1PW:
      osc1PW = value;
      if (dumpCompleteSW == 0) {
        drawPWIndicator1(6, osc1PW);
        tft1.updateScreen();
      }
      break;

    case CCosc1PWM:
      osc1PWM = value;
      if (dumpCompleteSW == 0) {
        drawBar1(52, osc1PWM, NUM_STEPS, STEP_HEIGHT);
        tft1.updateScreen();
      }
      break;

    case CCosc1SawLevel:
      osc1sawLevel = value;
      if (dumpCompleteSW == 0) {
        drawBar1(192, osc1sawLevel, NUM_STEPS, STEP_HEIGHT);
        tft1.updateScreen();
      }
      break;

    case CCosc1PulseLevel:
      osc1pulseLevel = value;
      if (dumpCompleteSW == 0) {
        drawBar1(236, osc1pulseLevel, NUM_STEPS, STEP_HEIGHT);
        tft1.updateScreen();
      }
      break;

    case CCosc1SubLevel:
      osc1subLevel = value;
      if (dumpCompleteSW == 0) {
        drawBar1(282, osc1subLevel, NUM_STEPS, STEP_HEIGHT);
        tft1.updateScreen();
      }
      break;

    case CCoscfmDepth:
      oscfmDepth = value;
      if (dumpCompleteSW == 0) {
        drawBar1(98, oscfmDepth, NUM_STEPS, STEP_HEIGHT);
        tft1.updateScreen();
      }
      break;

      // TFT 2

    case CCosc2PW:
      osc2PW = value;
      if (dumpCompleteSW == 0) {
        drawPWIndicator2(6, osc2PW);
        tft2.updateScreen();
      }
      break;

    case CCosc2PWM:
      osc2PWM = value;
      if (dumpCompleteSW == 0) {
        drawBar2(52, osc2PWM, NUM_STEPS, STEP_HEIGHT);
        tft2.updateScreen();
      }
      break;

    case CCosc2Detune:
      osc2detune = value;
      if (dumpCompleteSW == 0) {
        drawBar2(98, osc2detune, NUM_STEPS, STEP_HEIGHT);
        tft2.updateScreen();
      }
      break;

    case CCosc2Interval:
      osc2interval = value;
      if (dumpCompleteSW == 0) {
        drawBar2(144, osc2interval, DET_STEPS, DET_STEP_HEIGHT);
        tft2.updateScreen();
      }
      break;

    case CCosc2sawLevel:
      osc2sawLevel = value;
      if (dumpCompleteSW == 0) {
        drawBar2(192, osc2sawLevel, NUM_STEPS, STEP_HEIGHT);
        tft2.updateScreen();
      }
      break;

    case CCosc2pulseLevel:
      osc2pulseLevel = value;
      if (dumpCompleteSW == 0) {
        drawBar2(236, osc2pulseLevel, NUM_STEPS, STEP_HEIGHT);
        tft2.updateScreen();
      }
      break;

    case CCosc2triangleLevel:
      osc2triLevel = value;
      if (dumpCompleteSW == 0) {
        drawBar2(282, osc2triLevel, NUM_STEPS, STEP_HEIGHT);
        tft2.updateScreen();
      }
      break;

      // TFT 3

    case CCKeyTrack:
      keyTrack = value;
      if (dumpCompleteSW == 0) {
        drawBar3(144, keyTrack, NUM_STEPS, STEP_HEIGHT);
        tft3.updateScreen();
      }
      break;

    case CCfilterCutoff:
      filterCutoff = value;
      if (dumpCompleteSW == 0) {
        drawBar3(6, filterCutoff, NUM_STEPS, STEP_HEIGHT);
        tft3.updateScreen();
      }
      break;

    case CCfilterLFO:
      filterLFO = value;
      if (dumpCompleteSW == 0) {
        drawBar3(192, filterLFO, NUM_STEPS, STEP_HEIGHT);
        tft3.updateScreen();
      }
      break;

    case CCfilterRes:
      filterRes = value;
      if (dumpCompleteSW == 0) {
        drawBar3(52, filterRes, NUM_STEPS, STEP_HEIGHT);
        tft3.updateScreen();
      }
      break;

    case CCfilterEGlevel:
      filterEGlevel = value;
      if (dumpCompleteSW == 0) {
        drawBar3(98, filterEGlevel, NUM_STEPS, STEP_HEIGHT);
        tft3.updateScreen();
      }
      break;

    case CCeffect1:
      effect1 = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(7);
      // }
      break;

    case CCeffect2:
      effect2 = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(7);
      // }
      break;

    case CCeffect3:
      effect3 = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(7);
      // }
      break;

    case CCeffectMix:
      mixa = map(value, 0, readRes, 0, readRes);
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(7);
      // }
      break;

    case CCnoiseLevel:
      noiseLevel = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(8);
      // }
      break;

    case CCLFORate:
      LFORate = value;
      if (dumpCompleteSW == 0) {
        drawBar6(6, LFORate, NUM_STEPS, STEP_HEIGHT);
        tft6.updateScreen();
      }
      break;

    case CCfilterAttack:
      filterAttack = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(4);
      // }
      break;

    case CCfilterDecay:
      filterDecay = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(4);
      // }
      break;

    case CCfilterSustain:
      filterSustain = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(4);
      // }
      break;

    case CCfilterRelease:
      filterRelease = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(4);
      // }
      break;

    case CCampAttack:
      ampAttack = value;
      oldampAttack = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(5);
      // }
      break;

    case CCampDecay:
      ampDecay = value;
      oldampDecay = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(5);
      // }
      break;

    case CCampSustain:
      ampSustain = value;
      oldampSustain = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(5);
      // }
      break;

    case CCampRelease:
      ampRelease = value;
      oldampRelease = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(5);
      // }
      break;

    case CCvolumeControl:
      volumeControl = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(8);
      // }
      break;

    case CCLFODelay:
      lfoDelay = value;
      if (dumpCompleteSW == 0) {
        drawBar6(52, lfoDelay, NUM_STEPS, STEP_HEIGHT);
        tft6.updateScreen();
      }
      break;

    case CCpwLFO:
      PWRate = value;
      if (dumpCompleteSW == 0) {
        drawBar6(98, PWRate, NUM_STEPS, STEP_HEIGHT);
        tft6.updateScreen();
      }
      break;

    case CCmodWheelDepth:
      modWheelDepth = value;
      if (dumpCompleteSW == 0) {
        drawBar6(144, modWheelDepth, NUM_STEPS, STEP_HEIGHT);
        tft6.updateScreen();
      }
      break;

    case CCamDepth:
      amDepth = value;
      // if (!dumpCompleteSW) {
      //   renderCurrentPatchPage(8);
      // }
      break;

    case CCmodwheel:
      switch (modWheelDepth) {

        case 0:
          oscfmDepth = 0;
          break;

        case 1:
          modWheelLevel = (value / 5);
          oscfmDepth = (int(modWheelLevel));
          break;

        case 2:
          modWheelLevel = (value / 4);
          oscfmDepth = (int(modWheelLevel));
          break;

        case 3:
          modWheelLevel = (value / 3.5);
          oscfmDepth = (int(modWheelLevel));
          break;

        case 4:
          modWheelLevel = (value / 3);
          oscfmDepth = (int(modWheelLevel));
          break;

        case 5:
          modWheelLevel = (value / 2.5);
          oscfmDepth = (int(modWheelLevel));
          break;

        case 6:
          modWheelLevel = (value / 2);
          oscfmDepth = (int(modWheelLevel));
          break;

        case 7:
          modWheelLevel = (value / 1.75);
          oscfmDepth = (int(modWheelLevel));
          break;

        case 8:
          modWheelLevel = (value / 1.5);
          oscfmDepth = (int(modWheelLevel));
          break;

        case 9:
          modWheelLevel = (value / 1.25);
          oscfmDepth = (int(modWheelLevel));
          break;

        case 10:
          modWheelLevel = value;
          oscfmDepth = (int(modWheelLevel));
          break;
      }
      break;

      // Switches ////////////////////////////////////////////////

    case CCfilterType:
      filterType = value;
      updateFilterType();
      break;

    case CCLFOWaveform:
      LFOWaveform = value;
      updateStratusLFOWaveform();
      break;

    case CCglideSW:
      value > 0 ? glideSW = 1 : glideSW = 0;
      updateglideSW();
      break;

    case CCupperSW:
      upperSW = 1;
      lowerSW = 0;
      updateupperSW();
      break;

    case CClowerSW:
      lowerSW = 1;
      upperSW = 0;
      updatelowerSW();
      break;

    case CCchordHoldSW:
      value > 0 ? chordHoldSW = 1 : chordHoldSW = 0;
      updatechordHoldSW();
      break;

    case CCfilterPoleSW:
      value > 0 ? filterPoleSW = 1 : filterPoleSW = 0;
      updatefilterPoleSwitch();
      break;

    case CCfilterEGinv:
      value > 0 ? filterEGinv = 1 : filterEGinv = 0;
      updatefilterEGinv();
      break;

    case CCsyncSW:
      value > 0 ? syncSW = 1 : syncSW = 0;
      updatesyncSwitch();
      break;

    case CCosc1Oct:
      switch (oct1) {
        case 0:
          midiCCOut(CCosc1Oct, 0);
          break;

        case 1:
          midiCCOut(CCosc1Oct, 64);
          break;

        case 2:
          midiCCOut(CCosc1Oct, 127);
          break;
      }
      break;

    case CCosc2Oct:
      switch (oct2) {
        case 0:
          midiCCOut(CCosc2Oct, 0);
          break;

        case 1:
          midiCCOut(CCosc2Oct, 64);
          break;

        case 2:
          midiCCOut(CCosc2Oct, 127);
          break;
      }
      break;


    case CClfoAlt:
      value > 0 ? lfoAlt = 1 : lfoAlt = 0;
      updatelfoAlt();
      break;

      // case CClfoMult:
      //   updatelfoMult();
      //   break;

      // case CCfilterVelSW:
      //   value > 0 ? filterVelSW = 1 : filterVelSW = 0;
      //   updatefilterVelSW();
      //   break;

      // case CCampVelSW:
      //   value > 0 ? ampVelSW = 1 : ampVelSW = 0;
      //   updateampVelSW();
      //   break;

      // case CCFilterLoop:
      //   updateFilterLoop();
      //   break;

      // case CCAmpLoop:
      //   updateAmpLoop();
      //   break;

      // case CCAmpGatedSW:
      //   value > 0 ? AmpGatedSW = 1 : AmpGatedSW = 0;
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
      updatekeyboardMode();
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

void onButtonPress(uint16_t btnIndex, uint8_t btnType) {

  if (btnIndex == GLIDE_SW && btnType == ROX_PRESSED) {
    glideSW = !glideSW;
    myControlChange(midiChannel, CCglideSW, glideSW);
  }

  if (btnIndex == FILTER_POLE_SW && btnType == ROX_PRESSED) {
    filterPoleSW = !filterPoleSW;
    myControlChange(midiChannel, CCfilterPoleSW, filterPoleSW);
  }

  if (btnIndex == EG_INVERT_SW && btnType == ROX_PRESSED) {
    filterEGinv = !filterEGinv;
    myControlChange(midiChannel, CCfilterEGinv, filterEGinv);
  }

  if (btnIndex == DCO1_OCT_SW && btnType == ROX_PRESSED) {
    oct1 = oct1 + 1;
    if (oct1 > 2) {
      oct1 = 0;
    }
    myControlChange(midiChannel, CCosc1Oct, oct1);
  }

  if (btnIndex == DCO2_OCT_SW && btnType == ROX_PRESSED) {
    oct2 = oct2 + 1;
    if (oct2 > 2) {
      oct2 = 0;
    }
    myControlChange(midiChannel, CCosc2Oct, oct2);
  }

  if (btnIndex == FILTER_TYPE_SW && btnType == ROX_PRESSED) {
    filterType = filterType + 1;
    if (filterType > 7) {
      filterType = 0;
    }
    myControlChange(midiChannel, CCfilterType, filterType);
  }

  if (btnIndex == LFO_ALT_SW && btnType == ROX_PRESSED) {
    lfoAlt = !lfoAlt;
    myControlChange(midiChannel, CClfoAlt, lfoAlt);
  }

  if (btnIndex == PW_LFO_WAVEFORM_SW && btnType == ROX_PRESSED) {
    pwLFOwaveformSW = pwLFOwaveformSW + 1;
    if (pwLFOwaveformSW > 7) {
      pwLFOwaveformSW = 0;
    }
    myControlChange(midiChannel, CCpwLFOwaveformSW, pwLFOwaveformSW);
  }

  if (btnIndex == LFO_WAVEFORM_SW && btnType == ROX_PRESSED) {
    LFOWaveform = LFOWaveform + 1;
    if (LFOWaveform > 7) {
      LFOWaveform = 0;
    }
    myControlChange(midiChannel, CCLFOWaveform, LFOWaveform);
  }

  if (btnIndex == FILTER_ENV_VELOCITY_SW && btnType == ROX_PRESSED) {
    filterVelSW = !filterVelSW;
    myControlChange(midiChannel, CCfilterVel, filterVelSW);
  }

  if (btnIndex == AMP_ENV_VELOCITY_SW && btnType == ROX_PRESSED) {
    ampVelSW = !ampVelSW;
    myControlChange(midiChannel, CCvcaVel, ampVelSW);
  }

  if (btnIndex == FILTER_ENV_LOOP_SW && btnType == ROX_PRESSED) {
    FilterLoop = FilterLoop + 1;
    if (FilterLoop > 2) {
      FilterLoop = 0;
    }
    myControlChange(midiChannel, CCFilterLoop, FilterLoop);
  }

  if (btnIndex == AMP_ENV_LOOP_SW && btnType == ROX_PRESSED) {
    AmpLoop = AmpLoop + 1;
    if (AmpLoop > 2) {
      AmpLoop = 0;
    }
    myControlChange(midiChannel, CCAmpLoop, AmpLoop);
  }

  if (btnIndex == AMP_GATED_SW && btnType == ROX_PRESSED) {
    AmpGatedSW = !AmpGatedSW;
    myControlChange(midiChannel, CCAmpGatedSW, AmpGatedSW);
  }

  if (btnIndex == EFFECT_NUMBER_SW && btnType == ROX_PRESSED) {
    effectNumSW = effectNumSW + 1;
    if (effectNumSW > 7) {
      effectNumSW = 0;
    }
    myControlChange(midiChannel, CCeffectNumSW, effectNumSW);
  }

  if (btnIndex == EFFECT_BANK_SW && btnType == ROX_PRESSED) {
    effectBankSW = effectBankSW + 1;
    if (effectBankSW > 3) {
      effectBankSW = 0;
    }
    myControlChange(midiChannel, CCeffectBankSW, effectBankSW);
  }

  if (btnIndex == FILTER_ENV_LIN_LOG_SW && btnType == ROX_PRESSED) {
    filterenvLinLogSW = !filterenvLinLogSW;
    myControlChange(midiChannel, CCfilterenvLinLogSW, filterenvLinLogSW);
  }

  if (btnIndex == AMP_ENV_LIN_LOG_SW && btnType == ROX_PRESSED) {
    ampenvLinLogSW = !ampenvLinLogSW;
    myControlChange(midiChannel, CCampenvLinLogSW, ampenvLinLogSW);
  }

  if (btnIndex == POLY1_SW && btnType == ROX_PRESSED) {
    keyboardMode = 0;
    myControlChange(midiChannel, CCkeyboardMode, keyboardMode);
  }

  if (btnIndex == POLY2_SW && btnType == ROX_PRESSED) {
    keyboardMode = 1;
    myControlChange(midiChannel, CCkeyboardMode, keyboardMode);
  }

  if (btnIndex == UNISON_SW && btnType == ROX_PRESSED) {
    keyboardMode = 2;
    myControlChange(midiChannel, CCkeyboardMode, keyboardMode);
  }

  if (btnIndex == MONO_SW && btnType == ROX_PRESSED) {
    keyboardMode = 3;
    myControlChange(midiChannel, CCkeyboardMode, keyboardMode);
  }

  if (btnIndex == KEYBOARD_SW && btnType == ROX_PRESSED) {
    playMode = playMode + 1;
    if (playMode > 2) {
      playMode = 0;
    }
    myControlChange(midiChannel, CCplayMode, playMode);
  }

  if (btnIndex == PRIORITY_SW && btnType == ROX_PRESSED) {
    NotePriority = NotePriority + 1;
    if (NotePriority > 2) {
      NotePriority = 0;
    }
    myControlChange(midiChannel, CCNotePriority, NotePriority);
  }

  if (btnIndex == LFO_MULTI_MONO_SW && btnType == ROX_PRESSED) {
    monoMultiSW = !monoMultiSW;
    myControlChange(midiChannel, CCmonoMulti, monoMultiSW);
  }

  if (btnIndex == CHORD_HOLD_SW && btnType == ROX_PRESSED) {
    chordHoldSW = !chordHoldSW;
    myControlChange(midiChannel, CCchordHoldSW, chordHoldSW);
  }

  if (btnIndex == SYNC_SW && btnType == ROX_PRESSED) {
    syncSW = !syncSW;
    myControlChange(midiChannel, CCsyncSW, syncSW);
  }

  if (btnIndex == LOWER_SW && btnType == ROX_PRESSED) {
    lowerSW = 1;
    upperSW = 0;
    myControlChange(midiChannel, CClowerSW, lowerSW);
  }

  if (btnIndex == UPPER_SW && btnType == ROX_PRESSED) {
    lowerSW = 0;
    upperSW = 1;
    myControlChange(midiChannel, CCupperSW, upperSW);
  }

  if (btnIndex == PM_DCO1_DEST_SW && btnType == ROX_PRESSED) {
    pmDestDCO1SW = !pmDestDCO1SW;
    myControlChange(midiChannel, CCpmDestDCO1SW, pmDestDCO1SW);
  }

  if (btnIndex == PM_FILT_ENV_DEST_SW && btnType == ROX_PRESSED) {
    pmDestFilterSW = !pmDestFilterSW;
    myControlChange(midiChannel, CCpmDestFilterSW, pmDestFilterSW);
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

  octoswitch.update();  // read all the buttons for the Synth
  srp.update();         // update all the LEDs in the buttons
}
