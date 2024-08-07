#include "TeensyThreads.h"
#include "ScreenParams.h"

#define DISPLAYTIMEOUT 1500

#include <Fonts/Org_01.h>
#include "Yeysk16pt7b.h"
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansOblique24pt7b.h>
#include <Fonts/FreeSansBoldOblique24pt7b.h>

#define PULSE 1
#define VAR_TRI 2
#define FILTER_ENV 3
#define AMP_ENV1 4
#define AMP_ENV2 5

#define NUM_STEPS 32
#define STEP_HEIGHT 5   // Height of each step in pixels
#define BAR_WIDTH 24    // Width of each bar
#define BAR_SPACING 46  // Distance between the start of each bar

#define DET_STEPS 12        // Number of steps for the detune bar
#define DET_STEP_HEIGHT 13  // Height of each step for the detune bar (210 / 16 ≈ 13)
#define BAR_HEIGHT 160      // Total height for the bar indicators

String currentParameter = "";
String currentValue = "";
float currentFloatValue = 0.0;
String currentPgmNum = "";
String currentPatchName = "";
String newPatchName = "";
const char *currentSettingsOption = "";
const char *currentSettingsValue = "";
int currentSettingsPart = SETTINGS;
int paramType = PARAMETER;


// Screen dimensions
int screenWidth = 320;
int screenHeight = 140;

boolean MIDIClkSignal = false;

unsigned long timer = 0;

void startTimer() {
}

void renderBootUpPage() {
}

int mapValue(int value, int max_value, int scale) {
  return (value * scale) / max_value;
}

// Function to draw the bar
void drawBar0(int x, int value, int steps, int stepHeight) {
  int filledSteps = value / (1023 / steps);  // Calculate the number of steps to fill
  for (int i = 0; i < steps; i++) {
    int y = 210 - (i * stepHeight);
    if (i < filledSteps) {
      tft0.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_YELLOW);  // Filled step
    } else {
      tft0.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_BLACK);  // Unfilled step
    }
  }
}

// Function to draw the bar
void drawBar1(int x, int value, int steps, int stepHeight) {
  int filledSteps = value / (1023 / steps);  // Calculate the number of steps to fill
  for (int i = 0; i < steps; i++) {
    int y = 210 - (i * stepHeight);
    if (i < filledSteps) {
      tft1.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_YELLOW);  // Filled step
    } else {
      tft1.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_BLACK);  // Unfilled step
    }
  }
}

// Function to draw the bar
void drawBar2(int x, int value, int steps, int stepHeight) {
  int filledSteps = value / (1023 / steps);  // Calculate the number of steps to fill
  for (int i = 0; i < steps; i++) {
    int y = 210 - (i * stepHeight);
    if (i < filledSteps) {
      tft2.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_YELLOW);  // Filled step
    } else {
      tft2.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_BLACK);  // Unfilled step
    }
  }
}

// Function to draw the bar
void drawBar3(int x, int value, int steps, int stepHeight) {
  int filledSteps = value / (1023 / steps);  // Calculate the number of steps to fill
  for (int i = 0; i < steps; i++) {
    int y = 210 - (i * stepHeight);
    if (i < filledSteps) {
      tft3.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_YELLOW);  // Filled step
    } else {
      tft3.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_BLACK);  // Unfilled step
    }
  }
}

// Function to draw the bar
void drawBar6(int x, int value, int steps, int stepHeight) {
  int filledSteps = value / (1023 / steps);  // Calculate the number of steps to fill
  for (int i = 0; i < steps; i++) {
    int y = 210 - (i * stepHeight);
    if (i < filledSteps) {
      tft6.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_YELLOW);  // Filled step
    } else {
      tft6.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_BLACK);  // Unfilled step
    }
  }
}

// Function to draw the bar
void drawBar7(int x, int value, int steps, int stepHeight) {
  int filledSteps = value / (1023 / steps);  // Calculate the number of steps to fill
  for (int i = 0; i < steps; i++) {
    int y = 210 - (i * stepHeight);
    if (i < filledSteps) {
      tft7.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_YELLOW);  // Filled step
    } else {
      tft7.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_BLACK);  // Unfilled step
    }
  }
}


void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
  tft4.fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

void drawPWIndicator1(int x, int value) {
  // Clear the entire bar area by drawing a rectangle with the background color
  tft1.fillRoundRect(x, 208 - BAR_HEIGHT, BAR_WIDTH, BAR_HEIGHT, 2, ST7735_BLACK);
  tft1.drawFastVLine(18, 50, 155, ST7735_RED);
  tft1.drawFastHLine(6, 50, 24, ST7735_RED);
  tft1.drawFastHLine(6, 127, 24, ST7735_RED);
  tft1.drawFastHLine(6, 205, 24, ST7735_RED);
  // Calculate the y position for the current value
  int y = 205 - ((value * (BAR_HEIGHT - STEP_HEIGHT)) / 1023);

  // Draw the new bar at the calculated position
  tft1.fillRoundRect(x, y - STEP_HEIGHT / 2, BAR_WIDTH, STEP_HEIGHT, 2, ST7735_YELLOW);
}

