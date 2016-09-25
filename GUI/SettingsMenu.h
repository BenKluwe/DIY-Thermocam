/*
*
* SETTINGS MENU - Adjust different on-device settings
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

/* Draw the GUI elements for the adjust combined menu */
void adjustCombinedGUI(bool firstStart = false) {
	//Color and font
	setTextColor();
	display.setFont(bigFont);
	display.setBackColor(VGA_TRANSPARENT);

	//Down arrow
	display.drawLine(149, 225, 159, 235);
	display.drawLine(150, 225, 160, 235);
	display.drawLine(151, 225, 161, 235);
	display.drawLine(159, 235, 169, 225);
	display.drawLine(160, 235, 170, 225);
	display.drawLine(161, 235, 171, 225);
	//Left arrow
	display.drawLine(15, 109, 5, 119);
	display.drawLine(15, 110, 5, 120);
	display.drawLine(15, 111, 5, 121);
	display.drawLine(5, 119, 15, 129);
	display.drawLine(5, 120, 15, 130);
	display.drawLine(5, 121, 15, 131);
	//Up arrow
	display.drawLine(149, 15, 159, 5);
	display.drawLine(150, 15, 160, 5);
	display.drawLine(151, 15, 161, 5);
	display.drawLine(159, 5, 169, 15);
	display.drawLine(160, 5, 170, 15);
	display.drawLine(161, 5, 171, 15);
	//Right arrow
	display.drawLine(305, 109, 315, 119);
	display.drawLine(305, 110, 315, 120);
	display.drawLine(305, 111, 315, 121);
	display.drawLine(315, 119, 305, 129);
	display.drawLine(315, 120, 305, 130);
	display.drawLine(315, 121, 305, 131);

	//Refresh screen
	display.print('R', 5, 5);
	//Alpha level
	display.print('A', 5, 225);

	//In first start, do not print the close symbol
	if (!firstStart) {
		//Color for close
		display.setColor(255, 0, 0);
		//Close button
		display.drawLine(294, 5, 314, 25);
		display.drawLine(295, 5, 315, 25);
		display.drawLine(296, 5, 316, 25);
		display.drawLine(294, 25, 314, 5);
		display.drawLine(295, 25, 315, 5);
		display.drawLine(296, 25, 316, 5);
	}
	//Color for confirm
	display.setColor(0, 255, 0);
	//Confirm button
	display.drawLine(294, 225, 304, 235);
	display.drawLine(295, 225, 305, 235);
	display.drawLine(296, 225, 306, 235);
	display.drawLine(304, 235, 314, 215);
	display.drawLine(305, 235, 315, 215);
	display.drawLine(306, 235, 316, 215);

	//Restore old font
	display.setFont(smallFont);
}

/* Refresh the screen content in adjust combined menu */
void adjustCombinedRefresh(bool firstStart = false) {
	//Safe delay
	delay(10);
	//Display the preview image
	createVisCombImg();
	display.writeScreen(image);
	//Display the GUI
	adjustCombinedGUI(firstStart);
}

/* Shows on the screen that is refreshes */
void adjustCombinedLoading() {
	//Set Text Color
	setTextColor();
	//set Background transparent
	display.setBackColor(VGA_TRANSPARENT);
	//Give the user a hint that it tries to save
	display.setFont(bigFont);
	//Show text
	display.print((char*) "Please wait..", CENTER, 110);
	//Return to small font
	display.setFont(smallFont);
}

