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
void drawBar4(int x, int value, int steps, int stepHeight) {
  int filledSteps = value / (1023 / steps);  // Calculate the number of steps to fill
  for (int i = 0; i < steps; i++) {
    int y = 210 - (i * stepHeight);
    if (i < filledSteps) {
      tft4.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_YELLOW);  // Filled step
    } else {
      tft4.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_BLACK);  // Unfilled step
    }
  }
}

// Function to draw the bar
void drawBar5(int x, int value, int steps, int stepHeight) {
  int filledSteps = value / (1023 / steps);  // Calculate the number of steps to fill
  for (int i = 0; i < steps; i++) {
    int y = 210 - (i * stepHeight);
    if (i < filledSteps) {
      tft5.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_YELLOW);  // Filled step
    } else {
      tft5.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_BLACK);  // Unfilled step
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

// Function to draw the bar
void drawBar8(int x, int value, int steps, int stepHeight) {
  int filledSteps = value / (1023 / steps);  // Calculate the number of steps to fill
  for (int i = 0; i < steps; i++) {
    int y = 210 - (i * stepHeight);
    if (i < filledSteps) {
      tft8.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_YELLOW);  // Filled step
    } else {
      tft8.fillRoundRect(x, y - stepHeight, BAR_WIDTH, stepHeight - 2, 2, ST7735_BLACK);  // Unfilled step
    }
  }
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

void drawPWIndicator7(int x, int value) {
  // Clear the entire bar area by drawing a rectangle with the background color
  tft7.fillRoundRect(x, 208 - BAR_HEIGHT, BAR_WIDTH, BAR_HEIGHT, 2, ST7735_BLACK);
  tft7.drawFastVLine(156, 50, 155, ST7735_RED);
  tft7.drawFastHLine(144, 50, 24, ST7735_RED);
  tft7.drawFastHLine(144, 127, 24, ST7735_RED);
  tft7.drawFastHLine(144, 205, 24, ST7735_RED);
  // Calculate the y position for the current value
  int y = 205 - ((value * (BAR_HEIGHT - STEP_HEIGHT)) / 1023);

  // Draw the new bar at the calculated position
  tft7.fillRoundRect(x, y - STEP_HEIGHT / 2, BAR_WIDTH, STEP_HEIGHT, 2, ST7735_YELLOW);
}

void drawPWIndicator8(int x, int value) {
  // Clear the entire bar area by drawing a rectangle with the background color
  tft8.fillRoundRect(x, 208 - BAR_HEIGHT, BAR_WIDTH, BAR_HEIGHT, 2, ST7735_BLACK);
  tft8.drawFastVLine(204, 50, 155, ST7735_RED);
  tft8.drawFastHLine(192, 50, 24, ST7735_RED);
  tft8.drawFastHLine(192, 127, 24, ST7735_RED);
  tft8.drawFastHLine(192, 205, 24, ST7735_RED);
  // Calculate the y position for the current value
  int y = 205 - ((value * (BAR_HEIGHT - STEP_HEIGHT)) / 1023);

  // Draw the new bar at the calculated position
  tft8.fillRoundRect(x, y - STEP_HEIGHT / 2, BAR_WIDTH, STEP_HEIGHT, 2, ST7735_YELLOW);
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
      tft0.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      tft0.setFont(&FreeSans12pt7b);

      // glide indicator box along the top
      if (panelData[P_glideSW] == 0) {
        tft0.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft0.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft0.setCursor(30, 18);
      tft0.print(panelData[P_glideSW] == 0 ? "Glide Off" : "Glide On");

      // chord Hold indicator box along the top
      if (chordHoldSW == 0) {
        tft0.fillRoundRect(180, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft0.fillRoundRect(180, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft0.setCursor(195, 18);
      tft0.print(chordHoldSW == 0 ? "Hold Off" : "Hold On");

      // upper/lower indicator box along the top
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
      switch (playMode) {
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
      switch (panelData[P_keyboardMode]) {
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

      drawBar0(6, panelData[P_glideTime], NUM_STEPS, STEP_HEIGHT);
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
      tft1.setTextColor(ST7735_BLACK);
      // Set range label and value inside a box along the top
      tft1.fillRoundRect(180, 10, 130, 30, 5, ST7735_YELLOW);  // Background box for range
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
      // Drawing the PW lines
      tft1.drawFastVLine(18, 50, 155, ST7735_RED);
      tft1.drawFastHLine(6, 50, 24, ST7735_RED);
      tft1.drawFastHLine(6, 127, 24, ST7735_RED);
      tft1.drawFastHLine(6, 205, 24, ST7735_RED);

      // Drawing the bars
      drawPWIndicator1(6, panelData[P_osc1PW]);
      drawBar1(52, panelData[P_osc1PWM], NUM_STEPS, STEP_HEIGHT);
      drawBar1(98, panelData[P_fmDepth], NUM_STEPS, STEP_HEIGHT);

      drawBar1(192, panelData[P_osc1SawLevel], NUM_STEPS, STEP_HEIGHT);
      drawBar1(236, panelData[P_osc1PulseLevel], NUM_STEPS, STEP_HEIGHT);
      drawBar1(282, panelData[P_osc1SubLevel], NUM_STEPS, STEP_HEIGHT);

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
      tft2.setTextColor(ST7735_BLACK);
      // Set range label and value inside a box along the top
      tft2.fillRoundRect(180, 10, 130, 30, 5, ST7735_YELLOW);  // Background box for range

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

      // Sync indicator box along the top
      if (!panelData[P_sync]) {
        tft2.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft2.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft2.setCursor(30, 18);
      tft2.print(panelData[P_sync] == 0 ? "Sync Off" : "Sync On");

      // Drawing the PW lines
      tft2.drawFastVLine(18, 50, 155, ST7735_RED);
      tft2.drawFastHLine(6, 50, 24, ST7735_RED);
      tft2.drawFastHLine(6, 127, 24, ST7735_RED);
      tft2.drawFastHLine(6, 205, 24, ST7735_RED);

      // Drawing the bars
      drawPWIndicator2(6, panelData[P_osc2PW]);
      drawBar2(52, panelData[P_osc2PWM], NUM_STEPS, STEP_HEIGHT);
      drawBar2(98, panelData[P_osc2Detune], NUM_STEPS, STEP_HEIGHT);
      drawBar2(144, panelData[P_osc2Interval], DET_STEPS, DET_STEP_HEIGHT);
      drawBar2(192, panelData[P_osc2SawLevel], NUM_STEPS, STEP_HEIGHT);
      drawBar2(236, panelData[P_osc2PulseLevel], NUM_STEPS, STEP_HEIGHT);
      drawBar2(282, panelData[P_osc2TriangleLevel], NUM_STEPS, STEP_HEIGHT);

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
      tft3.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
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
      if (panelData[P_filterPoleSW] == 0) {
        tft3.fillRoundRect(240, 10, 70, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft3.fillRoundRect(240, 10, 70, 30, 5, ST7735_RED);  // Red box for on
      }
      tft3.setCursor(260, 18);
      tft3.print(panelData[P_filterPoleSW] == 0 ? "Off" : "On");

      // Set range label and value inside a box along the top
      tft3.fillRoundRect(10, 10, 200, 30, 5, ST7735_YELLOW);  // Background box for range
      tft3.setCursor(40, 18);
      tft3.print(filterDisplay);

      // Drawing the bars
      drawBar3(6, panelData[P_filterCutoff], NUM_STEPS, STEP_HEIGHT);
      drawBar3(52, panelData[P_filterRes], NUM_STEPS, STEP_HEIGHT);
      drawBar3(98, panelData[P_filterEGlevel], NUM_STEPS, STEP_HEIGHT);
      drawBar3(144, panelData[P_keytrack], NUM_STEPS, STEP_HEIGHT);
      drawBar3(192, panelData[P_filterLFO], NUM_STEPS, STEP_HEIGHT);

      break;

    case 4:  // filter ADSR
      // ************************ DISPLAY 4 **************************
      tft4.fillScreen(ST7735_BLACK);
      tft4.setFont(&FreeSans9pt7b);
      tft4.setTextColor(ST7735_WHITE);
      tft4.setTextSize(1);
      tft4.setCursor(10, 220);
      tft4.print("A");
      tft4.setCursor(58, 220);
      tft4.print("D");
      tft4.setCursor(106, 220);
      tft4.print("S");
      tft4.setCursor(148, 220);
      tft4.print("R");

      // filterVelocity
      tft4.setFont(&FreeSans12pt7b);
      tft4.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast
      if (panelData[P_filterVel] == 0) {
        tft4.fillRoundRect(180, 170, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft4.fillRoundRect(180, 170, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft4.setCursor(210, 178);
      tft4.print(panelData[P_filterVel] == 0 ? "Vel Off" : "Vel On");

      // filterEGInv
      if (panelData[P_filterEGinv] == 0) {
        tft4.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft4.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft4.setCursor(30, 18);
      tft4.print(panelData[P_filterEGinv] == 0 ? "EG Pos" : "Eg Neg");

      // filterLinLog
      if (!panelData[P_filterLogLin]) {
        tft4.fillRoundRect(180, 90, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft4.fillRoundRect(180, 90, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft4.setCursor(210, 98);
      tft4.print(panelData[P_filterLogLin] == 0 ? "Env Lin" : "Env Log");

      // filterLoop
      tft4.fillRoundRect(180, 10, 130, 30, 5, ST7735_YELLOW);  // Green box for off
      tft4.setCursor(195, 18);
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

      // Drawing the bars
      drawBar4(6, panelData[P_filterAttack], NUM_STEPS, STEP_HEIGHT);
      drawBar4(52, panelData[P_filterDecay], NUM_STEPS, STEP_HEIGHT);
      drawBar4(98, panelData[P_filterSustain], NUM_STEPS, STEP_HEIGHT);
      drawBar4(144, panelData[P_filterRelease], NUM_STEPS, STEP_HEIGHT);

      break;

    case 5:  // amp ADSR
      // ************************ DISPLAY 5 **************************
      tft5.fillScreen(ST7735_BLACK);
      tft5.setFont(&FreeSans9pt7b);
      tft5.setTextColor(ST7735_WHITE);
      tft5.setTextSize(1);
      tft5.setCursor(10, 220);
      tft5.print("A");
      tft5.setCursor(58, 220);
      tft5.print("D");
      tft5.setCursor(106, 220);
      tft5.print("S");
      tft5.setCursor(148, 220);
      tft5.print("R");
      tft5.setFont(&FreeSans12pt7b);
      tft5.setTextSize(1);
      tft5.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast

      if (!panelData[P_vcaVel]) {
        tft5.fillRoundRect(180, 170, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft5.fillRoundRect(180, 170, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft5.setCursor(210, 178);
      tft5.print(panelData[P_filterVel] == 0 ? "Vel Off" : "Vel On");

      if (panelData[P_vcaGate] == 0) {
        tft5.fillRoundRect(10, 10, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft5.fillRoundRect(10, 10, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft5.setCursor(30, 18);
      tft5.print(panelData[P_vcaGate] == 0 ? "Gate Off" : "Gate On");

      if (!panelData[P_ampLogLin]) {
        tft5.fillRoundRect(180, 90, 130, 30, 5, ST7735_GREEN);  // Green box for off
      } else {
        tft5.fillRoundRect(180, 90, 130, 30, 5, ST7735_RED);  // Red box for on
      }
      tft5.setCursor(210, 98);
      tft5.print(panelData[P_ampLogLin] == 0 ? "Env Lin" : "Env Log");


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

      // Drawing the bars
      drawBar5(6, panelData[P_ampAttack], NUM_STEPS, STEP_HEIGHT);
      drawBar5(52, panelData[P_ampDecay], NUM_STEPS, STEP_HEIGHT);
      drawBar5(98, panelData[P_ampSustain], NUM_STEPS, STEP_HEIGHT);
      drawBar5(144, panelData[P_ampRelease], NUM_STEPS, STEP_HEIGHT);

      break;

    case 6:  // LFO
      // ************************ DISPLAY 6 **************************
      tft6.fillScreen(ST7735_BLACK);  // Fill the screen with black
      tft6.setFont(&FreeSans9pt7b);
      tft6.setTextColor(ST7735_WHITE);
      tft6.setTextSize(1);

      // Setting text labels at the bottom of each bar
      tft6.setCursor(0, 220);
      tft6.print("LFO");
      tft6.setCursor(50, 220);
      tft6.print("Dly");
      tft6.setCursor(96, 220);
      tft6.print("PW");
      tft6.setCursor(140, 220);
      tft6.print("MW");

      tft6.setFont(&FreeSans12pt7b);
      tft6.setTextColor(ST7735_BLACK);
    
      switch (panelData[P_lfoAlt]) {
        case 0:
          str_ptr = reinterpret_cast<const char *>(pgm_read_ptr(&(lfo02[panelData[P_LFOWaveform]])));
          break;
        case 1:
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
      tft6.fillRoundRect(10, 10, 300, 30, 5, ST7735_YELLOW);  // Background box for range
      tft6.setCursor(40, 18);
      tft6.print(lfoDisplay);

      if (panelData[P_lfoAlt]) {
        tft6.fillRoundRect(180, 50, 130, 30, 5, ST7735_RED);
      } else {
        tft6.fillRoundRect(180, 50, 130, 30, 5, ST7735_GREEN);
      }
      tft6.setCursor(200, 58);
      tft6.print(panelData[P_lfoAlt] == 0 ? "Alt Off" : "Alt On");

      if (panelData[P_monoMulti]) {
        tft6.fillRoundRect(180, 90, 130, 30, 5, ST7735_RED);
      } else {
        tft6.fillRoundRect(180, 90, 130, 30, 5, ST7735_GREEN);
      }
      tft6.setCursor(200, 98);
      tft6.print(panelData[P_monoMulti] == 0 ? "Trig Off" : "Trig On");

      // Set range label and value inside a box along the top
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

      // Drawing the bars
      drawBar6(6, panelData[P_LFORate], NUM_STEPS, STEP_HEIGHT);
      drawBar6(52, panelData[P_LFODelay], NUM_STEPS, STEP_HEIGHT);
      drawBar6(98, panelData[P_pwLFO], NUM_STEPS, STEP_HEIGHT);
      drawBar6(144, panelData[P_modWheelDepth], NUM_STEPS, STEP_HEIGHT);

      break;

    case 7:  // Effects
      // ************************ DISPLAY 7 **************************
      tft7.fillScreen(ST7735_BLACK);  // Fill the screen with black
      tft7.setFont(&FreeSans9pt7b);
      tft7.setTextColor(ST7735_WHITE);
      tft7.setTextSize(1);

      // Setting text labels at the bottom of each bar
      tft7.setCursor(6, 220);
      tft7.print("P1");
      tft7.setCursor(50, 220);
      tft7.print("P2");
      tft7.setCursor(98, 220);
      tft7.print("P3");
      tft7.setCursor(142, 220);
      tft7.print("Mix");

      tft7.drawFastVLine(156, 50, 155, ST7735_RED);
      tft7.drawFastHLine(144, 50, 24, ST7735_RED);
      tft7.drawFastHLine(144, 127, 24, ST7735_RED);
      tft7.drawFastHLine(144, 205, 24, ST7735_RED);

      drawBar7(6, panelData[P_effectPot1], NUM_STEPS, STEP_HEIGHT);
      drawBar7(52, panelData[P_effectPot2], NUM_STEPS, STEP_HEIGHT);
      drawBar7(98, panelData[P_effectPot3], NUM_STEPS, STEP_HEIGHT);
      drawPWIndicator7(144, panelData[P_effectsMix]);

      tft7.setFont(&FreeSans12pt7b);
      tft7.setTextColor(ST7735_BLACK);  // Change text color to black for better contrast

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

      break;

    case 8:  // PolyMOD
      // ************************ DISPLAY 8 **************************
      tft8.fillScreen(ST7735_BLACK);  // Fill the screen with black
      tft8.setFont(&FreeSans9pt7b);
      tft8.setTextColor(ST7735_WHITE);
      tft8.setTextSize(1);

      // Setting text labels at the bottom of each bar
      tft8.setCursor(0, 220);
      tft8.print("DCO");
      tft8.setCursor(48, 220);
      tft8.print("Env");
      tft8.setCursor(96, 220);
      tft8.print("Vol");
      tft8.setCursor(142, 220);
      tft8.print("AM");
      tft8.setCursor(142, 220);
      tft8.print("AM");
      tft8.setCursor(184, 220);
      tft8.print("Noise");

      tft8.setFont(&FreeSans12pt7b);
      tft8.setTextColor(ST7735_BLACK);

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

      tft8.drawFastVLine(204, 50, 155, ST7735_RED);
      tft8.drawFastHLine(192, 50, 24, ST7735_RED);
      tft8.drawFastHLine(192, 127, 24, ST7735_RED);
      tft8.drawFastHLine(192, 205, 24, ST7735_RED);

      drawBar8(6, panelData[P_pmDCO2], NUM_STEPS, STEP_HEIGHT);
      drawBar8(52, panelData[P_pmFilterEnv], NUM_STEPS, STEP_HEIGHT);
      drawBar8(98, panelData[P_volumeControl], NUM_STEPS, STEP_HEIGHT);
      drawBar8(144, panelData[P_amDepth], NUM_STEPS, STEP_HEIGHT);
      drawPWIndicator8(192, panelData[P_noiseLevel]);

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
          renderEnv(panelData[P_filterAttack] * 0.0001, panelData[P_filterDecay] * 0.0001, panelData[P_filterSustain], panelData[P_filterRelease] * 0.0001);
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