void drawPWIndicator2(int x, int value) {
  // Clear the entire bar area by drawing a rectangle with the background color
  tft2.fillRoundRect(x, 208 - BAR_HEIGHT, BAR_WIDTH, BAR_HEIGHT, 2, ST7735_BLACK);
  tft2.drawFastVLine(18, 50, 155, ST7735_RED);
  tft2.drawFastHLine(6, 50, 24, ST7735_RED);
  tft2.drawFastHLine(6, 127, 24, ST7735_RED);
  tft2.drawFastHLine(6, 205, 24, ST7735_RED);
  // Calculate the y position for the current value
  int y = 205 - ((value * (BAR_HEIGHT - STEP_HEIGHT)) / 1023);

  // Draw the new bar at the calculated position
  tft2.fillRoundRect(x, y - STEP_HEIGHT / 2, BAR_WIDTH, STEP_HEIGHT, 2, ST7735_YELLOW);
}

void renderFilterPoints() {
  maxSectionWidth = (screenWidth - 80) / 3;  // Limit each section to a third of the available width minus the sustain
  attackX = mapValue(filterAttack, 1023, maxSectionWidth);
  decayX = attackX + mapValue(filterDecay, 1023, maxSectionWidth);
  sustainLevel = screenHeight - mapValue(filterSustain, 1023, screenHeight);
  sustainX = decayX + 80;                                                // Fixed length sustain bar of 80 pixels
  releaseX = sustainX + mapValue(filterRelease, 1023, maxSectionWidth);  // Release limited to a third of the screen width

  // Ensure release does not go beyond screen width
  if (releaseX > screenWidth) {
    releaseX = screenWidth;
  }

  // Define points for the ADSR envelope
  pointsX[0] = 0;
  pointsX[1] = attackX;
  pointsX[2] = decayX;
  pointsX[3] = sustainX;
  pointsX[4] = releaseX;
  pointsX[5] = screenWidth;

  pointsY[0] = screenHeight + offsetY;
  pointsY[1] = offsetY;
  pointsY[2] = sustainLevel + offsetY;
  pointsY[3] = sustainLevel + offsetY;
  pointsY[4] = screenHeight + offsetY;
  pointsY[5] = screenHeight + offsetY;
}

void renderAmpPoints() {
  maxSectionWidth = (screenWidth - 80) / 3;  // Limit each section to a third of the available width minus the sustain
  AmpattackX = mapValue(ampAttack, 1023, maxSectionWidth);
  AmpdecayX = AmpattackX + mapValue(ampDecay, 1023, maxSectionWidth);
  AmpsustainLevel = screenHeight - mapValue(ampSustain, 1023, screenHeight);
  AmpsustainX = AmpdecayX + 80;                                             // Fixed length sustain bar of 80 pixels
  AmpreleaseX = AmpsustainX + mapValue(ampRelease, 1023, maxSectionWidth);  // Release limited to a third of the screen width

  // Ensure release does not go beyond screen width
  if (AmpreleaseX > screenWidth) {
    AmpreleaseX = screenWidth;
  }

  // Define points for the ADSR envelope
  AmppointsX[0] = 0;
  AmppointsX[1] = AmpattackX;
  AmppointsX[2] = AmpdecayX;
  AmppointsX[3] = AmpsustainX;
  AmppointsX[4] = AmpreleaseX;
  AmppointsX[5] = screenWidth;

  AmppointsY[0] = screenHeight + offsetY;
  AmppointsY[1] = offsetY;
  AmppointsY[2] = AmpsustainLevel + offsetY;
  AmppointsY[3] = AmpsustainLevel + offsetY;
  AmppointsY[4] = screenHeight + offsetY;
  AmppointsY[5] = screenHeight + offsetY;
}