/* Switch the current preset menu item */
void adjustCombinedPresetSaveString(int pos) {
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

/* Menu to save the adjust combined settings to a preset */
bool adjustCombinedPresetSaveMenu() {
	//Save the current position inside the menu
	byte menuPos = 0;
	//Background
	mainMenuBackground();
	//Title
	mainMenuTitle((char*) "Select Preset");
	//Draw the selection menu
	drawSelectionMenu();
	//Draw the current item
	adjustCombinedPresetSaveString(menuPos);
	//Save the current position inside the menu
	while (true) {
		//Touch screen pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//SELECT
			if (pressedButton == 3) {
				switch (menuPos) {
					//Temporary
				case 0:
					EEPROM.write(eeprom_adjCombPreset, adjComb_temporary);
					break;
					//Preset 1
				case 1:
					EEPROM.write(eeprom_adjComb1Left, adjCombLeft);
					EEPROM.write(eeprom_adjComb1Right, adjCombRight);
					EEPROM.write(eeprom_adjComb1Up, adjCombUp);
					EEPROM.write(eeprom_adjComb1Down, adjCombDown);
					EEPROM.write(eeprom_adjComb1Alpha, round(adjCombAlpha * 100.0));
					EEPROM.write(eeprom_adjComb1Set, eeprom_setValue);
					EEPROM.write(eeprom_adjCombPreset, adjComb_preset1);
					break;
					//Preset 2
				case 2:
					EEPROM.write(eeprom_adjComb2Left, adjCombLeft);
					EEPROM.write(eeprom_adjComb2Right, adjCombRight);
					EEPROM.write(eeprom_adjComb2Up, adjCombUp);
					EEPROM.write(eeprom_adjComb2Down, adjCombDown);
					EEPROM.write(eeprom_adjComb2Alpha, round(adjCombAlpha * 100.0));
					EEPROM.write(eeprom_adjComb2Set, eeprom_setValue);
					EEPROM.write(eeprom_adjCombPreset, adjComb_preset2);
					break;
					//Preset 3
				case 3:
					EEPROM.write(eeprom_adjComb3Left, adjCombLeft);
					EEPROM.write(eeprom_adjComb3Right, adjCombRight);
					EEPROM.write(eeprom_adjComb3Up, adjCombUp);
					EEPROM.write(eeprom_adjComb3Down, adjCombDown);
					EEPROM.write(eeprom_adjComb3Alpha, round(adjCombAlpha * 100.0));
					EEPROM.write(eeprom_adjComb3Set, eeprom_setValue);
					EEPROM.write(eeprom_adjCombPreset, adjComb_preset3);
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
			adjustCombinedPresetSaveString(menuPos);
		}
	}
}

/* Asks the user if he really wants to leave the adjust combined menu */
bool adjustCombinedLeave() {
	//Title & Background
	drawTitle((char*) "Leave Prompt", true);
	display.setColor(VGA_BLACK);
	display.setFont(smallFont);
	display.setBackColor(200, 200, 200);
	display.print((char*)"Do you really want to leave", CENTER, 80);
	display.print((char*)"the adjust combined menu", CENTER, 100);
	display.print((char*)"without saving the settings?", CENTER, 120);
	//Draw the buttons
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(bigFont);
	touchButtons.addButton(165, 160, 140, 55, (char*) "Yes");
	touchButtons.addButton(15, 160, 140, 55, (char*) "No");
	touchButtons.drawButtons();
	touchButtons.setTextFont(smallFont);
	//Wait for touch release
	while (touch.touched());
	//Touch handler
	while (true) {
		//If touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//YES
			if (pressedButton == 0) {
				return true;
			}
			//NO
			else if (pressedButton == 1) {
				return false;
			}
		}
	}
}

/* Touch handler for the new adjust combined menu */
void adjustCombinedNewMenuHandler(bool firstStart = false) {
	//Touch handler
	while (true) {
		//Touch pressed
		if (touch.touched() == true) {
			//Get coordinates
			TS_Point p = touch.getPoint();
			uint16_t x = p.x;
			uint16_t y = p.y;
			//Reset
			if ((x >= 0) && (x <= 50) && (y >= 0) && (y <= 50)) {
				adjustCombinedLoading();
				adjCombDown = 0;
				adjCombUp = 0;
				adjCombLeft = 0;
				adjCombRight = 0;
				adjCombAlpha = 0.5;
				adjustCombinedRefresh();
			}
			//Alpha level
			else if ((x >= 0) && (x <= 50) && (y >= 190) && (y <= 240)) {
				char buffer[20];
				if (adjCombAlpha < 0.7)
					adjCombAlpha += 0.1;
				else
					adjCombAlpha = 0.3;
				sprintf(buffer, "Alpha set to %.1f", adjCombAlpha);
				display.setFont(bigFont);
				setTextColor();
				display.print(buffer, CENTER, 110);
				display.setFont(smallFont);
				adjustCombinedRefresh();
			}
			//Left
			else if ((x >= 0) && (x <= 50) && (y >= 95) && (y <= 145)) {
				if (adjCombLeft < 5) {
					adjustCombinedLoading();
					if (adjCombRight > 0)
						adjCombRight -= 1;
					else
						adjCombLeft += 1;
					adjustCombinedRefresh();
				}
			}
			//Right
			else if ((x >= 270) && (x <= 320) && (y >= 95) && (y <= 145)) {
				if (adjCombRight < 5) {
					adjustCombinedLoading();
					if (adjCombLeft > 0)
						adjCombLeft -= 1;
					else
						adjCombRight += 1;
					adjustCombinedRefresh();
				}
			}
			//Up
			else if ((x >= 135) && (x <= 185) && (y >= 0) && (y <= 50)) {
				if (adjCombUp < 5) {
					adjustCombinedLoading();
					if (adjCombDown > 0)
						adjCombDown -= 1;
					else
						adjCombUp += 1;
					adjustCombinedRefresh();
				}
			}
			//Down
			else if ((x >= 135) && (x <= 185) && (y >= 190) && (y <= 240)) {
				if (adjCombDown < 5) {
					adjustCombinedLoading();
					if (adjCombUp > 0)
						adjCombUp -= 1;
					else
						adjCombDown += 1;
					adjustCombinedRefresh();
				}
			}
			//OK
			else if ((x >= 270) && (x <= 320) && (y >= 190) && (y <= 240)) {
				//Preset chooser
				if (!firstStart) {
					if (adjustCombinedPresetSaveMenu())
						return;
					else
						adjustCombinedRefresh();
				}
				//First start, save as preset 1
				else {
					EEPROM.write(eeprom_adjComb1Left, adjCombLeft);
					EEPROM.write(eeprom_adjComb1Right, adjCombRight);
					EEPROM.write(eeprom_adjComb1Up, adjCombUp);
					EEPROM.write(eeprom_adjComb1Down, adjCombDown);
					EEPROM.write(eeprom_adjComb1Alpha, round(adjCombAlpha * 100.0));
					EEPROM.write(eeprom_adjComb1Set, eeprom_setValue);
					EEPROM.write(eeprom_adjCombPreset, adjComb_preset1);
					return;
				}
			}
			//Refresh
			else if ((x >= 50) && (x <= 270) && (y >= 50) && (y <= 210)) {
				adjustCombinedLoading();
				adjustCombinedRefresh();
			}
			//Exit without saving, not in first start
			else if ((x >= 270) && (x <= 320) && (y >= 0) && (y <= 50) && (!firstStart)) {
				//Ask if the user really wants to leave
				if (adjustCombinedLeave()) {
					readAdjustCombined();
					return;
				}
				else
					showFullMessage((char*) "Please wait..");
					adjustCombinedRefresh();
			}
		}
	}
}

/* Adjust combined new menu */
void adjustCombinedNewMenu(bool firstStart = false) {
	//Show loading message
	showFullMessage((char*)"Please wait..");
	//Prepare the preview image
	byte displayMode_old = displayMode;
	displayMode = displayMode_combined;
	changeCamRes(VC0706_160x120);
	//Load the defaults
	adjCombDown = 0;
	adjCombUp = 0;
	adjCombLeft = 0;
	adjCombRight = 0;
	adjCombAlpha = 0.5;
	//Show the preview image
	adjustCombinedRefresh(firstStart);
	//Run the handler and 
	adjustCombinedNewMenuHandler(firstStart);
	//Show message
	showFullMessage((char*) "Please wait..");
	//Restore the old mode
	displayMode = displayMode_old;
	if (displayMode == displayMode_thermal)
		changeCamRes(VC0706_640x480);
}

/* Switch the current temperature menu item */
void adjustCombinedString(int pos) {
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

/* Menu to save the adjust combined settings to a preset */
bool adjustCombinedMenu() {
	//Save the current position inside the menu
	byte adjCombMenuPos = 0;
	//Background
	mainMenuBackground();
	//Title
	mainMenuTitle((char*) "Adjust Combined");
	//Draw the selection menu
	drawSelectionMenu();
	//Draw the current item
	adjustCombinedString(adjCombMenuPos);
	//Save the current position inside the menu
	while (true) {
		//Touch screen pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//SELECT
			if (pressedButton == 3) {
				switch (adjCombMenuPos) {
					//New
				case 0:
					adjustCombinedNewMenu();
					return true;
					break;
					//Load Preset 1
				case 1:
					EEPROM.write(eeprom_adjCombPreset, adjComb_preset1);
					break;
					//Load Preset 2
				case 2:
					EEPROM.write(eeprom_adjCombPreset, adjComb_preset2);
					break;
					//Load Preset 3
				case 3:
					EEPROM.write(eeprom_adjCombPreset, adjComb_preset3);
					break;
				}
				//Read config from EEPROM
				readAdjustCombined();
				return true;
			}
			//BACKWARD
			else if (pressedButton == 0) {
				if (adjCombMenuPos > 0)
					adjCombMenuPos--;
				else if (adjCombMenuPos == 0)
					adjCombMenuPos = 3;
			}
			//FORWARD
			else if (pressedButton == 1) {
				if (adjCombMenuPos < 3)
					adjCombMenuPos++;
				else if (adjCombMenuPos == 3)
					adjCombMenuPos = 0;
			}
			//BACK
			else if (pressedButton == 2)
				return false;

			//Change the menu name
			adjustCombinedString(adjCombMenuPos);
		}
	}
}


/* Second Menu */
void secondMenu(bool firstStart) {
	drawTitle((char*) "Second", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 70, 70, (char*) "-");
	touchButtons.addButton(230, 60, 70, 70, (char*) "+");
	touchButtons.addButton(20, 150, 280, 70, (char*) "Back");
	touchButtons.drawButtons();
	drawCenterElement(second());
	//Touch handler
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//Minus
			if (pressedButton == 0) {
				if (second() >= 0) {
					if (second() > 0)
						setTime(hour(), minute(), second() - 1, day(), month(),
							year());
					else if (second() == 0)
						setTime(hour(), minute(), 59, day(), month(), year());
					drawCenterElement(second());
				}
			}
			//Plus
			else if (pressedButton == 1) {
				if (second() <= 59) {
					if (second() < 59)
						setTime(hour(), minute(), second() + 1, day(), month(),
							year());
					else if (second() == 59)
						setTime(hour(), minute(), 0, day(), month(), year());
					drawCenterElement(second());
				}
			}
			//Back
			else if (pressedButton == 2) {
				Teensy3Clock.set(now());
				timeMenu();
				break;
			}
		}
	}
}

