/*
*
* MAIN MENU - Display the main menu with icons
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

/* Draws the background in the main menu */
void mainMenuBackground() {
	display.setColor(120, 120, 120);
	display.fillRoundRect(6, 6, 314, 234);
	display.setColor(200, 200, 200);
	display.fillRect(6, 36, 314, 180);
	display.setColor(VGA_BLACK);
	display.drawHLine(6, 36, 314);
	display.drawHLine(6, 180, 314);
}

/* Draws the content of the selection menu*/
void drawSelectionMenu() {
	//Buttons
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(bigFont);
	touchButtons.addButton(15, 45, 38, 77, (char*) "<", 0, true);
	touchButtons.addButton(267, 45, 38, 77, (char*) ">", 0, true);
	touchButtons.addButton(15, 188, 120, 40, (char*) "Back");
	touchButtons.addButton(95, 132, 130, 35, (char*) "OK");
	touchButtons.drawButtons();
	//Border
	display.setColor(VGA_BLUE);
	display.drawRect(65, 57, 257, 111);
}

/* Draws the title in the main menu */
void mainMenuTitle(char* title) {
	display.setFont(bigFont);
	display.setBackColor(120, 120, 120);
	display.setColor(VGA_WHITE);
	display.print(title, CENTER, 14);
}

/* Draws the current selection in the menu */
void mainMenuSelection(char* selection) {
	//Clear the old content
	display.setColor(VGA_WHITE);
	display.fillRect(66, 58, 257, 111);
	//Print the text
	display.setBackColor(VGA_WHITE);
	display.setColor(VGA_BLUE);
	display.print(selection, CENTER, 77);
}

/* Calibration*/
void calibrationScreen(bool firstStart) {
	//Normal mode
	if (firstStart == false) {
		mainMenuBackground();
		mainMenuTitle((char*)"Calibrating..");
		display.setColor(VGA_BLACK);
		display.setBackColor(200, 200, 200);
		display.setFont(smallFont);
		display.print((char*)"Point the camera to different", CENTER, 63);
		display.print((char*)"hot and cold objects in the area.", CENTER, 96);
		touchButtons.deleteAllButtons();
		touchButtons.setTextFont(bigFont);
		touchButtons.addButton(90, 188, 140, 40, (char*) "Abort");
		touchButtons.drawButtons();
		display.setFont(bigFont);
		display.print((char*) "Status:  0%", CENTER, 140);
	}
	//First start
	else {
		display.fillScr(200, 200, 200);
		display.setFont(bigFont);
		display.setBackColor(200, 200, 200);
		display.setColor(VGA_BLACK);
		display.print((char*) "Calibrating..", CENTER, 100);
		display.print((char*) "Status:  0%", CENTER, 140);
	}
}

/* Calibration Repeat Choose */
bool calibrationRepeat() {
	//Title & Background
	mainMenuBackground();
	mainMenuTitle((char*)"Bad Calibration");
	display.setColor(VGA_BLACK);
	display.setFont(bigFont);
	display.setBackColor(200, 200, 200);
	display.print((char*)"Try again ?", CENTER, 66);
	display.setFont(smallFont);
	display.print((char*)"Use different calibration objects !", CENTER, 201);
	//Draw the buttons
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(bigFont);
	touchButtons.addButton(165, 106, 140, 55, (char*) "Yes");
	touchButtons.addButton(15, 106, 140, 55, (char*) "No");
	touchButtons.drawButtons();
	//Touch handler
	while (true) {
		//If touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//YES
			if (pressedButton == 0) {
				return true;
				break;
			}
			//NO
			else if (pressedButton == 1) {
				return false;
				break;
			}
		}
	}
	return true;
}

/* Calibration Chooser */
bool calibrationChooser() {
	//Title & Background
	mainMenuBackground();
	mainMenuTitle((char*)"Calibration");
	//Draw the buttons
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(bigFont);
	touchButtons.addButton(15, 47, 140, 120, (char*) "New");
	touchButtons.addButton(165, 47, 140, 120, (char*) "Delete");
	touchButtons.addButton(15, 188, 140, 40, (char*) "Back");
	touchButtons.drawButtons();
	//Touch handler
	while (true) {
		//If touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//NEW
			if (pressedButton == 0) {
				calibrationProcess();
				return true;
				break;
			}
			//DELETE
			else if (pressedButton == 1) {
				calSlope = cal_stdSlope;
				calOffset = mlx90614Amb - (calSlope * 8192) + calComp;
				calStatus = cal_standard;
				storeCalibration();
				return true;
				break;
			}
			//BACK
			else if (pressedButton == 2)
				return false;
		}
	}
	return true;
}

/* Switch the current preset menu item */
void tempLimitsPresetSaveString(int pos) {
	char* text = (char*) "";
	switch (pos) {
	case 0:
		text = (char*) "Temporary";
		break;
	case 1:
		text = (char*) "Preset 1";
		break;
	case 2:
		text = (char*) "Preset 2";
		break;
	case 3:
		text = (char*) "Preset 3";
		break;
	}
	mainMenuSelection(text);
}