void renderCurrentPatchPage(int parameter) {

  parameterGroup = parameter;

  switch (parameterGroup) {

    case 0:  // System
      // ************************ DISPLAY 0 **************************
      tft0.fillScreen(ST7735_BLACK);  // Fill the screen with black
      tft0.setFont(&FreeSans9pt7b);
      tft0.setTextColor(ST7735_WHITE);
      tft0.setTextSize(1);

      // Setting text labels at the bottom of each bar
      tft0.setCursor(2, 220);
      tft0.print("GLD");

      tft0.setFont(&FreeSans12pt7b);

      // glide indicator box along the top
      if (glideSW == 0) {
        tft0.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft0.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft0.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft0.setCursor(30, 18);
      tft0.print(glideSW == 0 ? "Glide Off" : "Glide On");

      // chord Hold indicator box along the top
      if (chordHoldSW == 0) {
        tft0.fillRoundRect(180, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft0.fillRoundRect(180, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft0.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft0.setCursor(195, 18);
      tft0.print(chordHoldSW == 0 ? "Hold Off" : "Hold On");

      // upper/lower indicator box along the top
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

      tft0.fillRoundRect(50, 130, 120, 30, 5, ST7735_CYAN);  // Cyan box
      tft0.setCursor(60, 138);
      tft0.print("Priority");
      tft0.fillRoundRect(180, 130, 130, 30, 5, ST7735_YELLOW);  // Yellow box
      switch (NotePriority) {
        case 0:
          tft0.setCursor(195, 138);
          tft0.print("First");
          break;

        case 1:
          tft0.setCursor(195, 138);
          tft0.print("Bottom");
          break;

        case 2:
          tft0.setCursor(195, 138);
          tft0.print("Last");
          break;
      }

      tft0.fillRoundRect(50, 170, 120, 30, 5, ST7735_CYAN);  // Cyan box
      tft0.setCursor(60, 178);
      tft0.print("Keyboard");
      tft0.fillRoundRect(180, 170, 130, 30, 5, ST7735_YELLOW);  // Yellow box
      switch (keyboardMode) {
        case 0:
          tft0.setCursor(195, 178);
          tft0.print("Whole");
          break;

        case 1:
          tft0.setCursor(195, 178);
          tft0.print("Dual");
          break;

        case 2:
          tft0.setCursor(195, 178);
          tft0.print("Split");
          break;
      }

      tft0.fillRoundRect(50, 210, 120, 30, 5, ST7735_CYAN);  // Cyan box
      tft0.setCursor(60, 218);
      tft0.print("Key Mode");
      tft0.fillRoundRect(180, 210, 130, 30, 5, ST7735_YELLOW);  // Yellow box
      switch (playMode) {
        case 0:
          tft0.setCursor(195, 218);
          tft0.print("Poly 1");
          break;

        case 1:
          tft0.setCursor(195, 218);
          tft0.print("Poly 2");
          break;

        case 2:
          tft0.setCursor(195, 218);
          tft0.print("Unison");
          break;

        case 3:
          tft0.setCursor(195, 218);
          tft0.print("Mono");
          break;
      }

      drawBar0(6, glideTime, NUM_STEPS, STEP_HEIGHT);
      // tft0.setFont(&FreeSans12pt7b);
      // tft0.fillScreen(ST7735_BLACK);
      // tft0.setCursor(0, 5);
      // tft0.setTextSize(3);
      // tft0.setTextColor(ST7735_YELLOW);
      // tft0.println(currentPgmNum);
      // tft0.setCursor(80, 15);
      // tft0.setFont(&FreeSans9pt7b);
      // tft0.setTextSize(2);
      // tft0.setTextColor(ST7735_WHITE);
      // tft0.println(currentPatchName);
      // tft0.setFont(&FreeSans12pt7b);
      // tft0.setTextSize(1);
      // tft0.drawFastHLine(0, 58, tft0.width(), ST7735_RED);
      // tft0.setTextColor(ST7735_WHITE);

      // tft0.setTextColor(ST7735_WHITE);
      // tft0.setTextSize(1);
      // tft0.setCursor(0, 70);
      // tft0.print("Glide Time");
      // tft0.setCursor(0, 100);
      // tft0.print("Glide");
      // tft0.setCursor(0, 130);
      // tft0.print("Key Mode");
      // tft0.setCursor(0, 160);
      // tft0.print("Note Priority");
      // tft0.setCursor(0, 190);
      // tft0.print("OSC1 Coarse");
      // tft0.setCursor(0, 220);
      // tft0.print("OSC2 Fine");
      // tft0.setTextColor(ST7735_YELLOW);
      // tft0.setCursor(160, 100);
      // switch (glideSW) {
      //   case 0:
      //     tft0.print("Off");
      //     break;
      //   case 1:
      //     tft0.print("On");
      //     break;
      // }
      // tft0.setCursor(160, 130);
      // switch (keyboardMode) {
      //   case 0:
      //     tft0.print("Poly 1");
      //     break;
      //   case 1:
      //     tft0.print("Poly 2");
      //     break;
      //   case 2:
      //     tft0.print("Mono");
      //     break;
      //   case 3:
      //     tft0.print("Unison");
      //     break;
      // }
      // tft0.setCursor(160, 160);
      // switch (NotePriority) {
      //   case 0:
      //     tft0.print("Top Note");
      //     break;
      //   case 1:
      //     tft0.print("Bottom Note");
      //     break;
      //   case 2:
      //     tft0.print("Last Note");
      //     break;
      // }

      // tft0.fillRoundRect(160, 68, int((glideTime * 8) / 6.5), 16, 2, ST7735_YELLOW);
      //tft0.updateScreen();
      break;



    case 1:  // Oscillator 1
      // ************************ DISPLAY 1 **************************
      tft1.fillScreen(ST7735_BLACK);  // Fill the screen with black
      tft1.setFont(&FreeSans9pt7b);
      tft1.setTextColor(ST7735_WHITE);
      tft1.setTextSize(1);

      // Setting text labels at the bottom of each bar
      tft1.setCursor(4, 220);
      tft1.print("PW");
      tft1.setCursor(43, 220);
      tft1.print("PWM");
      tft1.setCursor(96, 220);
      tft1.print("FM");

      tft1.setCursor(184, 220);
      tft1.print("Saw");
      tft1.setCursor(234, 220);
      tft1.print("Sqr");
      tft1.setCursor(280, 220);
      tft1.print("Sub");

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
      // Drawing the PW lines
      tft1.drawFastVLine(18, 50, 155, ST7735_RED);
      tft1.drawFastHLine(6, 50, 24, ST7735_RED);
      tft1.drawFastHLine(6, 127, 24, ST7735_RED);
      tft1.drawFastHLine(6, 205, 24, ST7735_RED);

      // Drawing the bars
      drawPWIndicator1(6, osc1PW);
      drawBar1(52, osc1PWM, NUM_STEPS, STEP_HEIGHT);
      drawBar1(98, oscfmDepth, NUM_STEPS, STEP_HEIGHT);

      drawBar1(192, osc1sawLevel, NUM_STEPS, STEP_HEIGHT);
      drawBar1(236, osc1pulseLevel, NUM_STEPS, STEP_HEIGHT);
      drawBar1(282, osc1subLevel, NUM_STEPS, STEP_HEIGHT);

      //tft1.updateScreen();
      break;

    case 2:  // oscillator 2
      // ************************ DISPLAY 2 *************************
      tft2.fillScreen(ST7735_BLACK);  // Fill the screen with black
      tft2.setFont(&FreeSans9pt7b);
      tft2.setTextColor(ST7735_WHITE);
      tft2.setTextSize(1);

      // Setting text labels at the bottom of each bar
      tft2.setCursor(4, 220);
      tft2.print("PW");
      tft2.setCursor(43, 220);
      tft2.print("PWM");
      tft2.setCursor(96, 220);
      tft2.print("Det");
      tft2.setCursor(146, 220);
      tft2.print("Int");
      tft2.setCursor(184, 220);
      tft2.print("Saw");
      tft2.setCursor(234, 220);
      tft2.print("Sqr");
      tft2.setCursor(284, 220);
      tft2.print("Tri");

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

      // Sync indicator box along the top
      if (syncSW == 0) {
        tft2.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft2.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft2.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft2.setCursor(30, 18);
      tft2.print(syncSW == 0 ? "Sync Off" : "Sync On");

      // Drawing the PW lines
      tft2.drawFastVLine(18, 50, 155, ST7735_RED);
      tft2.drawFastHLine(6, 50, 24, ST7735_RED);
      tft2.drawFastHLine(6, 127, 24, ST7735_RED);
      tft2.drawFastHLine(6, 205, 24, ST7735_RED);

      // Drawing the bars
      drawPWIndicator2(6, osc2PW);
      drawBar2(52, osc2PWM, NUM_STEPS, STEP_HEIGHT);
      drawBar2(98, osc2detune, NUM_STEPS, STEP_HEIGHT);
      drawBar2(144, osc2interval, DET_STEPS, DET_STEP_HEIGHT);
      drawBar2(192, osc2sawLevel, NUM_STEPS, STEP_HEIGHT);
      drawBar2(236, osc2pulseLevel, NUM_STEPS, STEP_HEIGHT);
      drawBar2(282, osc2triLevel, NUM_STEPS, STEP_HEIGHT);

      //tft2.updateScreen();
      break;

    case 3:                           // filter
                                      // ************************ DISPLAY 3 **************************
      tft3.fillScreen(ST7735_BLACK);  // Fill the screen with black
      tft3.setFont(&FreeSans9pt7b);
      tft3.setTextColor(ST7735_WHITE);
      tft3.setTextSize(1);

      // Setting text labels at the bottom of each bar
      tft3.setCursor(2, 220);
      tft3.print("Cut");
      tft3.setCursor(48, 220);
      tft3.print("Res");
      tft3.setCursor(98, 220);
      tft3.print("EG");
      tft3.setCursor(140, 220);
      tft3.print("Key");
      tft3.setCursor(190, 220);
      tft3.print("TM");

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

      // Drawing the bars
      drawBar3(6, filterCutoff, NUM_STEPS, STEP_HEIGHT);
      drawBar3(52, filterRes, NUM_STEPS, STEP_HEIGHT);
      drawBar3(98, filterEGlevel, NUM_STEPS, STEP_HEIGHT);
      drawBar3(144, keyTrack, NUM_STEPS, STEP_HEIGHT);
      drawBar3(192, filterLFO, NUM_STEPS, STEP_HEIGHT);

      //tft3.updateScreen();
      break;

    case 4:  // filter ADSR
      // ************************ DISPLAY 4 **************************
      tft4.fillScreen(ST7735_BLACK);
      tft4.setTextColor(ST7735_WHITE);
      tft4.setFont(&FreeSans12pt7b);
      tft4.setTextSize(1);

      if (filterVelSW == 0) {
        tft4.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft4.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft4.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft4.setCursor(30, 18);
      tft4.print(filterVelSW == 0 ? "Vel Off" : "Vel On");

      if (filterEGinv == 0) {
        tft4.fillRoundRect(180, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft4.fillRoundRect(180, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft4.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft4.setCursor(195, 18);
      tft4.print(filterEGinv == 0 ? "EG Pos" : "Eg Neg");

      if (filterenvLinLogSW == 0) {
        tft4.fillRoundRect(10, 50, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft4.fillRoundRect(10, 50, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft4.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft4.setCursor(30, 58);
      tft4.print(filterenvLinLogSW == 0 ? "Env Lin" : "Env Log");


      tft4.fillRoundRect(180, 50, 130, 30, 5, ST7735_YELLOW);  // Green box for off

      tft4.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft4.setCursor(195, 58);
      switch (FilterLoop) {
        case 0:
          tft4.setCursor(195, 58);
          tft4.print("Loop Off");
          break;
        case 1:
          tft4.setCursor(183, 58);
          tft4.print("Loop Gated");
          break;
        case 2:
          tft4.setCursor(195, 58);
          tft4.print("Loop LFO");
          break;
      }

      // Fill the ADSR envelope with triangles
      for (int i = 0; i < 4; i++) {
        fillTriangle(pointsX[i], pointsY[i], pointsX[i + 1], pointsY[i + 1], pointsX[i + 1], screenHeight + offsetY, ST7735_YELLOW);
        fillTriangle(pointsX[i], pointsY[i], pointsX[i], screenHeight + offsetY, pointsX[i + 1], screenHeight + offsetY, ST7735_YELLOW);
      }

      // Draw the ADSR envelope with thicker lines
      for (int i = 0; i < 4; i++) {
        tft4.drawLine(pointsX[i], pointsY[i], pointsX[i + 1], pointsY[i + 1], ST7735_YELLOW);
        tft4.drawLine(pointsX[i], pointsY[i] + 1, pointsX[i + 1], pointsY[i + 1] + 1, ST7735_YELLOW);
        tft4.drawLine(pointsX[i], pointsY[i] - 1, pointsX[i + 1], pointsY[i + 1] - 1, ST7735_YELLOW);
      }

      //tft4.updateScreen();
      break;

    case 5:  // amp ADSR
      // ************************ DISPLAY 5 **************************

      tft5.fillScreen(ST7735_BLACK);
      tft5.setTextColor(ST7735_WHITE);
      tft5.setFont(&FreeSans12pt7b);
      tft5.setTextSize(1);

      if (ampVelSW == 0) {
        tft5.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft5.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft5.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft5.setCursor(30, 18);
      tft5.print(filterVelSW == 0 ? "Vel Off" : "Vel On");

      if (AmpGatedSW == 0) {
        tft5.fillRoundRect(180, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft5.fillRoundRect(180, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft5.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft5.setCursor(195, 18);
      tft5.print(AmpGatedSW == 0 ? "Gate Off" : "Gate On");

      if (ampenvLinLogSW == 0) {
        tft5.fillRoundRect(10, 50, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft5.fillRoundRect(10, 50, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft5.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft5.setCursor(30, 58);
      tft5.print(filterenvLinLogSW == 0 ? "Env Lin" : "Env Log");


      tft5.fillRoundRect(180, 50, 130, 30, 5, ST7735_YELLOW);  // Green box for off

      tft5.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft5.setCursor(195, 58);
      switch (AmpLoop) {
        case 0:
          tft5.setCursor(195, 58);
          tft5.print("Loop Off");
          break;
        case 1:
          tft5.setCursor(183, 58);
          tft5.print("Loop Gated");
          break;
        case 2:
          tft5.setCursor(195, 58);
          tft5.print("Loop LFO");
          break;
      }

      // Fill the ADSR envelope with triangles
      for (int i = 0; i < 4; i++) {
        fillTriangle(AmppointsX[i], AmppointsY[i], AmppointsX[i + 1], AmppointsY[i + 1], AmppointsX[i + 1], screenHeight + offsetY, ST7735_YELLOW);
        fillTriangle(AmppointsX[i], AmppointsY[i], AmppointsX[i], screenHeight + offsetY, AmppointsX[i + 1], screenHeight + offsetY, ST7735_YELLOW);
      }

      // Draw the ADSR envelope with thicker lines
      for (int i = 0; i < 4; i++) {
        tft5.drawLine(AmppointsX[i], AmppointsY[i], AmppointsX[i + 1], AmppointsY[i + 1], ST7735_YELLOW);
        tft5.drawLine(AmppointsX[i], AmppointsY[i] + 1, AmppointsX[i + 1], AmppointsY[i + 1] + 1, ST7735_YELLOW);
        tft5.drawLine(AmppointsX[i], AmppointsY[i] - 1, AmppointsX[i + 1], AmppointsY[i + 1] - 1, ST7735_YELLOW);
      }

      //tft5.updateScreen();
      break;

    case 6:  // LFO
      // ************************ DISPLAY 6 **************************
      tft6.fillScreen(ST7735_BLACK);  // Fill the screen with black
      tft6.setFont(&FreeSans9pt7b);
      tft6.setTextColor(ST7735_WHITE);
      tft6.setTextSize(1);

      // Setting text labels at the bottom of each bar
      tft6.setCursor(4, 220);
      tft6.print("LFO");
      tft6.setCursor(48, 220);
      tft6.print("Dly");
      tft6.setCursor(96, 220);
      tft6.print("PW");
      tft6.setCursor(142, 220);
      tft6.print("MW");

      tft6.setFont(&FreeSans12pt7b);
      // Set range label and value inside a box along the top
      tft6.fillRoundRect(240, 10, 70, 30, 5, ST7735_YELLOW);  // Background box for range
      tft6.setTextColor(ST7735_BLACK);
      tft6.setCursor(260, 18);
      switch (lfoMult) {
        case 0:
          tft6.print("x0.5");
          break;
        case 1:
          tft6.print("x1.0");
          break;
        case 2:
          tft6.print("x1.5");
          break;
      }

      switch (lfoAlt) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(lfo02[LFOWaveform])));
          break;
        case 1:
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

      if (lfoAlt == 1) {
        tft6.fillRoundRect(180, 50, 130, 30, 5, ST7735_RED);
      } else {
        tft6.fillRoundRect(180, 50, 130, 30, 5, ST7735_GREEN);
      }
      tft6.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft6.setCursor(195, 58);
      tft6.print(lfoAlt == 0 ? "Alt Off" : "Alt On");

      // Drawing the bars
      drawBar6(6, LFORate, NUM_STEPS, STEP_HEIGHT);
      drawBar6(52, lfoDelay, NUM_STEPS, STEP_HEIGHT);
      drawBar6(98, PWRate, NUM_STEPS, STEP_HEIGHT);
      drawBar6(144, modWheelDepth, NUM_STEPS, STEP_HEIGHT);

      // tft6.setCursor(160, 70);
      // tft6.setTextColor(ST7735_YELLOW);
      // tft6.print(lfoDisplay);
      //tft6.updateScreen();

      // tft6.setFont(&FreeSans12pt7b);
      // tft6.fillScreen(ST7735_BLACK);
      // tft6.setCursor(0, 5);
      // tft6.setTextSize(3);
      // tft6.setTextColor(ST7735_YELLOW);
      // tft6.println(currentPgmNum);
      // tft6.setCursor(80, 15);
      // tft6.setFont(&FreeSans9pt7b);
      // tft6.setTextSize(2);
      // tft6.setTextColor(ST7735_WHITE);
      // tft6.println(currentPatchName);
      // tft6.setFont(&FreeSans12pt7b);
      // tft6.setTextSize(1);
      // tft6.drawFastHLine(0, 58, tft4.width(), ST7735_RED);
      // tft6.setTextColor(ST7735_WHITE);

      // char lfoDisplay[30];
      // switch (lfoAlt) {
      //   case 0:
      //     str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(lfo02[LFOWaveform])));
      //     break;
      //   case 1:
      //     str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(lfo01[LFOWaveform])));
      //     break;
      // }
      // // Check if the pointer is valid
      // if (str_ptr != nullptr) {
      //   // Copy the string from program memory to RAM
      //   strcpy_P(lfoDisplay, str_ptr);
      // } else {
      //   // Handle the case where the pointer is NULL (if needed)
      // }
      // tft6.setTextSize(1);
      // tft6.setCursor(0, 70);
      // tft6.print("LFO Wave");
      // tft6.setCursor(160, 70);
      // tft6.setTextColor(ST7735_YELLOW);
      // tft6.print(lfoDisplay);
      // tft6.setTextColor(ST7735_WHITE);
      // tft6.setCursor(0, 100);
      // tft6.print("LFO Rate");
      // tft6.setCursor(0, 130);
      // tft6.print("LFO Delay");
      // tft6.setCursor(0, 160);
      // tft6.print("LFO Multiplier");
      // tft6.setCursor(0, 190);
      // tft6.print("Multi Trigger");
      // tft6.setCursor(0, 220);
      // tft6.print("FM Mod Sync");
      // tft6.setTextColor(ST7735_YELLOW);
      // tft6.setCursor(160, 160);
      // switch (lfoMult) {
      //   case 0:
      //     tft6.print("x 0.5");
      //     break;
      //   case 1:
      //     tft6.print("x 1.0");
      //     break;
      //   case 2:
      //     tft6.print("x 2.0");
      //     break;
      // }
      // tft6.setCursor(160, 190);
      // switch (monoMultiSW) {
      //   case 0:
      //     tft6.print("Off");
      //     break;
      //   case 1:
      //     tft6.print("On");
      //     break;
      // }
      // tft6.setCursor(160, 220);
      // switch (oscfmDepth) {
      //   case 0:
      //     tft6.print("Off");
      //     break;
      //   case 1:
      //     tft6.print("On");
      //     break;
      // }

      // tft6.fillRoundRect(160, 98, int(LFORate / 6.5), 16, 2, ST7735_YELLOW);
      // tft6.fillRoundRect(160, 128, int(lfoDelay / 6.5), 16, 2, ST7735_YELLOW);
      // tft6.updateScreen();
      break;

    case 7:  // Effects
             // ************************ DISPLAY 7 **************************
      tft7.setFont(&FreeSans12pt7b);
      tft7.fillScreen(ST7735_BLACK);
      tft7.setCursor(0, 5);
      tft7.setTextSize(3);
      tft7.setTextColor(ST7735_YELLOW);
      tft7.println(currentPgmNum);
      tft7.setCursor(80, 15);
      tft7.setFont(&FreeSans9pt7b);
      tft7.setTextSize(2);
      tft7.setTextColor(ST7735_WHITE);
      tft7.println(currentPatchName);
      tft7.setFont(&FreeSans12pt7b);
      tft7.setTextSize(1);
      tft7.drawFastHLine(0, 58, tft7.width(), ST7735_RED);

      tft7.setTextColor(ST7735_WHITE);
      tft7.setTextSize(1);
      tft7.setCursor(0, 70);  //effect mix
      tft7.print("Effect:");
      tft7.setCursor(80, 70);
      tft7.setTextColor(ST7735_YELLOW);
      char buf1[30];  // first word of effect name
      switch (effectBankSW) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name01[effectNumSW])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name11[effectNumSW])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name21[effectNumSW])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name31[effectNumSW])));
          break;
      }

      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf1, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }
      tft7.setTextSize(1);
      tft7.print(buf1);
      tft7.print(" ");

      char buf2[30];  // second word of effect name
      switch (effectBankSW) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name02[effectNumSW])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name12[effectNumSW])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name22[effectNumSW])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name32[effectNumSW])));
          break;
      }

      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf2, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }
      tft7.print(buf2);
      tft7.setTextSize(1);
      tft7.setTextColor(ST7735_WHITE);
      tft7.setCursor(0, 100);
      tft7.print("Bank:");
      tft7.setTextColor(ST7735_YELLOW);
      tft7.setCursor(80, 100);
      tft7.print(effectBankSW + 1);
      tft7.setTextColor(ST7735_WHITE);
      tft7.setCursor(160, 100);
      tft7.print("Number:");
      tft7.setTextColor(ST7735_YELLOW);
      tft7.setCursor(270, 100);
      tft7.print(effectNumSW + 1);
      tft7.setTextSize(1);
      tft7.setTextColor(ST7735_WHITE);

      tft7.setCursor(0, 130);  //effect param1
      char buf3[30];
      switch (effectBankSW) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name03[effectNumSW])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name13[effectNumSW])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name23[effectNumSW])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name33[effectNumSW])));
          break;
      }

      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf3, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }
      tft7.setTextSize(1);
      tft7.print(buf3);

      tft7.setCursor(0, 160);  //effect param2
      char buf4[30];
      switch (effectBankSW) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name04[effectNumSW])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name14[effectNumSW])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name24[effectNumSW])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name34[effectNumSW])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf4, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }
      tft7.setTextSize(1);
      tft7.print(buf4);

      tft7.setCursor(0, 190);  //effect param3
      char buf5[30];
      switch (effectBankSW) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name05[effectNumSW])));
          break;
        case 1:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name15[effectNumSW])));
          break;
        case 2:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name25[effectNumSW])));
          break;
        case 3:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(name35[effectNumSW])));
          break;
      }
      // Check if the pointer is valid
      if (str_ptr != nullptr) {
        // Copy the string from program memory to RAM
        strcpy_P(buf5, str_ptr);
      } else {
        // Handle the case where the pointer is NULL (if needed)
      }
      tft7.setTextSize(1);
      tft7.print(buf5);
      tft7.setCursor(0, 220);  //effect mix
      tft7.print("Effects Mix");
      tft7.drawFastHLine(160, 226, 160, ST7735_RED);
      tft7.drawFastVLine(160, 220, 12, ST7735_RED);
      tft7.drawFastVLine(240, 220, 12, ST7735_RED);
      tft7.drawFastVLine(319, 220, 12, ST7735_RED);
      tft7.fillRoundRect(160, 128, int(effect1 / 6.5), 16, 2, ST7735_YELLOW);
      tft7.fillRoundRect(160, 158, int(effect2 / 6.5), 16, 2, ST7735_YELLOW);
      tft7.fillRoundRect(160, 188, int(effect3 / 6.5), 16, 2, ST7735_YELLOW);
      tft7.fillRoundRect((int(mixa / 6.7) + 160), 218, 8, 16, 2, ST7735_YELLOW);
      tft7.updateScreen();
      //break;

    case 8:  // PolyMOD
      // ************************ DISPLAY 8 **************************
      tft8.setFont(&FreeSans12pt7b);
      tft8.fillScreen(ST7735_BLACK);
      tft8.setCursor(0, 5);
      tft8.setTextSize(3);
      tft8.setTextColor(ST7735_YELLOW);
      tft8.println(currentPgmNum);
      tft8.setCursor(80, 15);
      tft8.setFont(&FreeSans9pt7b);
      tft8.setTextSize(2);
      tft8.setTextColor(ST7735_WHITE);
      tft8.println(currentPatchName);
      tft8.setFont(&FreeSans12pt7b);
      tft8.setTextSize(1);
      tft8.drawFastHLine(0, 58, tft8.width(), ST7735_RED);
      tft8.setTextColor(ST7735_WHITE);

      tft8.setTextColor(ST7735_WHITE);
      tft8.setTextSize(1);
      tft8.setCursor(0, 70);
      tft8.print("Volume");
      tft8.setCursor(0, 100);
      tft8.print("AM Depth");
      tft8.setCursor(0, 130);
      tft8.print("Noise Level");
      tft8.setTextColor(ST7735_RED);

      tft8.drawFastHLine(160, 136, 160, ST7735_RED);
      tft8.drawFastVLine(160, 130, 12, ST7735_RED);
      tft8.drawFastVLine(240, 130, 12, ST7735_RED);
      tft8.drawFastVLine(319, 130, 12, ST7735_RED);
      tft8.fillRoundRect(160, 68, int(volumeControl / 6.5), 16, 2, ST7735_YELLOW);
      tft8.fillRoundRect(160, 98, int(amDepth / 6.5), 16, 2, ST7735_YELLOW);
      tft8.fillRoundRect((int(noiseLevel / 6.7) + 160), 128, 8, 16, 2, ST7735_YELLOW);
      //tft8.updateScreen();
      break;
  }
}