/* Minute Menu */
void minuteMenu(bool firstStart) {
	drawTitle((char*) "Minute", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 70, 70, (char*) "-");
	touchButtons.addButton(230, 60, 70, 70, (char*) "+");
	touchButtons.addButton(20, 150, 280, 70, (char*) "Back");
	touchButtons.drawButtons();
	drawCenterElement(minute());
	//Touch handler
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//Minus
			if (pressedButton == 0) {
				if (minute() >= 0) {
					if (minute() > 0)
						setTime(hour(), minute() - 1, second(), day(), month(),
							year());
					else if (minute() == 0)
						setTime(hour(), 59, second(), day(), month(), year());
					drawCenterElement(minute());
				}
			}
			//Plus
			else if (pressedButton == 1) {
				if (minute() <= 59) {
					if (minute() < 59)
						setTime(hour(), minute() + 1, second(), day(), month(),
							year());
					else if (minute() == 59)
						setTime(hour(), 0, second(), day(), month(), year());
					drawCenterElement(minute());
				}
			}
			//Back
			else if (pressedButton == 2) {
				Teensy3Clock.set(now());
				timeMenu();
				break;
			}
		}
	}
}

/* Hour menu */
void hourMenu(bool firstStart) {
	drawTitle((char*) "Hour", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 70, 70, (char*) "-");
	touchButtons.addButton(230, 60, 70, 70, (char*) "+");
	touchButtons.addButton(20, 150, 280, 70, (char*) "Back");
	touchButtons.drawButtons();
	drawCenterElement(hour());
	//Touch handler
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//Minus
			if (pressedButton == 0) {
				if (hour() >= 0) {
					if (hour() > 0)
						setTime(hour() - 1, minute(), second(), day(), month(),
							year());
					else if (hour() == 0)
						setTime(23, minute(), second(), day(), month(), year());
					drawCenterElement(hour());
				}
			}
			//Plus
			else if (pressedButton == 1) {
				if (hour() <= 23) {
					if (hour() < 23)
						setTime(hour() + 1, minute(), second(), day(), month(),
							year());
					else if (hour() == 23)
						setTime(0, minute(), second(), day(), month(), year());
					drawCenterElement(hour());
				}
			}
			//Back
			else if (pressedButton == 2) {
				Teensy3Clock.set(now());
				timeMenu();
				break;
			}
		}
	}
}