/* Menu to save the temperature limits to a preset */
bool tempLimitsPresetSaveMenu() {
	//Save the current position inside the menu
	byte menuPos = 0;
	//Background
	mainMenuBackground();
	//Title
	mainMenuTitle((char*) "Select Preset");
	//Draw the selection menu
	drawSelectionMenu();
	//Draw the current item
	tempLimitsPresetSaveString(menuPos);
	//Save the current position inside the menu
	while (true) {
		//Touch screen pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//SELECT
			if (pressedButton == 3) {
				int16_t min, max;
				switch (menuPos) {
					//Temporary
				case 0:
					EEPROM.write(eeprom_minMaxPreset, minMax_temporary);
					break;
					//Preset 1
				case 1:
					min = (int16_t)round(calFunction(minTemp));
					max = (int16_t)round(calFunction(maxTemp));
					EEPROM.write(eeprom_minTemp1High, (min & 0xFF00) >> 8);
					EEPROM.write(eeprom_minTemp1Low, min & 0x00FF);
					EEPROM.write(eeprom_maxTemp1High, (max & 0xFF00) >> 8);
					EEPROM.write(eeprom_maxTemp1Low, max & 0x00FF);
					EEPROM.write(eeprom_minMax1Set, eeprom_setValue);
					EEPROM.write(eeprom_minMaxPreset, minMax_preset1);
					break;
					//Preset 2
				case 2:
					min = (int16_t)round(calFunction(minTemp));
					max = (int16_t)round(calFunction(maxTemp));
					EEPROM.write(eeprom_minTemp2High, (min & 0xFF00) >> 8);
					EEPROM.write(eeprom_minTemp2Low, min & 0x00FF);
					EEPROM.write(eeprom_maxTemp2High, (max & 0xFF00) >> 8);
					EEPROM.write(eeprom_maxTemp2Low, max & 0x00FF);
					EEPROM.write(eeprom_minMax2Set, eeprom_setValue);
					EEPROM.write(eeprom_minMaxPreset, minMax_preset2);
					break;
					//Preset 3
				case 3:
					min = (int16_t)round(calFunction(minTemp));
					max = (int16_t)round(calFunction(maxTemp));
					EEPROM.write(eeprom_minTemp3High, (min & 0xFF00) >> 8);
					EEPROM.write(eeprom_minTemp3Low, min & 0x00FF);
					EEPROM.write(eeprom_maxTemp3High, (max & 0xFF00) >> 8);
					EEPROM.write(eeprom_maxTemp3Low, max & 0x00FF);
					EEPROM.write(eeprom_minMax3Set, eeprom_setValue);
					EEPROM.write(eeprom_minMaxPreset, minMax_preset3);
					break;
				}
				return true;
			}
			//BACKWARD
			else if (pressedButton == 0) {
				if (menuPos > 0)
					menuPos--;
				else if (menuPos == 0)
					menuPos = 3;
			}
			//FORWARD
			else if (pressedButton == 1) {
				if (menuPos < 3)
					menuPos++;
				else if (menuPos == 3)
					menuPos = 0;
			}
			//BACK
			else if (pressedButton == 2)
				return false;
			//Change the menu name
			tempLimitsPresetSaveString(menuPos);
		}
	}
}

