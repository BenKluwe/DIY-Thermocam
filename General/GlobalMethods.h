/*
*
* GLOBAL METHODS - Global method definitions, that are used firmware-wide
*
* DIY-Thermocam Firmware
*
* GNU General Public License v3.0
*
* Copyright by Max Ritter
*
* http://www.diy-thermocam.net
* https://github.com/maxritter/DIY-Thermocam
*
*/

/* Abstract methods */

bool adjustCombinedMenu();
void showFullMessage(char*, bool small = false);
void firstStart();
void startAltClockline(bool sdStart = false);
void endAltClockline();
bool tempLimits();
float calFunction(uint16_t rawValue);
void createThermalImg();
void floatToChar(char* buffer, float val);
void showSpot();
void showColorBar();
void settingsMenu();
void timeMenu(bool firstStart = false);
void dateMenu(bool firstStart = false);
void limitValues();
void timeAndDateMenu(bool firstStart = false);
void saveRawData(bool image, char* name, uint16_t framesCaptured = 0);
void proccessVideoFrames(uint16_t framesCaptured, char* dirname);
void createVideoFolder(char* dirname);
void boxFilter();
void gaussianFilter();
void convertColors();
void bootScreen();
void storageMenu();
void checkWarmup();
float bytesToFloat(uint8_t* farray);
float celciusToFahrenheit(float Tc);
uint16_t tempToRaw(float temp);
void frameFilename(char* filename, uint16_t count);
void liveMode();
void load();
void loadRawData(char* filename, char* dirname = NULL);
void settingsMenuHandler();
void saveDisplayImage(char* filename, char* dirname = NULL);
uint16_t getVideoFrameNumber(char* dirname);
bool checkSDCard();
void infoScreen(String* text, bool Continue = true);
void getTemperatures();
void liveModeHelper();
void selectColorScheme();
void findMinMaxPositions();
void changeColorScheme(byte* pos);
void calibrationProcess(bool firstStart = false);
void clearTemperatures();
void tempPointFunction(bool remove = false);
void showTemperatures();
void calculateMinMaxPoint(uint16_t* xpos, uint16_t* ypos, uint16_t pixelIndex);
void mainMenu();
bool calibration();
void storeCalibration();
void changeDisplayOptions(byte* pos);
void displayInfos();
void longTouchHandler();
void showTransMessage(char* msg, bool bottom = false);
void createJPGFile(char* filename, char* dirname = NULL);
void floatToBytes(uint8_t* farray, float val);
bool extButtonPressed();
void disableScreenLight();
void enableScreenLight();
void refreshTempPoints();
void displayMenu();
void showDiagnostic();
void setDiagnostic(byte device);
bool checkDiagnostic(byte device);
void createVisCombImg();
void showImage();
void compensateCalib();
void touchIRQ();
void displayRawData();
void loadBMPImage(char* filename);
void loadTouchIRQ();
void checkImageSave();
void createSDName(char* filename, bool folder = false);
void toggleLaser(bool message = false);