/* Day Menu */
void dayMenu(bool firstStart) {
	drawTitle((char*) "Day", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 70, 70, (char*) "-");
	touchButtons.addButton(230, 60, 70, 70, (char*) "+");
	touchButtons.addButton(20, 150, 280, 70, (char*) "Back");
	touchButtons.drawButtons();
	drawCenterElement(day());
	//Touch handler
	while (true) {
		//touch press
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//Minus
			if (pressedButton == 0) {
				if (day() >= 1) {
					if (day() > 1)
						setTime(hour(), minute(), second(), day() - 1, month(),
							year());
					else if (day() == 1)
						setTime(hour(), minute(), second(), 31, month(),
							year());
					drawCenterElement(day());
				}
			}
			//Plus
			else if (pressedButton == 1) {
				if (day() <= 31) {
					if (day() < 31)
						setTime(hour(), minute(), second(), day() + 1, month(),
							year());
					else if (day() == 31)
						setTime(hour(), minute(), second(), 1, month(), year());
					drawCenterElement(day());
				}
			}
			//Back
			else if (pressedButton == 2) {
				Teensy3Clock.set(now());
				dateMenu();
				break;
			}
		}
	}
}

/* Month Menu */
void monthMenu(bool firstStart) {
	drawTitle((char*) "Month", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 70, 70, (char*) "-");
	touchButtons.addButton(230, 60, 70, 70, (char*) "+");
	touchButtons.addButton(20, 150, 280, 70, (char*) "Back");
	touchButtons.drawButtons();
	drawCenterElement(month());
	//Touch handler
	while (true) {
		//touch press
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//Minus
			if (pressedButton == 0) {
				if (month() >= 1) {
					if (month() > 1)
						setTime(hour(), minute(), second(), day(), month() - 1,
							year());
					else if (month() == 1)
						setTime(hour(), minute(), second(), day(), 12, year());
					drawCenterElement(month());
				}
			}
			//Plus
			else if (pressedButton == 1) {
				if (month() <= 12) {
					if (month() < 12)
						setTime(hour(), minute(), second(), day(), month() + 1,
							year());
					else if (month() == 12)
						setTime(hour(), minute(), second(), day(), 1, year());
					drawCenterElement(month());
				}
			}
			//Back
			else if (pressedButton == 2) {
				Teensy3Clock.set(now());
				dateMenu();
				break;
			}
		}
	}
}