void renderPulseWidth(float value) {
  tft0.drawFastHLine(108, 74, 15 + (value * 13), ST7735_CYAN);
  tft0.drawFastVLine(123 + (value * 13), 74, 20, ST7735_CYAN);
  tft0.drawFastHLine(123 + (value * 13), 94, 16 - (value * 13), ST7735_CYAN);
  if (value < 0) {
    tft0.drawFastVLine(108, 74, 21, ST7735_CYAN);
  } else {
    tft0.drawFastVLine(138, 74, 21, ST7735_CYAN);
  }
}

void renderVarTriangle(float value) {
  tft0.drawLine(110, 94, 123 + (value * 13), 74, ST7735_CYAN);
  tft0.drawLine(123 + (value * 13), 74, 136, 94, ST7735_CYAN);
}

void renderEnv(float att, float dec, float sus, float rel) {
  tft0.drawLine(100, 94, 100 + (att * 60), 74, ST7735_CYAN);
  tft0.drawLine(100 + (att * 60), 74.0, 100 + ((att + dec) * 60), 94 - (sus / 52), ST7735_CYAN);
  tft0.drawFastHLine(100 + ((att + dec) * 60), 94 - (sus / 52), 40 - ((att + dec) * 60), ST7735_CYAN);
  tft0.drawLine(139, 94 - (sus / 52), 139 + (rel * 60), 94, ST7735_CYAN);
}

