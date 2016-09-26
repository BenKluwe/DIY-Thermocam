/*
*
* FIRST START - Menu that is displayed on the first device start
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

/* Methods */

/* Show welcome Screen for the first start procedure */
void welcomeScreen() {
	display.fillScr(200, 200, 200);
	display.setBackColor(200, 200, 200);
	display.setFont(smallFont);
	display.printC("Welcome to the", CENTER, 20);
	display.setFont(bigFont);
	display.printC("DIY-Thermocam", CENTER, 60, VGA_BLUE);
	display.setFont(smallFont);
	display.printC("This is the first time setup.", CENTER, 110);
	display.printC("It will guide you through the", CENTER, 140);
	display.printC("basic settings for your device.", CENTER, 170);
	display.printC("-> Please touch screen <-", CENTER, 210, VGA_BLUE);
	//Wait for touch press
	while (!touch.touched());
	//Touch release again
	while (touch.touched());
}

/* Shows an info screen during the first start procedure */
void infoScreen(String* text, bool Continue) {
	display.fillScr(200, 200, 200);
	display.setBackColor(200, 200, 200);
	display.setFont(bigFont);
	display.printC(text[0], CENTER, 20, VGA_BLUE);
	display.setFont(smallFont);
	display.printC(text[1], CENTER, 55);
	display.printC(text[2], CENTER, 80);
	display.printC(text[3], CENTER, 105);
	display.printC(text[4], CENTER, 130);
	if (Continue) {
		display.printC(text[5], CENTER, 155);
		display.printC(text[6], CENTER, 180);
		display.printC("-> Please touch screen <-", CENTER, 212, VGA_BLUE);
		//Wait for touch press
		while (!touch.touched());
		//Touch release again
		while (touch.touched());
	}
	else {
		display.printC(text[5], CENTER, 180);
		display.printC(text[6], CENTER, 205);
	}
}

/* Setting screen for the time and date */
void timeDateScreen() {
	String text[7];
	text[0] = "Set Time & Date";
	text[1] = "In the next screen, you can";
	text[2] = "set the time and date, so ";
	text[3] = "that it matches your current";
	text[4] = "time zone. If the settings do";
	text[5] = "not survive a reboot, check";
	text[6] = "the coin cell battery voltage.";
	infoScreen(text);
	//Adjust Time & Date settings
	setTime(12, 30, 30, 15, 6, 2016);
	timeAndDateMenu(true);
	timeAndDateMenuHandler(true);
	//Set time to RTC
	Teensy3Clock.set(now());
}

/* Setting screen for the temperature format */
void tempFormatScreen() {
	String text[7];
	text[0] = "Set Temp. Format";
	text[1] = "In the next screen, you can";
	text[2] = "set the temperature format ";
	text[3] = "for the temperature display.";
	text[4] = "Choose between Celsius or";
	text[5] = "Fahrenheit, the conversion will";
	text[6] = "be done automatically.";
	infoScreen(text);
	//Temperature format
	tempFormatMenu(true);
}

/* Setting screen for the convert image option */
void convertImageScreen() {
	String text[7];
	text[0] = "Convert DAT to BMP";
	text[1] = "In the next screen, select if";
	text[2] = "you also want to create a bitmap";
	text[3] = "file for every saved thermal";
	text[4] = "raw image file on the device. ";
	text[5] = "You can still convert images man-";
	text[6] = "ually in the load menu later.";
	infoScreen(text);;
	//Convert image
	convertImageMenu(true);
}

/* Setting screen for the visual image option */
void visualImageScreen() {
	String text[7];
	text[0] = "Save visual image";
	text[1] = "In the next screen, choose";
	text[2] = "if you want to save an addi-";
	text[3] = "tional visual image together";
	text[4] = "with each saved thermal image.";
	text[5] = "Enable this if you want to";
	text[6] = "create combined images on the PC.";
	infoScreen(text);
	//Visual image
	visualImageMenu(true);
}

/* Setting screen for the combined image alignment */
void combinedAlignmentScreen() {
	String text[7];
	text[0] = "Combined Alignment";
	text[1] = "In the next screen, you can";
	text[2] = "move, increase or decrease";
	text[3] = "the visual image on top of the";
	text[4] = "thermal image. Press the A-button";
	text[5] = "to change the alpha transparency";
	text[6] = "and touch the middle to refresh.";
	infoScreen(text);
	//Set color scheme to rainbow
	colorMap = colorMap_rainbow;
	colorElements = 256;
	//Adjust combined menu
	adjustCombinedNewMenu(true);
}

/* Setting screen for the calibration procedure */
void calibrationScreen() {
	String text[7];
	text[0] = "Calibration";
	text[1] = "Before using the device, you need";
	text[2] = "to calibrate it first. Point the ";
	text[3] = "device to different hot and cold";
	text[4] = "objects in the surrounding area";
	text[5] = "slowly, until the calibration";
	text[6] = "process has been completed.";
	infoScreen(text);
	//Calibration procedure
	calibrationProcess(true);
}