/* Touch Handler for the limit chooser menu */
bool tempLimitsManualHandler() {

	//Save the old limits in case the user wants to restore them
	uint16_t maxTemp_old = maxTemp;
	uint16_t minTemp_old = minTemp;
	//Set both modes to false for the first time
	bool minChange = false;
	bool maxChange = false;
	//Buffer
	int min, max;
	char minC[10];
	char maxC[10];

	//Touch handler
	while (true) {
		display.setFont(smallFont);
		min = (int)round(calFunction(minTemp));
		max = (int)round(calFunction(maxTemp));
		if (tempFormat == tempFormat_celcius) {
			sprintf(minC, "Min:%dC", min);
			sprintf(maxC, "Max:%dC", max);
		}
		else {
			sprintf(minC, "Min:%dF", min);
			sprintf(maxC, "Max:%dF", max);
		}
		display.print(maxC, 180, 145);
		display.print(minC, 85, 145);
		display.setFont(bigFont);
		//If touch pressed
		if (touch.touched() == true) {
			int pressedButton;
			//Change values continously and fast when the user holds the plus or minus button
			if (minChange || maxChange)
				pressedButton = touchButtons.checkButtons(true, true);
			//Normal check when not in minChange or maxChange mode
			else
				pressedButton = touchButtons.checkButtons();
			//RESET
			if (pressedButton == 0) {
				//Restore the old values
				if (minChange) {
					minTemp = minTemp_old;
				}
				else if (maxChange) {
					maxTemp = maxTemp_old;
				}
				else {
					minTemp = minTemp_old;
					maxTemp = maxTemp_old;
				}
			}
			//SELECT
			else if (pressedButton == 1) {
				//Leave the minimum or maximum change mode
				if (minChange || maxChange) {
					touchButtons.relabelButton(1, (char*) "OK", true);
					touchButtons.relabelButton(2, (char*) "Min", true);
					touchButtons.relabelButton(3, (char*) "Max", true);
					if (minChange == true)
						minChange = false;
					if (maxChange == true)
						maxChange = false;
				}
				//Go back
				else {
					if (tempLimitsPresetSaveMenu())
						return true;
					else
						return false;
				}
			}
			//DECREASE
			else if (pressedButton == 2) {
				//In minimum change mode - decrease minimum temp
				if ((minChange == true) && (maxChange == false)) {
					//Check if minimum is in range
					if (min > -70) {
						min--;
						minTemp = tempToRaw(min);
					}
				}
				//Enter minimum change mode
				else if ((minChange == false) && (maxChange == false)) {
					touchButtons.relabelButton(1, (char*) "Back", true);
					touchButtons.relabelButton(2, (char*) "-", true);
					touchButtons.relabelButton(3, (char*) "+", true);
					minChange = true;
				}
				//In maximum change mode - decrease maximum temp
				else if ((minChange == false) && (maxChange == true)) {
					//Check if maximum is bigger than minimum
					if (max > (min + 1)) {
						max--;
						maxTemp = tempToRaw(max);
					}
				}
			}
			//INCREASE
			else if (pressedButton == 3) {
				//In maximum change mode - increase maximum temp
				if ((minChange == false) && (maxChange == true)) {
					//Check if maximum is in range
					if (max < 380) {
						max++;
						maxTemp = tempToRaw(max);
					}
				}
				//Enter maximum change mode
				else if ((minChange == false) && (maxChange == false)) {
					touchButtons.relabelButton(1, (char*) "Back", true);
					touchButtons.relabelButton(2, (char*) "-", true);
					touchButtons.relabelButton(3, (char*) "+", true);
					maxChange = true;

				}
				//In minimum change mode - increase minimum temp
				else if ((minChange == true) && (maxChange == false)) {
					//Check if minimum is smaller than maximum
					if (min < (max - 1)) {
						min++;
						minTemp = tempToRaw(min);
					}
				}
			}
			//Prepare the preview image
			delay(10);
			createThermalImg();
			//Display the preview image
			display.drawBitmap(80, 40, 160, 120, image, 1);
		}
	}
}

/* Select the limits in Manual Mode*/
void tempLimitsManual() {
redraw:
	//Background & title
	mainMenuBackground();
	mainMenuTitle((char*) "Temp. Limits");
	//Draw the buttons
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(bigFont);
	touchButtons.addButton(15, 188, 140, 40, (char*) "Reset");
	touchButtons.addButton(165, 188, 140, 40, (char*) "OK");
	touchButtons.addButton(15, 48, 55, 120, (char*) "Min");
	touchButtons.addButton(250, 48, 55, 120, (char*) "Max");
	touchButtons.drawButtons();
	//Prepare the preview image
	delay(10);
	createThermalImg();
	//Display the preview image
	display.drawBitmap(80, 40, 160, 120, image, 1);
	//Draw the border for the preview image
	display.setColor(VGA_BLACK);
	display.drawRect(79, 39, 241, 161);
	//Go into the normal touch handler
	if (!tempLimitsManualHandler())
		goto redraw;
}

/* Switch the temperature limits preset string */
void tempLimitsPresetsString(int pos) {
	char* text = (char*) "";
	switch (pos) {
	case 0:
		text = (char*) "New";
		break;
	case 1:
		text = (char*) "Preset 1";
		break;
	case 2:
		text = (char*) "Preset 2";
		break;
	case 3:
		text = (char*) "Preset 3";
		break;
	}
	mainMenuSelection(text);
}

