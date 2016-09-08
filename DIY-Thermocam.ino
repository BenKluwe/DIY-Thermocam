/*
*
* MAIN SKETCH - Main entry point for the firmware
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

/* Current firmware version */

#define Version "Firmware 2.10 from 08.09.2016"
#define fwVersion 210

/* External Libraries */

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
	//Init the hardware components
	initHardware();
	//Check for hardware issues
	checkDiagnostic();
	//Do the first start setup if required
	if (checkFirstStart())
		firstStart();
	//Check for firmware upgrade done
	checkFWUpgrade();
	//Read all settings from EEPROM
	readEEPROM();
	//Show the live mode helper if required
	if (checkLiveModeHelper())
		liveModeHelper();
	//Go to the live Mode
	liveMode();
}

/* Loop forever */

void loop()
{
	//The code never reaches this
}