/* Format the SD card for the first time */
void firstFormat() {
	//Old HW generation, check SD card
	if (mlx90614Version == mlx90614Version_old)
		while (!checkSDCard());
	//Format the SD card
	showFullMessage((char*) "Formatting SD card..");
	formatCard();
}

/* Show the first start complete screen */
void firstStartComplete() {
	String text[7];
	text[0] = "Setup completed";
	text[1] = "The first-time setup is";
	text[2] = "now complete. Please reboot";
	text[3] = "the device by turning the";
	text[4] = "power switch off and on again.";
	text[5] = "Afterwards, you will be redirected";
	text[6] = "to the align combined menu.";
	infoScreen(text, false);
	while (true);
}

/* Check if the live mode helper needs to be shown */
bool checkLiveModeHelper() {
	return EEPROM.read(eeprom_liveHelper) != eeprom_setValue;
}

/* Help screen for the first start of live mode */
void liveModeHelper() {
	//Hint screen for the combined image setting
	changeCamRes(VC0706_160x120);
	combinedAlignmentScreen();
	changeCamRes(VC0706_640x480);
	//Do the first time calibration
	calibrationScreen();
	//Array to store up to 7 lines of text
	String text[7];
	//Hint screen for the live mode #1 
	text[0] = "First time helper";
	text[1] = "To enter the live mode menu,";
	text[2] = "touch the screen. 'Exit' will";
	text[3] = "bring you to the main menu.";
	text[4] = "Pressing the push button on";
	text[5] = "top of the device short takes";
	text[6] = "an image, long records a video.";
	infoScreen(text);
	text[1] = "The device needs one minute to";
	text[2] = "warmup the sensor, more functions";
	text[3] = "will be activated afterwards. You";
	text[4] = "can lock the limits or toggle dif-";
	text[5] = "ferent temperature limits by pres-";
	text[6] = "sing the screen long in live mode.";
	infoScreen(text);
	showFullMessage((char*)"Please wait..");
	//Set EEPROM marker to complete
	EEPROM.write(eeprom_liveHelper, eeprom_setValue);
}


/* Set the EEPROM values to default for the first time */
void stdEEPROMSet() {
	showFullMessage((char*) "Flashing spot EEPROM settings..");
	//Set spot maximum temp to 380°C
	mlx90614SetMax();
	//Set spot minimum temp to -70°
	mlx90614SetMin();
	//Set spot filter settings
	mlx90614SetFilter();
	//Set spot emissivity to 0.9
	mlx90614SetEmissivity();
	//Set device EEPROM settings
	EEPROM.write(eeprom_rotationEnabled, false);
	EEPROM.write(eeprom_spotEnabled, true);
	EEPROM.write(eeprom_colorbarEnabled, true);
	EEPROM.write(eeprom_batteryEnabled, true);
	EEPROM.write(eeprom_timeEnabled, true);
	EEPROM.write(eeprom_dateEnabled, true);
	EEPROM.write(eeprom_pointsEnabled, true);
	EEPROM.write(eeprom_storageEnabled, true);
	EEPROM.write(eeprom_displayMode, displayMode_thermal);
	EEPROM.write(eeprom_textColor, textColor_white);
	EEPROM.write(eeprom_minMaxPoints, minMaxPoints_disabled);
	EEPROM.write(eeprom_screenOffTime, screenOffTime_disabled);
	EEPROM.write(eeprom_hotColdMode, hotColdMode_disabled);
	//Set Color Scheme to Rainbow
	EEPROM.write(eeprom_colorScheme, colorScheme_rainbow);
	//Set filter type to box blur
	EEPROM.write(eeprom_filterType, filterType_gaussian);
	//Set current firmware version
	EEPROM.write(eeprom_fwVersion, fwVersion);
	//Set first start marker to true
	EEPROM.write(eeprom_firstStart, eeprom_setValue);
	//Set live helper to false to show it the next time
	EEPROM.write(eeprom_liveHelper, false);
}

/* First start setup*/
void firstStart() {
	//Clear EEPROM
	clearEEPROM();
	//Welcome screen
	welcomeScreen();
	//Hint screen for the time and date settings
	timeDateScreen();
	//Hint screen for temperature format setting
	tempFormatScreen();
	//Hint screen for the convert image settings
	convertImageScreen();
	//Hint screen for the visual image settings
	visualImageScreen();
	//Format SD card for the first time
	firstFormat();
	//Set EEPROM values
	stdEEPROMSet();
	//Show completion message
	firstStartComplete();
}

/* Check if the first start needs to be done */
bool checkFirstStart() {
	return EEPROM.read(eeprom_firstStart) != eeprom_setValue;
}