/* Menu to save the temperature limits to a preset */
bool tempLimitsPresets() {
	//Save the current position inside the menu
	byte tempLimitsMenuPos = EEPROM.read(eeprom_minMaxPreset);
	//Background
	mainMenuBackground();
	//Title
	mainMenuTitle((char*) "Choose Preset");
	//Draw the selection menu
	drawSelectionMenu();
	//Draw the current item
	tempLimitsPresetsString(tempLimitsMenuPos);
	//Save the current position inside the menu
	while (true) {
		//Touch screen pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//SELECT
			if (pressedButton == 3) {
				switch (tempLimitsMenuPos) {
					//New
				case 0:
					tempLimitsManual();
					return true;
					break;
					//Load Preset 1
				case 1:
					EEPROM.write(eeprom_minMaxPreset, minMax_preset1);
					break;
					//Load Preset 2
				case 2:
					EEPROM.write(eeprom_minMaxPreset, minMax_preset2);
					break;
					//Load Preset 3
				case 3:
					EEPROM.write(eeprom_minMaxPreset, minMax_preset3);
					break;
				}
				//Read temperature limits from EEPROM
				readTempLimits();
				return true;
			}
			//BACKWARD
			else if (pressedButton == 0) {
				if (tempLimitsMenuPos > 0)
					tempLimitsMenuPos--;
				else if (tempLimitsMenuPos == 0)
					tempLimitsMenuPos = 3;
			}
			//FORWARD
			else if (pressedButton == 1) {
				if (tempLimitsMenuPos < 3)
					tempLimitsMenuPos++;
				else if (tempLimitsMenuPos == 3)
					tempLimitsMenuPos = 0;
			}
			//BACK
			else if (pressedButton == 2)
				return false;
			//Change the menu name
			tempLimitsPresetsString(tempLimitsMenuPos);
		}
	}
}

/* Temperature Limit Mode Selection */
bool tempLimits() {
	//Do not show in visual mode
	if (displayMode == displayMode_visual) {
		showFullMessage((char*) "No use in visual mode", true);
		delay(1500);
		return false;
	}

	//Still in warmup, do not let the user do this
	if (calStatus == cal_warmup) {
		showFullMessage((char*) "Please wait for sensor warmup!", true);
		delay(1500);
		return true;
	}

	//Title & Background
	mainMenuBackground();
	mainMenuTitle((char*)"Temp. Limits");
	//Draw the buttons
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(bigFont);
	touchButtons.addButton(15, 47, 140, 120, (char*) "Auto");
	touchButtons.addButton(165, 47, 140, 120, (char*) "Manual");
	touchButtons.addButton(15, 188, 140, 40, (char*) "Back");
	touchButtons.drawButtons();
	//Touch handler
	while (true) {
		//If touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//AUTO
			if (pressedButton == 0) {
				//Enable auto mode again and disable limits locked
				autoMode = true;
				limitsLocked = false;
				return true;
			}
			//MANUAL
			else if (pressedButton == 1) {
				//Disable auto mode and limits locked
				autoMode = false;
				limitsLocked = false;
				//Let the user choose the new limits
				return tempLimitsPresets();
			}
			//BACK
			else if (pressedButton == 2)
				return false;
		}
	}
}

/* Menu to add or remove temperature points to the thermal image */
bool tempPointsMenu() {
	//Still in warmup, do not add points
	if (calStatus == cal_warmup) {
		showFullMessage((char*) "Please wait for sensor warmup!", true);
		delay(1500);
		return true;
	}
redraw:
	//Background
	mainMenuBackground();
	//Title
	mainMenuTitle((char*) "Temp. points");
	//Draw the selection menu
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(smallFont);
	touchButtons.addButton(15, 45, 90, 122, (char*) "Add");
	touchButtons.addButton(115, 45, 90, 122, (char*) "Remove");
	touchButtons.addButton(215, 45, 90, 122, (char*) "Clear");
	touchButtons.addButton(15, 188, 120, 40, (char*) "Back");
	touchButtons.drawButtons();
	//Save the current position inside the menu
	while (true) {
		//Touch screen pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//Add
			if (pressedButton == 0) {
				tempPointFunction();
				//Enable points show
				pointsEnabled = true;
				goto redraw;
			}
			//Remove
			else if (pressedButton == 1) {
				tempPointFunction(true);
				goto redraw;
			}
			//Clear
			else if (pressedButton == 2) {
				clearTemperatures();
				showFullMessage((char*)"All points cleared!");
				delay(1000);
				goto redraw;
			}
			//BACK
			else if (pressedButton == 3)
				return false;
		}
	}
	return false;
}

/* Select the color for the live mode string */
void hotColdColorMenuString(int pos) {
	char* text = (char*) "";
	switch (pos) {
		//White
	case 0:
		text = (char*) "White";
		break;
		//Black
	case 1:
		text = (char*) "Black";
		break;
		//Red
	case 2:
		text = (char*) "Red";
		break;
		//Green
	case 3:
		text = (char*) "Green";
		break;
		//Blue
	case 4:
		text = (char*) "Blue";
		break;
	}
	mainMenuSelection(text);
}