void renderCurrentParameterPage() {
  switch (state) {
    case PARAMETER:
      tft0.fillScreen(ST7735_BLACK);
      tft0.setFont(&FreeSans12pt7b);
      tft0.setCursor(0, 53);
      tft0.setTextColor(ST7735_YELLOW);
      tft0.setTextSize(1);
      tft0.println(currentParameter);
      tft0.drawFastHLine(10, 62, tft0.width() - 20, ST7735_RED);
      tft0.setCursor(1, 90);
      tft0.setTextColor(ST7735_WHITE);
      tft0.println(currentValue);
      switch (paramType) {
        case PULSE:
          renderPulseWidth(currentFloatValue);
          break;
        case VAR_TRI:
          renderVarTriangle(currentFloatValue);
          break;
        case FILTER_ENV:
          renderEnv(filterAttack * 0.0001, filterDecay * 0.0001, filterSustain, filterRelease * 0.0001);
          break;
      }
      break;
  }
}

void renderDeletePatchPage() {
}

void renderDeleteMessagePage() {
}

void renderSavePage() {
}

void renderReinitialisePage() {
  tft0.fillScreen(ST7735_BLACK);
  tft0.setFont(&FreeSans12pt7b);
  tft0.setTextColor(ST7735_YELLOW);
  tft0.setTextSize(1);
  tft0.setCursor(5, 53);
  tft0.println("Initialise to");
  tft0.setCursor(5, 96);
  tft0.println("panel setting");
}

