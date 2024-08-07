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

int transpose = -12;
int realoctave = -36;
int keyboardMode = 0;
int playMode = 0;
int noteMsg;
int readRes = 1023;
int parameterGroup = 0;
int lastPlayedNote = -1;  // Track the last note played
int lastPlayedVoice = 0;  // Track the voice of the last note played
int lastUsedVoice = 0; // Global variable to store the last used voice

//Values below are just for initialising and will be changed when synth is initialised to current panel controls & EEPROM settings
byte midiChannel = 1;//(EEPROM)
byte midiOutCh = 1;//(EEPROM)
byte LEDintensity = 10;//(EEPROM)
byte oldLEDintensity;
int SLIDERintensity = 1;//(EEPROM)
int oldSLIDERintensity;
int i = 0;

//Delayed LFO
int numberOfNotes = 0;
int oldnumberOfNotes = 0;
unsigned long previousMillis = 0;
unsigned long interval = 1; //10 seconds
long delaytime  = 0;

int resolutionFrig = 3;
boolean recallPatchFlag = false;
boolean learning = false;
boolean noteArrived = false;
int setCursorPos = 0;

int MIDIThru = midi::Thru::Off;//(EEPROM)
String patchName = INITPATCHNAME;
boolean encCW = true;//This is to set the encoder to increment when turned CW - Settings Option
boolean updateParams = false;  //(EEPROM)
boolean sendNotes = false;  //(EEPROM)

int NotePriority = 0;
int FilterLoop = 0;
int AmpLoop = 0;
int ClockSource = 0;

// polykit parameters in order of mux

int osc1Tune = 0;
int osc1Tunestr = 0; // for display
int initialosc1Tune = 448;
int oscfmDepth = 0;
int oscfmDepthstr = 0;
int osc2Tune = 0;
int osc2Tunestr = 0;
int initialosc2Tune = 384;
int osc1WaveMod = 0;
int osc1WaveModstr = 0;
int osc2WaveMod = 0;
int osc2WaveModstr = 0;
int osc1WaveSelect = 0;
int osc1WaveSelectstr = 0;
int oct1 = 0;
int oct2 = 0;
int osc2WaveSelect = 0;
int osc2WaveSelectstr = 0;
int osc1WaveA = 0;
int osc1WaveB = 0;
int osc1WaveC = 0;
int osc2WaveA = 0;
int osc2WaveB = 0;
int osc2WaveC = 0;
int glideTime = 0;
int glideTimestr = 0;
int glideTimedisplay = 0;
int glideSW = 0;
int noiseLevel = 0;
int noiseLevelstr = 0;
int noiseLeveldisplay = 0.00;
int osc1PW = 0;
int osc1PWM = 0;
int osc2PW = 0;
int osc2PWM = 0;
int osc1sawLevel = 0;
int osc2sawLevel = 0;
int osc1pulseLevel = 0;
int osc2pulseLevel = 0;
int osc1subLevel = 0;
int osc2triLevel = 0;
int osc2interval = 0;
int osc2detune = 0;
int osc1fmWaveMod = 0;
int osc1fmWaveModstr = 0; // for display
int osc2fmWaveMod = 0;
int osc2fmWaveModstr = 0; // for display

int filterCutoff = 0;
int oldfilterCutoff = 0;
int filterCutoffstr = 0; // for display
int filterLFO = 0;
int filterLFOstr = 0; // for display
int filterRes = 0;
int filterResstr = 0;
int filterType = 0;
int filterA = 0;
int filterB = 0;
int filterC = 0;
int filterEGlevel = 0;
int filterEGlevelstr = 0;
int LFORate = 0;
int LFODelayGo = 0;
int lfoDelay = 0;
String StratusLFOWaveform = "                ";
String Oscillator1Waveform = "                ";
String Oscillator2Waveform = "                ";
int LFOWaveform = 0;
int LFOWaveCV = 0;
int filterAttack = 0;
int filterAttackstr = 0;
int filterDecay = 0;
int filterDecaystr = 0;
int filterSustain = 0;
int filterSustainstr = 0;
int filterRelease = 0;
int filterReleasestr = 0;
int ampAttack = 0;
int oldampAttack = 0;
int ampAttackstr = 0;
int ampDecay = 0;
int oldampDecay = 0;
int ampDecaystr = 0;
int ampSustain = 0;
int oldampSustain = 0;
int ampSustainstr = 0;
int ampRelease = 0;
int oldampRelease = 0;
int ampReleasestr = 0;
int AmpGatedSW = 0;
int amDepth = 0;
int amDepthstr = 0;
int volumeControl = 0;
int volumeControlstr = 0; // for display
int keyTrack = 0;
int keyTrackMult = 0.00;
int keyTrackstr = 0;
int effect1 = 0;
int effect1str = 0;
int effect2 = 0;
int effect2str = 0;
int effect3 = 0;
int effect3str = 0;
int mixa = 0;
int mixastr = 0;
int mixbstr = 0;
int osc1Bank = 0;
int osc1BankB = 0;
int osc2Bank = 0;
int osc2BankB = 0;
int lfoAlt = 0;
int lfoMult = 0;
int LFOMultCV = 0;
int filterPoleSW = 0;
int filterEGinv = 0;
int newbend = 0;
int PitchBendLevel = 0;
int PitchBendLevelstr = 0; // for display
int modWheelDepth = 0;
int modWheelLevel = 0;
int filterLogLin = 0;
int ampLogLin = 0;
float afterTouch = 0;
int AfterTouchDest = 0;
int afterTouchDepth = 0;
float masterTune = 511;

int filterVelSW = 0;
int ampVelSW = 0;
int monoMultiSW = 0;

int effectBankSW = 0;
int currentbank = 0;
int effectNumSW = 0;

int filterenvLinLogSW = 0;
int ampenvLinLogSW = 0;
boolean upperSW = false;
boolean lowerSW = true;
int dumpCompleteSW = 0;
int dumpStartedSW = 0;
int pwLFOwaveformSW = 0;
int PWRate = 0;
int syncSW = 0;
int chordHoldSW = 0;
int pmDestFilterSW = 0;
int pmDestDCO1SW = 0;

int returnvalue = 0;