/* Menu to display the color in hot/cold color mode */
bool hotColdColorMenu() {
	//Save the current position inside the menu
	byte hotColdColorMenuPos;
	if (hotColdMode == hotColdMode_hot)
		hotColdColorMenuPos = 2;
	else if (hotColdMode == hotColdMode_cold)
		hotColdColorMenuPos = 4;
	else
		hotColdColorMenuPos = 0;
	//Background
	mainMenuBackground();
	//Title
	mainMenuTitle((char*) "Select color");
	//Draw the selection menu
	drawSelectionMenu();
	//Draw the current item
	hotColdColorMenuString(hotColdColorMenuPos);
	//Save the current position inside the menu
	while (true) {
		//Touch screen pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//SELECT
			if (pressedButton == 3) {
				//Save
				hotColdColor = hotColdColorMenuPos;
				//Write to EEPROM
				EEPROM.write(eeprom_hotColdColor, hotColdColor);
				return true;
			}
			//BACK
			if (pressedButton == 2)
				return false;
			//BACKWARD
			else if (pressedButton == 0) {
				if (hotColdColorMenuPos > 0)
					hotColdColorMenuPos--;
				else if (hotColdColorMenuPos == 0)
					hotColdColorMenuPos = 4;
			}
			//FORWARD
			else if (pressedButton == 1) {
				if (hotColdColorMenuPos < 4)
					hotColdColorMenuPos++;
				else if (hotColdColorMenuPos == 4)
					hotColdColorMenuPos = 0;
			}
			//Change the menu name
			hotColdColorMenuString(hotColdColorMenuPos);
		}
	}
}

/* Touch handler for the hot & cold limit changer menu */
void hotColdChooserHandler() {
	//Help variables
	char margin[14];

	//Display level as temperature
	display.setFont(smallFont);
	if (!tempFormat) {
		sprintf(margin, "Limit: %dC", hotColdLevel);
	}
	else {
		sprintf(margin, "Limit: %dF", hotColdLevel);
	}
	display.print(margin, CENTER, 145);

	//Touch handler
	while (true) {
		//If touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//RESET
			if (pressedButton == 0) {
				if (hotColdMode == hotColdMode_cold)
					hotColdLevel = (int16_t)round(calFunction(0.1 * (maxTemp - minTemp) + minTemp));
				if (hotColdMode == hotColdMode_hot)
					hotColdLevel = (int16_t)round(calFunction(0.9 * (maxTemp - minTemp) + minTemp));
			}
			//SELECT
			else if (pressedButton == 1) {
				//Save to EEPROM
				EEPROM.write(eeprom_hotColdLevelHigh, (hotColdLevel & 0xFF00) >> 8);
				EEPROM.write(eeprom_hotColdLevelLow, hotColdLevel & 0x00FF);
				break;
			}
			//MINUS
			else if (pressedButton == 2) {
				if (hotColdLevel > round(calFunction(minTemp)))
					hotColdLevel--;
			}
			//PLUS
			else if (pressedButton == 3) {
				if (hotColdLevel < round(calFunction(maxTemp)))
					hotColdLevel++;
			}
			//Prepare the preview image
			delay(10);
			createThermalImg();
			//Display the preview image
			display.drawBitmap(80, 40, 160, 120, image, 1);
			//Display level as temperature
			if (!tempFormat) {
				sprintf(margin, "Limit: %dC", hotColdLevel);
			}
			else {
				sprintf(margin, "Limit: %dF", hotColdLevel);
			}
			display.print(margin, CENTER, 145);
		}
	}
}

/* Select the limit in hot/cold mode */
void hotColdChooser() {
	//Still in warmup, do not add points
	if (calStatus == cal_warmup) {
		showFullMessage((char*) "Please wait for sensor warmup!", true);
		delay(1500);
		hotColdMode = EEPROM.read(eeprom_hotColdMode);
		return;
	}
	//Background & title
	mainMenuBackground();
	mainMenuTitle((char*) "Set Limit");
	//Draw the buttons
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(bigFont);
	touchButtons.addButton(15, 188, 140, 40, (char*) "Reset");
	touchButtons.addButton(165, 188, 140, 40, (char*) "OK");
	touchButtons.addButton(15, 48, 55, 120, (char*) "-");
	touchButtons.addButton(250, 48, 55, 120, (char*) "+");
	touchButtons.drawButtons();
	//Calculate initial level
	if (hotColdMode == hotColdMode_cold)
		hotColdLevel = (int16_t)round(calFunction(0.1 * (maxTemp - minTemp) + minTemp));
	if (hotColdMode == hotColdMode_hot)
		hotColdLevel = (int16_t)round(calFunction(0.9 * (maxTemp - minTemp) + minTemp));
	//Prepare the preview image
	delay(10);
	createThermalImg();
	//Display the preview image
	display.drawBitmap(80, 40, 160, 120, image, 1);
	//Draw the border for the preview image
	display.setColor(VGA_BLACK);
	display.drawRect(79, 39, 241, 161);
	//Go into the normal touch handler
	hotColdChooserHandler();
}

