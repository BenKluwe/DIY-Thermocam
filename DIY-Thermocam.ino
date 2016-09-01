/*
*
* DIY-Thermocam Firmware
*
* 2014-2016 by Max Ritter
*
* http://www.diy-thermocam.net
* https://github.com/maxritter/DIY-Thermocam
*
*/

/* Current firmware version */

#define Version "Firmware 2.01 from 01.09.2016"
#define fwVersion 201

/* Libraries */
#include <ADC.h>
#include <i2c_t3.h>
#include <SdFat.h>
#include <tjpgd.h>
#include <SPI.h>
#include <TimeLib.h>
#include <EEPROM.h>
#include <Bounce.h>
#include <UTFT_Buttons.h>
#include <UTFT.h>
#include <Touchscreen.h>
#include <Camera.h>
#include <Metro.h>

/* General Includes */
#include "General/ColorSchemes.h"
#include "General/GlobalVariables.h"
#include "General/GlobalMethods.h"

/* Modules */
#include "Hardware/Hardware.h"
#include "GUI/GUI.h"
#include "Thermal/Thermal.h"


/* Main Entry point */
void setup()
{
	//Reset diagnostic status
	resetDiagnostic();
	//Init Hardware
	initHardware();
	//Check for hardware issues
	checkDiagnostic();
	//Do the first start setup
	if (checkFirstStart())
		firstStart();
	//Check FW upgrade
	checkFWUpgrade();
	//Read EEPROM settings
	readEEPROM();
	//Show the live mode helper
	if (checkLiveModeHelper())	
		//Show the live mode helper
		liveModeHelper();
	//Go to the live Mode
	liveMode();
}

/* Loop forever */
void loop()
{
}
