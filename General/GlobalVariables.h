/*
*
* GLOBAL VARIABLES - Global variable declarations, that are used firmware-wide
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

#include "GlobalDefines.h"

/* Variables */

//Display Controller
UTFT display;
//Touch Controller
Touchscreen touch;
//ADC
ADC *batMeasure;
//Buttons
UTFT_Buttons touchButtons(&display, &touch);
//Timer
Metro screenOff;
bool screenPressed;
byte screenOffTime;
//Button Debouncer
Bounce buttonDebouncer(pin_button, 100);
Bounce touchDebouncer(pin_touch_irq, 100);
//Fonts
extern uint8_t tinyFont[];
extern uint8_t smallFont[];
extern uint8_t bigFont[];
//SD
SdFat sd;
SdFile sdFile;
String sdInfo;
//Camera
Camera cam(&Serial1);

//Current color scheme - standard is rainbow
byte colorScheme;
//Pointer to the current color scheme
const byte *colorMap;
//Number of rgb elements inside the color scheme
int16_t colorElements;

//160x120 image storage
unsigned short image[19200];
//Array to store the printed temperatures
uint16_t showTemp[192];
//Save filename 
char saveFilename[20];

//HW diagnostic information
byte diagnostic = diag_ok;
//Battery
int8_t batPercentage;
//MLX90614 sensor version
bool mlx90614Version;
//FLIR Lepton sensor version
byte leptonVersion;
//Temperature format
bool tempFormat;
//Text color
byte textColor;
//Laser state
bool laserEnabled;
//Shutter modus
byte shutterMode;
//Display mode
byte displayMode;
//Variables for color calculation
uint16_t maxTemp;
uint16_t minTemp;
//Position of min and maxtemp
uint16_t minTempPos;
uint16_t maxTempPos;
//Hot / Cold mode
byte hotColdMode;
int16_t hotColdLevel;
byte hotColdColor;

//If enabled, image will be converted to bitmap every time
bool convertEnabled;
//If enabled, visual image will be saved automatically
bool visualEnabled;
//Automatic mode
bool autoMode;
//Lock current limits
bool limitsLocked;
//Display rotation enabled
bool rotationEnabled;

//Display options
bool batteryEnabled;
bool timeEnabled;
bool dateEnabled;
bool spotEnabled;
bool colorbarEnabled;
bool pointsEnabled;
bool storageEnabled;
byte filterType;
bool ambientEnabled;
byte minMaxPoints;

//Calibration offset
float calOffset;
//Calibration slope
float calSlope;
//Calibration status
byte calStatus;
//Calibration compensation
float calComp;
//Calibration warmup timer
long calTimer;

//Adjust combined image
float adjCombAlpha;
byte adjCombLeft;
byte adjCombRight;
byte adjCombUp;
byte adjCombDown;

//Save Image in the next cycle
volatile byte imgSave;
//Save Video in the next cycle
volatile bool videoSave;
//Show Live Mode Menu in the next cycle
volatile bool showMenu;
//Handler for a long touch press
volatile bool longTouch;
//Check if in serial mode
volatile bool serialMode;