/* Menu to display hot or cold areas */
bool hotColdMenu() {
redraw:
	//Background
	mainMenuBackground();
	//Title
	mainMenuTitle((char*) "Hot / Cold");
	//Draw the selection menu
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(smallFont);
	touchButtons.addButton(15, 45, 90, 122, (char*) "Hot");
	touchButtons.addButton(115, 45, 90, 122, (char*) "Cold");
	touchButtons.addButton(215, 45, 90, 122, (char*) "Disabled");
	touchButtons.addButton(15, 188, 120, 40, (char*) "Back");
	touchButtons.drawButtons();
	//Save the current position inside the menu
	while (true) {
		//Touch screen pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//Hot
			if (pressedButton == 0) {
				hotColdMode = hotColdMode_hot;
				//Choose the color
				if (hotColdColorMenu())
					//Set the limit
					hotColdChooser();
				//Go back
				else
					goto redraw;
				//Write to EEPROM
				EEPROM.write(eeprom_hotColdMode, hotColdMode);
				return true;
			}
			//Cold
			if (pressedButton == 1) {
				hotColdMode = hotColdMode_cold;
				//Choose the color
				if (hotColdColorMenu())
					//Set the limit
					hotColdChooser();
				//Go back
				else
					goto redraw;
				//Write to EEPROM
				EEPROM.write(eeprom_hotColdMode, hotColdMode);
				return true;
			}
			//Disabled
			if (pressedButton == 2) {
				hotColdMode = hotColdMode_disabled;
				EEPROM.write(eeprom_hotColdMode, hotColdMode);
				return true;
			}
			//BACK
			if (pressedButton == 2)
				return false;
		}
	}
}

/* Switch the current color scheme item */
void colorMenuString(int pos) {
	char* text = (char*) "";
	switch (pos) {
	case colorScheme_arctic:
		text = (char*) "Arctic";
		break;
	case colorScheme_blackHot:
		text = (char*) "Black-Hot";
		break;
	case colorScheme_blueRed:
		text = (char*) "Blue-Red";
		break;
	case colorScheme_coldest:
		text = (char*) "Coldest";
		break;
	case colorScheme_contrast:
		text = (char*) "Contrast";
		break;
	case colorScheme_doubleRainbow:
		text = (char*) "Double-Rain";
		break;
	case colorScheme_grayRed:
		text = (char*) "Gray-Red";
		break;
	case colorScheme_glowBow:
		text = (char*) "Glowbow";
		break;
	case colorScheme_grayscale:
		text = (char*) "Grayscale";
		break;
	case colorScheme_hottest:
		text = (char*) "Hottest";
		break;
	case colorScheme_ironblack:
		text = (char*) "Ironblack";
		break;
	case colorScheme_lava:
		text = (char*) "Lava";
		break;
	case colorScheme_medical:
		text = (char*) "Medical";
		break;
	case colorScheme_rainbow:
		text = (char*) "Rainbow";
		break;
	case colorScheme_wheel1:
		text = (char*) "Wheel 1";
		break;
	case colorScheme_wheel2:
		text = (char*) "Wheel 2";
		break;
	case colorScheme_wheel3:
		text = (char*) "Wheel 3";
		break;
	case colorScheme_whiteHot:
		text = (char*) "White-Hot";
		break;
	case colorScheme_yellow:
		text = (char*) "Yellow";
		break;
	}
	mainMenuSelection(text);
}

/* Choose the applied color scale */
bool colorMenu() {
	//Do not show in visual mode
	if (displayMode == displayMode_visual) {
		showFullMessage((char*) "No use in visual mode", true);
		delay(1500);
		return false;
	}

	//Save the current position inside the menu
	byte changeColorPos = colorScheme;
	//Background
	mainMenuBackground();
	//Title
	mainMenuTitle((char*) "Change Color");
	//Draw the selection menu
	drawSelectionMenu();
	//Draw the current item
	colorMenuString(changeColorPos);
	while (true) {
		//Touch screen pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//SELECT
			if (pressedButton == 3) {
				changeColorScheme(&changeColorPos);
				return true;
			}
			//BACK
			if (pressedButton == 2)
				return false;
			//BACKWARD
			else if (pressedButton == 0) {
				if (changeColorPos > 0)
					changeColorPos--;
				else if (changeColorPos == 0)
					changeColorPos = colorSchemeTotal - 1;
			}
			//FORWARD
			else if (pressedButton == 1) {
				if (changeColorPos < (colorSchemeTotal - 1))
					changeColorPos++;
				else if (changeColorPos == (colorSchemeTotal - 1))
					changeColorPos = 0;
			}
			//Change the menu name
			colorMenuString(changeColorPos);
		}
	}
}

