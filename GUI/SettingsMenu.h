/*
* Settings menu to adjust settings
*/


/* Draws a loading box for the combined image */
void adjustCombinedLoading() {
	//Fill replacement
	display.setColor(VGA_WHITE);
	display.fillRect(80, 50, 240, 170);
	//Draw the border for the preview image
	display.setColor(VGA_BLACK);
	display.setBackColor(VGA_WHITE);
	display.print((char*) "Please wait..", CENTER, 100);
}

/* Touch handler for the adjust combined menu */
void adjustCombinedMenuHandler() {
	//Touch handler
	while (true) {
		//Check for touch
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//Increment
			if (pressedButton == 0) {
				if (adjCombFactor < 1.0) {
					adjustCombinedLoading();
					adjCombFactor += 0.05;
					createVisCombImg();
					display.drawBitmap(80, 50, 160, 120, image, 1);
				}
			}
			//Decrement
			else if (pressedButton == 1) {
				if (adjCombFactor > 0.5) {
					adjustCombinedLoading();
					adjCombFactor -= 0.05;
					createVisCombImg();
					display.drawBitmap(80, 50, 160, 120, image, 1);
				}
			}
			//Left
			else if (pressedButton == 2) {
				if (adjCombLeft < 10) {
					adjustCombinedLoading();
					if (adjCombRight > 0)
						adjCombRight -= 1;
					else
						adjCombLeft += 1;
					createVisCombImg();
					display.drawBitmap(80, 50, 160, 120, image, 1);
				}
			}
			//Right
			else if (pressedButton == 3) {
				if (adjCombRight < 10) {
					adjustCombinedLoading();
					if (adjCombLeft > 0)
						adjCombLeft -= 1;
					else
						adjCombRight += 1;
					createVisCombImg();
					display.drawBitmap(80, 50, 160, 120, image, 1);
				}
			}
			//Up
			else if (pressedButton == 4) {
				if (adjCombUp < 10) {
					adjustCombinedLoading();
					if (adjCombDown > 0)
						adjCombDown -= 1;
					else
						adjCombUp += 1;
					createVisCombImg();
					display.drawBitmap(80, 50, 160, 120, image, 1);
				}
			}
			//Down
			else if (pressedButton == 5) {
				if (adjCombDown < 10) {
					adjustCombinedLoading();
					if (adjCombUp > 0)
						adjCombUp -= 1;
					else
						adjCombDown += 1;
					createVisCombImg();
					display.drawBitmap(80, 50, 160, 120, image, 1);
				}
			}
			//OK
			else if (pressedButton == 6) {
				//Save values to EEPROM
				EEPROM.write(eeprom_adjCombLeft, adjCombLeft);
				EEPROM.write(eeprom_adjCombRight, adjCombRight);
				EEPROM.write(eeprom_adjCombUp, adjCombUp);
				EEPROM.write(eeprom_adjCombDown, adjCombDown);
				EEPROM.write(eeprom_adjCombFactor, round(adjCombFactor * 100.0));
				EEPROM.write(eeprom_adjCombSet, eeprom_setValue);
				//Go back
				return;
			}
			//Reset
			else if (pressedButton == 7) {
				adjustCombinedLoading();
				adjCombLeft = 0;
				adjCombRight = 0;
				adjCombUp = 0;
				adjCombDown = 0;
				adjCombFactor = 1.0;
				createVisCombImg();
				display.drawBitmap(80, 50, 160, 120, image, 1);
			}
		}
	}
}

/* Adjust combined menu */
void adjustCombinedMenu(bool firstStart) {
	//Draw the title
	if (firstStart)
		display.fillScr(127, 127, 127);
	else {
		display.setColor(127, 127, 127);
		display.fillRoundRect(6, 6, 314, 234);
	}
	display.setFont(bigFont);
	display.setBackColor(127, 127, 127);
	display.setColor(VGA_WHITE);
	display.print((char*) "Align combined", CENTER, 18);
	display.setFont(smallFont);
	//Buttons
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(smallFont);
	touchButtons.addButton(15, 180, 55, 40, (char*) "Inc.");
	touchButtons.addButton(250, 180, 55, 40, (char*) "Dec.");
	touchButtons.addButton(15, 115, 55, 55, (char*) "Left");
	touchButtons.addButton(250, 115, 55, 55, (char*) "Right");
	touchButtons.addButton(15, 50, 55, 55, (char*) "Up");
	touchButtons.addButton(250, 50, 55, 55, (char*) "Down");
	touchButtons.addButton(165, 180, 75, 40, (char*) "OK");
	touchButtons.addButton(80, 180, 75, 40, (char*) "Reset");
	touchButtons.drawButtons();
	//Fill replacement
	adjustCombinedLoading();
	//Draw Border
	display.drawRect(79, 49, 241, 171);
	//Prepare the preview image
	byte displayMode_old = displayMode;
	displayMode = displayMode_combined;
	combinedDecomp = true;
	changeCamRes(VC0706_160x120);
	createVisCombImg();
	//Display the preview image
	display.drawBitmap(80, 50, 160, 120, image, 1);
	//Run the handler
	adjustCombinedMenuHandler();
	//Restore the old mode
	if (displayMode != displayMode_combined)
		combinedDecomp = false;
	if (displayMode == displayMode_thermal)
		changeCamRes(VC0706_640x480);
	displayMode = displayMode_old;
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
						drawMessage((char*) "Year must be >= 2016 !");
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
		drawMessage((char*) "Checking SD card..");
		if (!checkSDCard()) {
			storageMenu();
			return;
		}
	}
	//Title & Background
	drawTitle((char*) "Storage");
	display.setColor(VGA_WHITE);
	display.setFont(smallFont);
	display.setBackColor(127, 127, 127);
	display.print((char*)"Do you really want to format ?", CENTER, 66);
	display.print((char*)"This will delete all images", CENTER, 105);
	display.print((char*)"and videos on the internal storage.", CENTER, 125);
	//Draw the buttons
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(bigFont);
	touchButtons.addButton(15, 160, 140, 55, (char*) "No");
	touchButtons.addButton(165, 160, 140, 55, (char*) "Yes");
	touchButtons.drawButtons();
	touchButtons.setTextFont(smallFont);
	//Touch handler
	while (true) {
		//If touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//YES
			if (pressedButton == 1) {
				drawMessage((char*) "Formatting storage..");
				formatCard();
				drawMessage((char*) "Format finished !");
				delay(1000);
				refreshFreeSpace();
				break;
			}
			//NO
			else if (pressedButton == 0) {
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
	touchButtons.addButton(170, 60, 130, 70, (char*) "10 Min.");
	touchButtons.addButton(20, 150, 130, 70, (char*) "30 Min.");
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