/* Year Menu */
void yearMenu(bool firstStart) {
	drawTitle((char*) "Year", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 70, 70, (char*) "-");
	touchButtons.addButton(230, 60, 70, 70, (char*) "+");
	touchButtons.addButton(20, 150, 280, 70, (char*) "Back");
	touchButtons.drawButtons();
	drawCenterElement(year());
	//Touch handler
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//Minus
			if (pressedButton == 0) {
				if (year() > 2016) {
					setTime(hour(), minute(), second(), day(), month(),
						year() - 1);
					drawCenterElement(year());
				}
			}
			//Plus
			else if (pressedButton == 1) {
				if (year() < 2099) {
					setTime(hour(), minute(), second(), day(), month(),
						year() + 1);
					drawCenterElement(year());
				}
			}
			//Back
			else if (pressedButton == 2) {
				Teensy3Clock.set(now());
				dateMenu();
				break;
			}
		}
	}
}

/* Date Menu */
void dateMenu(bool firstStart) {
	drawTitle((char*) "Date", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 130, 70, (char*) "Day");
	touchButtons.addButton(170, 60, 130, 70, (char*) "Month");
	touchButtons.addButton(20, 150, 130, 70, (char*) "Year");
	touchButtons.addButton(170, 150, 130, 70, (char*) "Back");
	touchButtons.drawButtons();
}

/* Date Menu Handler */
void dateMenuHandler(bool firstStart) {
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons();
			//Day
			if (pressedButton == 0) {
				dayMenu(firstStart);
			}
			//Month
			else if (pressedButton == 1) {
				monthMenu(firstStart);
			}
			//Year
			else if (pressedButton == 2) {
				yearMenu(firstStart);
			}
			//Back
			else if (pressedButton == 3) {
				timeAndDateMenu(firstStart);
				break;
			}
		}
	}
}