/* Choose the current display mode */
bool modeMenu() {
	//Background
	mainMenuBackground();
	//Title
	mainMenuTitle((char*) "Change Mode");
	//Draw the selection menu
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(smallFont);
	touchButtons.addButton(15, 45, 90, 122, (char*) "Thermal");
	touchButtons.addButton(115, 45, 90, 122, (char*) "Visual");
	touchButtons.addButton(215, 45, 90, 122, (char*) "Combined");
	touchButtons.addButton(15, 188, 120, 40, (char*) "Back");
	touchButtons.drawButtons();
	while (true) {
		//Touch screen pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//Thermal
			if (pressedButton == 0) {
				showFullMessage((char*)"Please wait..", true);
				changeCamRes(VC0706_640x480);
				displayMode = displayMode_thermal;
				EEPROM.write(eeprom_displayMode, displayMode_thermal);
				return true;
			}
			//Visual
			else if (pressedButton == 1) {
				showFullMessage((char*)"Please wait..", true);
				changeCamRes(VC0706_160x120);
				displayMode = displayMode_visual;
				EEPROM.write(eeprom_displayMode, displayMode_visual);
				return true;
			}
			//Combined
			else if (pressedButton == 2) {
				showFullMessage((char*)"Please wait..", true);
				changeCamRes(VC0706_160x120);
				displayMode = displayMode_combined;
				EEPROM.write(eeprom_displayMode, displayMode_combined);
				return true;
			}
			//Back
			if (pressedButton == 3)
				return false;
		}
	}
}

/* Switch the current display option item */
void liveDispMenuString(int pos) {
	char* text = (char*) "";
	switch (pos) {
		//Battery
	case 0:
		if (batteryEnabled)
			text = (char*) "Battery On";
		else
			text = (char*) "Battery Off";
		break;
		//Time
	case 1:
		if (timeEnabled)
			text = (char*) "Time On";
		else
			text = (char*) "Time Off";
		break;
		//Date
	case 2:
		if (dateEnabled)
			text = (char*) "Date On";
		else
			text = (char*) "Date Off";
		break;
		//Spot
	case 3:
		if (spotEnabled)
			text = (char*) "Spot On";
		else
			text = (char*) "Spot Off";
		break;
		//Colorbar
	case 4:
		if (colorbarEnabled)
			text = (char*) "Bar On";
		else
			text = (char*) "Bar Off";
		break;
		//Temperature Points
	case 5:
		if (pointsEnabled)
			text = (char*) "Points On";
		else
			text = (char*) "Points Off";
		break;
		//Storage
	case 6:
		if (storageEnabled)
			text = (char*) "Storage On";
		else
			text = (char*) "Storage Off";
		break;
		//Filter
	case 7:
		if (filterType == filterType_box)
			text = (char*) "Box-Filter";
		else if (filterType == filterType_gaussian)
			text = (char*) "Gaus-Filter";
		else
			text = (char*) "No Filter";
		break;
		//Text Color
	case 8:
		if (textColor == textColor_black)
			text = (char*) "Black Text";
		else if (textColor == textColor_red)
			text = (char*) "Red Text";
		else if (textColor == textColor_green)
			text = (char*) "Green Text";
		else if (textColor == textColor_blue)
			text = (char*) "Blue Text";
		else
			text = (char*) "White Text";
		break;
		//Hottest or coldest
	case 9:
		if (minMaxPoints == minMaxPoints_disabled)
			text = (char*) "No Cold/Hot";
		else if (minMaxPoints == minMaxPoints_min)
			text = (char*) "Coldest";
		else if (minMaxPoints == minMaxPoints_max)
			text = (char*) "Hottest";
		else
			text = (char*) "Both C/H";
		break;

	}
	mainMenuSelection(text);
}

/* Change the live display options */
bool liveDispMenu() {
	//Save the current position inside the menu
	static byte displayOptionsPos = 0;
	//Background
	mainMenuBackground();
	//Title
	mainMenuTitle((char*) "Live Disp. Options");
	//Draw the selection menu
	drawSelectionMenu();
	//Rename OK button
	touchButtons.relabelButton(3, (char*) "Switch", true);
	//Draw the current item
	liveDispMenuString(displayOptionsPos);
	while (true) {
		//Touch screen pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//SELECT
			if (pressedButton == 3) {
				changeDisplayOptions(&displayOptionsPos);
			}
			//BACK
			if (pressedButton == 2) {
				touchButtons.relabelButton(3, (char*) "OK", true);
				return false;
			}
			//BACKWARD
			else if (pressedButton == 0) {
				if (displayOptionsPos > 0)
					displayOptionsPos--;
				else if (displayOptionsPos == 0)
					displayOptionsPos = 9;
			}
			//FORWARD
			else if (pressedButton == 1) {
				if (displayOptionsPos < 9)
					displayOptionsPos++;
				else if (displayOptionsPos == 9)
					displayOptionsPos = 0;
			}
			//Change the menu name
			liveDispMenuString(displayOptionsPos);
		}
	}
}