void renderPatchNamingPage() {
}

void renderRecallPage() {
}

void showRenamingPage(String newName) {
  // newPatchName = newName;
}

void renderUpDown(uint16_t x, uint16_t y, uint16_t colour) {
  //Produces up/down indicator glyph at x,y
  tft0.setCursor(x, y);
  tft0.fillTriangle(x, y, x + 8, y - 8, x + 16, y, colour);
  tft0.fillTriangle(x, y + 4, x + 8, y + 12, x + 16, y + 4, colour);
}


void renderSettingsPage() {
}

void showCurrentParameterPage(const char *param, float val, int pType) {
  currentParameter = param;
  currentValue = String(val);
  currentFloatValue = val;
  paramType = pType;
  //startTimer();
}

void showCurrentParameterPage(const char *param, String val, int pType) {
  if (state == SETTINGS || state == SETTINGSVALUE) state = PARAMETER;  //Exit settings page if showing
  currentParameter = param;
  currentValue = val;
  paramType = pType;
  //startTimer();
}

void showCurrentParameterPage(const char *param, String val) {
  showCurrentParameterPage(param, val, PARAMETER);
}

void showPatchPage(String number, String patchName) {
  currentPgmNum = number;
  currentPatchName = patchName;
}

void showSettingsPage(const char *option, const char *value, int settingsPart) {
}

void displayThread() {
  threads.delay(2000);  //Give bootup page chance to display
  while (1) {
    // //    renderCurrentPatchPage(0);
    //     tft0.updateScreen();
    // //    renderCurrentPatchPage(1);
    //     tft1.updateScreen();
    // //    renderCurrentPatchPage(2);
    //     tft2.updateScreen();
    // //    renderCurrentPatchPage(3);
    //     tft3.updateScreen();
    // //    renderCurrentPatchPage(4);
    //     tft4.updateScreen();
    // //    renderCurrentPatchPage(5);
    //     tft5.updateScreen();
    // //    renderCurrentPatchPage(6);
    //     tft6.updateScreen();
    // //    renderCurrentPatchPage(7);
    //     tft7.updateScreen();
    // //    renderCurrentPatchPage(8);
    //     tft8.updateScreen();
  }
}

void setupDisplay() {

  //threads.addThread(displayThread);
}