/* Time Menu */
void timeMenu(bool firstStart) {
	drawTitle((char*) "Time", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 130, 70, (char*) "Hour");
	touchButtons.addButton(170, 60, 130, 70, (char*) "Minute");
	touchButtons.addButton(20, 150, 130, 70, (char*) "Second");
	touchButtons.addButton(170, 150, 130, 70, (char*) "Back");
	touchButtons.drawButtons();
}

/* Time Menu Handler */
void timeMenuHandler(bool firstStart = false) {
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons();
			//Hours
			if (pressedButton == 0) {
				hourMenu(firstStart);
			}
			//Minutes
			else if (pressedButton == 1) {
				minuteMenu(firstStart);
			}
			//Seconds
			else if (pressedButton == 2) {
				secondMenu(firstStart);
			}
			//Back
			else if (pressedButton == 3) {
				timeAndDateMenu(firstStart);
				break;
			}
		}
	}
}

/* Time & Date Menu */
void timeAndDateMenu(bool firstStart) {
	drawTitle((char*) "Time & Date", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 130, 70, (char*) "Time");
	touchButtons.addButton(170, 60, 130, 70, (char*) "Date");
	touchButtons.addButton(20, 150, 280, 70, (char*) "Save");
	if (firstStart)
		touchButtons.relabelButton(2, (char*) "Set", false);
	touchButtons.drawButtons();
}

/* Time & Date Menu Handler */
void timeAndDateMenuHandler(bool firstStart = false) {
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons();
			//Time
			if (pressedButton == 0) {
				timeMenu(firstStart);
				timeMenuHandler(firstStart);
			}
			//Date
			else if (pressedButton == 1) {
				dateMenu(firstStart);
				dateMenuHandler(firstStart);
			}
			//Save
			else if (pressedButton == 2) {
				if (firstStart) {
					if (year() < 2016) {
						showFullMessage((char*) "Year must be >= 2016 !");
						delay(1000);
						timeAndDateMenu(true);
					}
					else
						break;
				}
				else {
					settingsMenu();
					break;
				}
			}
		}
	}
}



/* Visual image selection menu */
void visualImageMenu(bool firstStart = false) {
	drawTitle((char*) "Visual image", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 130, 70, (char*) "Disabled");
	touchButtons.addButton(170, 60, 130, 70, (char*) "Save JPEG");
	if (firstStart) {
		touchButtons.addButton(20, 150, 280, 70, (char*) "Set");
		visualEnabled = true;
	}
	else
		touchButtons.addButton(20, 150, 280, 70, (char*) "Back");
	touchButtons.drawButtons();
	if (!visualEnabled)
		touchButtons.setActive(0);
	else
		touchButtons.setActive(1);
	//Touch handler
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons();
			//Disabled
			if (pressedButton == 0) {
				if (visualEnabled) {
					visualEnabled = false;
					touchButtons.setActive(0);
					touchButtons.setInactive(1);
				}
			}
			//Save JPEG
			else if (pressedButton == 1) {
				if (!visualEnabled) {
					visualEnabled = true;
					touchButtons.setActive(1);
					touchButtons.setInactive(0);
				}
			}
			//Save
			else if (pressedButton == 2) {
				//Write new settings to EEPROM
				EEPROM.write(eeprom_visualEnabled, visualEnabled);
				if (firstStart)
					return;
				else {
					storageMenu();
				}
				break;
			}
		}
	}
}

/* Convert image selection menu */
void convertImageMenu(bool firstStart = false) {
	drawTitle((char*) "Convert image", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 130, 70, (char*) "DAT only");
	touchButtons.addButton(170, 60, 130, 70, (char*) "BMP & DAT");
	if (firstStart) {
		touchButtons.addButton(20, 150, 280, 70, (char*) "Set");
		convertEnabled = true;
	}
	else
		touchButtons.addButton(20, 150, 280, 70, (char*) "Back");
	touchButtons.drawButtons();
	if (!convertEnabled)
		touchButtons.setActive(0);
	else
		touchButtons.setActive(1);

	//Touch handler
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons();
			//DAT only
			if (pressedButton == 0) {
				if (convertEnabled) {
					convertEnabled = false;
					touchButtons.setActive(0);
					touchButtons.setInactive(1);
				}
			}
			//BMP & DAT
			else if (pressedButton == 1) {
				if (!convertEnabled) {
					convertEnabled = true;
					touchButtons.setActive(1);
					touchButtons.setInactive(0);
				}
			}

			//Save
			else if (pressedButton == 2) {
				//Write new settings to EEPROM
				EEPROM.write(eeprom_convertEnabled, convertEnabled);
				if (firstStart)
					return;
				else {
					storageMenu();
				}
				break;
			}
		}
	}
}