/* Select the action when the select button is pressed */
bool mainMenuSelect(byte pos, byte page) {
	//First page
	if (page == 0) {
		//Change color
		if (pos == 0) {
			return colorMenu();
		}
		//Change mode
		if (pos == 1) {
			return modeMenu();
		}
		//Temperature limits
		if (pos == 2) {
			return tempLimits();
		}
	}
	//Second page
	if (page == 1) {
		//Load menu
		if (pos == 0) {
			load();
			return true;
		}
		//File Transfer
		if (pos == 1) {
			massStorage();
		}
		//Settings
		if (pos == 2) {
			settingsMenu();
			settingsMenuHandler();
		}
	}
	//Third page
	if (page == 2) {
		//Display options
		if (pos == 0) {
			return liveDispMenu();
		}
		//Laser
		if (pos == 1) {
			toggleLaser(true);
		}
		//Toggle display
		if (pos == 2) {
			toggleDisplay();
		}
	}
	//Fourth page
	if (page == 3) {
		//Calibration
		if (pos == 0) {
			return calibration();
		}
		//Isotherm or adjust visual
		if (pos == 1) {
			if (displayMode == displayMode_thermal)
				hotColdMenu();
			else
				return adjustCombinedMenu();
			return true;
		}
		//Points
		if (pos == 2) {
			return tempPointsMenu();
		}
	}
	return false;
}

/* Draws the content of the main menu*/
void drawMainMenu(byte pos) {
	//Border
	display.setColor(VGA_BLACK);
	display.fillRoundRect(5, 5, 315, 235);
	display.fillRoundRect(4, 4, 316, 236);
	//Background
	display.setColor(200, 200, 200);
	display.fillRoundRect(6, 6, 314, 234);
	//Buttons
	touchButtons.deleteAllButtons();
	//First page
	if (pos == 0) {
		touchButtons.addButton(23, 28, 80, 80, icon1Bitmap, icon1Colors);
		touchButtons.addButton(120, 28, 80, 80, icon2Bitmap, icon2Colors);
		touchButtons.addButton(217, 28, 80, 80, icon3Bitmap, icon3Colors);
	}
	//Second page
	if (pos == 1) {
		touchButtons.addButton(23, 28, 80, 80, icon4Bitmap, icon4Colors);
		touchButtons.addButton(120, 28, 80, 80, icon5Bitmap, icon5Colors);
		touchButtons.addButton(217, 28, 80, 80, icon6Bitmap, icon6Colors);
	}
	//Third page
	if (pos == 2) {
		touchButtons.addButton(23, 28, 80, 80, icon7Bitmap, icon7Colors);
		touchButtons.addButton(120, 28, 80, 80, icon8Bitmap, icon8Colors);
		touchButtons.addButton(217, 28, 80, 80, icon9Bitmap, icon9Colors);
	}
	//Fourth page
	if (pos == 3) {
		touchButtons.addButton(23, 28, 80, 80, icon10Bitmap, icon10Colors);
		if (displayMode == displayMode_thermal)
			touchButtons.addButton(120, 28, 80, 80, icon11_1Bitmap, icon11_1Colors);
		else
			touchButtons.addButton(120, 28, 80, 80, icon11_2Bitmap, icon11_2Colors);
		touchButtons.addButton(217, 28, 80, 80, icon12Bitmap, icon12Colors);
	}
	touchButtons.addButton(23, 132, 80, 80, iconBWBitmap, iconBWColors);
	touchButtons.addButton(120, 132, 80, 80, iconReturnBitmap, iconReturnColors);
	touchButtons.addButton(217, 132, 80, 80, iconFWBitmap, iconFWColors);
	touchButtons.drawButtons();
}

/* Touch Handler for the Live Menu */
void mainMenuHandler(byte* pos) {
	//Main loop
	while (true) {
		//Check for screen sleep
		if (screenOffCheck())
			drawMainMenu(*pos);
		//Touch screen pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//FIRST BUTTON
			if (pressedButton == 0) {
				//Leave menu
				if (mainMenuSelect(0, *pos))
					break;
			}
			//SECOND BUTTON
			if (pressedButton == 1) {
				//Leave menu
				if (mainMenuSelect(1, *pos))
					break;
			}
			//THIRD BUTTON
			if (pressedButton == 2) {
				//Leave menu
				if (mainMenuSelect(2, *pos))
					break;
			}
			//BACKWARD
			else if (pressedButton == 3) {
				if (*pos > 0)
					*pos = *pos - 1;
				else if (*pos == 0)
					*pos = 3;
			}
			//EXIT
			if (pressedButton == 4) {
				showFullMessage((char*)"Please wait..", true);
				return;
			}
			//FORWARD
			else if (pressedButton == 5) {
				if (*pos < 3)
					*pos = *pos + 1;
				else if (*pos == 3)
					*pos = 0;
			}
			drawMainMenu(*pos);
		}
	}
}

/* Start live menu */
void mainMenu() {
	//Position in the main menu
	static byte mainMenuPos = 0;
	//Draw content
	drawMainMenu(mainMenuPos);
	//Touch handler - return true if exit to Main menu, otherwise false
	mainMenuHandler(&mainMenuPos);
	//Wait for touch release
	while (touch.touched());
	//Restore old fonts
	display.setFont(smallFont);
	touchButtons.setTextFont(smallFont);
	//Delete the old buttons
	touchButtons.deleteAllButtons();
	//Disable menu marker
	showMenu = false;
}