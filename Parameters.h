const int sysexDataLength = 72;
byte receivedSysExData[sysexDataLength];
bool isSysExProcessing = false;  // Flag to indicate SysEx processing state
unsigned long lastSysExByteTime = 0;
const unsigned long sysExTimeout = 100; // Timeout for SysEx processing (in milliseconds)

unsigned long timerStart = 0;
bool timerRunning = false;
const unsigned long timerDuration = 500; // 500 milliseconds

int maxSectionWidth = 0;
int attackX = 0;
int decayX = 0;
int sustainLevel = 0;
int sustainX = 0;
int releaseX = 0;
int offsetY = 98;
int pointsX[6];
int pointsY[6];
int AmpattackX = 0;
int AmpdecayX = 0;
int AmpsustainLevel = 0;
int AmpsustainX = 0;
int AmpreleaseX = 0;
int AmppointsX[6];
int AmppointsY[6];

char filterDisplay[30];
char lfoDisplay[30];

int keyboardMode = 0;
int playMode = 0;
int readRes = 1023;
int parameterGroup = 0;

//Values below are just for initialising and will be changed when synth is initialised to current panel controls & EEPROM settings
byte midiChannel = 1;//(EEPROM)
byte midiOutCh = 1;//(EEPROM)
int i = 0;

int resolutionFrig = 3;
boolean recallPatchFlag = false;
int setCursorPos = 0;

String patchName = INITPATCHNAME;

int NotePriority = 0;
int FilterLoop = 0;
int AmpLoop = 0;
int ClockSource = 0;
int chordHoldSW = 0;
boolean upperSW = false;
boolean lowerSW = true;
boolean syncSW = false;
boolean filterenvLinLogSW = false;
boolean ampenvLinLogSW = false;
int lfoMult = 0;
int effectBankSW = 0;
int effectNumSW = 0;
boolean pmDestDCO1SW = false;
boolean pmDestFilterSW = false;
boolean monoMultiSW = false;
int pwLFOwaveformSW = 0;
boolean dumpCompleteSW = false;

int returnvalue = 0;

int upperData[70];
int lowerData[70];
int panelData[70];

#define P_pwLFO 1
#define P_fmDepth 2
#define P_osc2PW 3
#define P_osc2PWM 4
#define P_osc1PW 5
#define P_osc1PWM 6
#define P_osc1Range 7 
#define P_osc2Range 8 
#define P_osc2Interval 9
#define P_glideTime 10 
#define P_osc2Detune 11
#define P_noiseLevel 12
#define P_osc2SawLevel 13
#define P_osc1SawLevel 14
#define P_osc2PulseLevel 15
#define P_osc1PulseLevel 16
#define P_filterCutoff 17
#define P_filterLFO 18
#define P_filterRes 19
#define P_filterType 20
#define P_modWheelDepth 21
#define P_effectsMix 22
#define P_LFODelayGo 23
#define P_filterEGlevel 24
#define P_LFORate 25
#define P_LFOWaveform 26
#define P_filterAttack 27
#define P_filterDecay 28
#define P_filterSustain 29
#define P_filterRelease 30
#define P_ampAttack 31
#define P_ampDecay 32
#define P_ampSustain 33
#define P_ampRelease 34
#define P_volumeControl 35
#define P_glideSW 36
#define P_keytrack 37
#define P_filterPoleSW 38
#define P_filterLoop 39
#define P_filterEGinv 40
#define P_filterVel 41
#define P_vcaLoop 42
#define P_vcaVel 43
#define P_vcaGate 44
#define P_lfoAlt 45
#define P_pmDCO2 46
#define P_pmFilterEnv 47
#define P_monoMulti 48
#define P_modWheelLevel 49
#define P_PitchBendLevel 50
#define P_amDepth 51
#define P_sync 52
#define P_effectPot1 53
#define P_effectPot2 54
#define P_effectPot3 55
#define P_oldampAttack 56
#define P_oldampDecay 57
#define P_oldampSustain 58
#define P_oldampRelease 59
#define P_AfterTouchDest 60
#define P_filterLogLin 61
#define P_ampLogLin 62
#define P_osc2TriangleLevel 63
#define P_osc1SubLevel 64
#define P_keyTrackSW 65
#define P_LFODelay 66