/* Asks the user if he really wants to format */
void formatStorage() {
	//Old HW generation, check SD card
	if (mlx90614Version == mlx90614Version_old) {
		showFullMessage((char*) "Checking SD card..");
		if (!checkSDCard()) {
			storageMenu();
			return;
		}
	}
	//Title & Background
	drawTitle((char*) "Storage");
	display.setColor(VGA_BLACK);
	display.setFont(smallFont);
	display.setBackColor(200, 200, 200);
	display.print((char*)"Do you really want to format ?", CENTER, 66);
	display.print((char*)"This will delete all images", CENTER, 105);
	display.print((char*)"and videos on the internal storage.", CENTER, 125);
	//Draw the buttons
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(bigFont);
	touchButtons.addButton(165, 160, 140, 55, (char*) "Yes");
	touchButtons.addButton(15, 160, 140, 55, (char*) "No");
	touchButtons.drawButtons();
	touchButtons.setTextFont(smallFont);
	//Touch handler
	while (true) {
		//If touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//YES
			if (pressedButton == 0) {
				showFullMessage((char*) "Formatting storage..", true);
				formatCard();
				showFullMessage((char*) "Format finished !", true);
				delay(1000);
				refreshFreeSpace();
				break;
			}
			//NO
			else if (pressedButton == 1) {
				break;
			}
		}
	}
	//Go back to the storage menu
	storageMenu();
}

/* Storage menu handler*/
void storageMenuHandler() {
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons();
			//Convert image
			if (pressedButton == 0) {
				convertImageMenu();
			}
			//Visual image
			else if (pressedButton == 1) {
				visualImageMenu();
			}
			//Format
			else if (pressedButton == 2) {
				formatStorage();
			}
			//Back
			else if (pressedButton == 3) {
				settingsMenu();
				break;
			}
		}
	}
}

/* Storage menu */
void storageMenu() {
	drawTitle((char*) "Storage");
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 130, 70, (char*) "Convert image");
	touchButtons.addButton(170, 60, 130, 70, (char*) "Visual image");
	touchButtons.addButton(20, 150, 130, 70, (char*) "Format");
	touchButtons.addButton(170, 150, 130, 70, (char*) "Back");
	touchButtons.drawButtons();
}

/* Temperature format menu */
void tempFormatMenu(bool firstStart = false) {
	drawTitle((char*) "Temp. Format", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 130, 70, (char*) "Celcius");
	touchButtons.addButton(170, 60, 130, 70, (char*) "Fahrenheit");
	touchButtons.addButton(20, 150, 280, 70, (char*) "Save");
	if (firstStart) {
		touchButtons.relabelButton(2, (char*) "Set", false);
		tempFormat = tempFormat_celcius;
	}
	touchButtons.drawButtons();
	if (tempFormat == tempFormat_celcius)
		touchButtons.setActive(tempFormat_celcius);
	else
		touchButtons.setActive(tempFormat_fahrenheit);
	//Touch handler
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons();
			//Celcius
			if (pressedButton == 0) {
				if (tempFormat == tempFormat_fahrenheit) {
					tempFormat = tempFormat_celcius;
					touchButtons.setActive(0);
					touchButtons.setInactive(1);
				}
			}
			//Fahrenheit
			else if (pressedButton == 1) {
				if (tempFormat == tempFormat_celcius) {
					tempFormat = tempFormat_fahrenheit;
					touchButtons.setActive(1);
					touchButtons.setInactive(0);
				}
			}
			//Save
			else if (pressedButton == 2) {
				//Write new settings to EEPROM
				EEPROM.write(eeprom_tempFormat, tempFormat);
				if (firstStart)
					return;
				else {
					displayMenu();
				}
				break;
			}
		}
	}
}

/* Rotate display menu */
void rotateDisplayMenu(bool firstStart = false) {
	drawTitle((char*) "Disp. rotation", firstStart);
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 130, 70, (char*) "Enabled");
	touchButtons.addButton(170, 60, 130, 70, (char*) "Disabled");
	touchButtons.addButton(20, 150, 280, 70, (char*) "Save");
	if (firstStart)
		touchButtons.relabelButton(2, (char*) "Set", false);
	touchButtons.drawButtons();
	if (rotationEnabled)
		touchButtons.setActive(0);
	else
		touchButtons.setActive(1);
	//Touch handler
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons();
			//Enabled
			if (pressedButton == 0) {
				if (!rotationEnabled) {
					rotationEnabled = true;
					touchButtons.setActive(0);
					touchButtons.setInactive(1);
				}
			}
			//Disabled
			else if (pressedButton == 1) {
				if (rotationEnabled) {
					rotationEnabled = false;
					touchButtons.setActive(1);
					touchButtons.setInactive(0);
				}
			}
			//Save
			else if (pressedButton == 2) {
				//Write new settings to EEPROM
				EEPROM.write(eeprom_rotationEnabled, rotationEnabled);
				if (firstStart)
					return;
				else {
					setDisplayRotation();
					displayMenu();
				}
				break;
			}
		}
	}
}

/* Screen timeout menu */
void screenTimeoutMenu() {
	drawTitle((char*) "Screen timeout");
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 130, 70, (char*) "Disabled");
	touchButtons.addButton(170, 60, 130, 70, (char*) "5 Min.");
	touchButtons.addButton(20, 150, 130, 70, (char*) "20 Min.");
	touchButtons.addButton(170, 150, 130, 70, (char*) "Back");
	touchButtons.drawButtons();
	//Set current one active
	touchButtons.setActive(screenOffTime);
	//Touch handler
	while (true) {
		//Touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons();
			//Set to new color
			if ((pressedButton == 0) || (pressedButton == 1) || (pressedButton == 2)) {
				touchButtons.setInactive(screenOffTime);
				screenOffTime = pressedButton;
				touchButtons.setActive(screenOffTime);
			}
			//Save
			else if (pressedButton == 3) {
				//Write new settings to EEPROM
				EEPROM.write(eeprom_screenOffTime, screenOffTime);
				//Init timer
				initScreenOffTimer();
				//Return to display menu
				displayMenu();
				break;
			}
		}
	}
}


/* Display menu handler*/
void displayMenuHandler() {
	while (true) {
		//touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons();
			//Temp. format
			if (pressedButton == 0) {
				tempFormatMenu();
			}
			//Rotate display
			else if (pressedButton == 1) {
				rotateDisplayMenu();
			}
			//Screen timeout
			else if (pressedButton == 2) {
				screenTimeoutMenu();
			}
			//Back
			else if (pressedButton == 3) {
				settingsMenu();
				break;
			}
		}
	}
}

/* Display menu */
void displayMenu() {
	drawTitle((char*) "Display");
	touchButtons.deleteAllButtons();
	touchButtons.addButton(20, 60, 130, 70, (char*) "Temp. format");
	touchButtons.addButton(170, 60, 130, 70, (char*) "Disp. rotation");
	touchButtons.addButton(20, 150, 130, 70, (char*) "Screen timeout");
	touchButtons.addButton(170, 150, 130, 70, (char*) "Back");
	touchButtons.drawButtons();

}

/* Touch handler for the settings menu */
void settingsMenuHandler() {
	while (1) {
		//touch press
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons();
			//Display
			if (pressedButton == 0) {
				displayMenu();
				displayMenuHandler();
			}
			//Storage
			else if (pressedButton == 1) {
				storageMenu();
				storageMenuHandler();
			}
			//Time & Date
			else if (pressedButton == 2) {
				timeAndDateMenu();
				timeAndDateMenuHandler();
			}
			//Back
			else if (pressedButton == 3)
				break;
		}
	}
}

/* Settings menu main screen */
void settingsMenu() {
	drawTitle((char*) "Settings");
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(smallFont);
	touchButtons.addButton(20, 60, 130, 70, (char*) "Display");
	touchButtons.addButton(170, 60, 130, 70, (char*) "Storage");
	touchButtons.addButton(20, 150, 130, 70, (char*) "Time & Date");
	touchButtons.addButton(170, 150, 130, 70, (char*) "Back");
	touchButtons.drawButtons();
}