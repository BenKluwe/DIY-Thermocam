/*
*
* VIDEO MENU - Record single frames or time interval videos
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

/* Variables */

//Video save interval in seconds
int16_t videoInterval;

/* Methods */

/* Switch the video interval string*/
void videoIntervalString(int pos) {
	char* text = (char*) "";
	switch (pos) {
		//1 second
	case 0:
		text = (char*) "1 second";
		break;
		//5 seconds
	case 1:
		text = (char*) "5 seconds";
		break;
		//10 seconds
	case 2:
		text = (char*) "10 seconds";
		break;
		//20 seconds
	case 3:
		text = (char*) "20 seconds";
		break;
		//30 seconds
	case 4:
		text = (char*) "30 seconds";
		break;
		//1 minute
	case 5:
		text = (char*) "1 minute";
		break;
		//5 minutes
	case 6:
		text = (char*) "5 minutes";
		break;
		//10 minutes
	case 7:
		text = (char*) "10 minutes";
		break;
	}
	//Draws the current selection
	mainMenuSelection(text);
}

/* Touch Handler for the video interval chooser */
bool videoIntervalHandler(byte* pos) {
	//Main loop
	while (true) {
		//Touch screen pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);
			//SELECT
			if (pressedButton == 3) {
				switch (*pos) {
					//1 second
				case 0:
					videoInterval = 1;
					break;
					//5 seconds
				case 1:
					videoInterval = 5;
					break;
					//10 seconds
				case 2:
					videoInterval = 10;
					break;
					//20 seconds
				case 3:
					videoInterval = 20;
					break;
					//30 seconds
				case 4:
					videoInterval = 30;
					break;
					//1 minute
				case 5:
					videoInterval = 60;
					break;
					//5 minutes
				case 6:
					videoInterval = 300;
					break;
					//10 minutes
				case 7:
					videoInterval = 600;
					break;
				}
				return true;
			}
			//BACK
			else if (pressedButton == 2) {
				return false;
			}
			//BACKWARD
			else if (pressedButton == 0) {
				if (*pos > 0)
					*pos = *pos - 1;
				else if (*pos == 0)
					*pos = 7;
			}
			//FORWARD
			else if (pressedButton == 1) {
				if (*pos < 7)
					*pos = *pos + 1;
				else if (*pos == 7)
					*pos = 0;
			}
			//Change the menu name
			videoIntervalString(*pos);
		}
	}
}

/* Start video menu to choose interval */
bool videoIntervalChooser() {
	bool rtn = 0;
	static byte videoIntervalPos = 0;
	//Background
	mainMenuBackground();
	//Title
	mainMenuTitle((char*) "Choose interval");
	//Draw the selection menu
	drawSelectionMenu();
	//Current choice name
	videoIntervalString(videoIntervalPos);
	//Touch handler - return true if exit to Main menu, otherwise false
	rtn = videoIntervalHandler(&videoIntervalPos);
	//Restore old fonts
	display.setFont(smallFont);
	touchButtons.setTextFont(smallFont);
	//Delete the old buttons
	touchButtons.deleteAllButtons();
	return rtn;
}

/* Display the video capture screen contents */
void refreshCapture() {
	//Apply low-pass filter
	if (filterType == filterType_box)
		boxFilter();
	else if (filterType == filterType_gaussian)
		gaussianFilter();

	//Convert lepton data to RGB565 colors
	convertColors();

	//Display additional infos
	displayInfos();

	//Draw thermal image on screen
	display.writeScreen(image);
}

/* Captures video frames in an interval */
void videoCaptureInterval(int16_t* remainingTime, uint16_t* framesCaptured, char* dirname) {
	//Measure time it takes to display everything
	long measure = millis();
	char buffer[30];

	//If there is no more time or the first frame, capture it
	if ((*remainingTime == 0) || (*framesCaptured == 0)) {

		//Send capture command to camera if activated and there is enough time
		if ((visualEnabled) && (videoInterval >= 10))
			captureVisualImage();

		//Save video raw frame
		saveRawData(false, dirname, *framesCaptured);

		//Refresh capture
		refreshCapture();
		//Display title
		display.setFont(bigFont);
		display.print((char*) "Interval capture", CENTER, 20);

		//Save visual image if activated
		if ((visualEnabled) && (videoInterval >= 10)) {
			//Display message
			sprintf(buffer, "Saving thermal + visual now!");
			display.setFont(smallFont);
			display.print(buffer, CENTER, 210);
			//Create filename to save data
			char filename[] = "00000.JPG";
			frameFilename(filename, *framesCaptured);
			//Create file
			createJPGFile(filename, dirname);
			//Save visual image
			saveVisualImage();
			//Reset counter
			int16_t elapsed = (millis() - measure) / 1000;
			*remainingTime = videoInterval - elapsed;
		}
		else {
			*remainingTime = videoInterval;
			//Display message
			sprintf(buffer, "Saving thermal frame now!");
			display.setFont(smallFont);
			display.print(buffer, CENTER, 210);
		}

		//Raise capture counter
		*framesCaptured = *framesCaptured + 1;
	}

	//Show waiting time
	else {
		//Refresh display content
		refreshCapture();
		//Display title
		display.setFont(bigFont);
		display.print((char*) "Interval capture", CENTER, 20);
		//Display remaining time message
		sprintf(buffer, "Saving next frame in %d second(s)", *remainingTime);
		display.setFont(smallFont);
		display.print(buffer, CENTER, 210);
		//Wait rest of the time
		measure = millis() - measure;
		if (measure < 1000)
			delay(1000 - measure);
		//Decrease remaining time by one
		*remainingTime -= 1;
	}
}

/* This screen is shown during the video capture */
void videoCapture() {
	//Help variables
	char dirname[20];
	char buffer[30];
	int16_t delayTime = videoInterval;
	uint16_t framesCaptured = 0;

	//Update display
	showFullMessage((char*) "Please wait..");

	//Create folder 
	createVideoFolder(dirname);

	//For normal video, display screen content
	if (videoInterval == 0) {
		display.fillScr(200, 200, 200);
		display.setBackColor(200, 200, 200);
		display.setFont(bigFont);
		display.setColor(VGA_BLUE);
		display.print((char*)"Recording video", CENTER, 30);
		display.setFont(smallFont);
		display.setColor(VGA_BLACK);
		display.print((char*)"Press push button to stop capture", CENTER, 80);
		display.print((char*)"Touch to toggle screen backlight", CENTER, 120);
		sprintf(buffer, "Frames captured: %5d", framesCaptured);
		display.print(buffer, CENTER, 160);
		sprintf(buffer, "Folder name: %s", dirname);
		display.print(buffer, CENTER, 200);
	}
	else {
		//Set text colors
		setTextColor();
		display.setBackColor(VGA_TRANSPARENT);
	}

	//Main loop
	while (videoSave) {

		//Touch - turn display on or off
		if (!digitalRead(pin_touch_irq)) {
			digitalWrite(pin_lcd_backlight, !(checkScreenLight()));
			while (!digitalRead(pin_touch_irq));
		}

		//Button press - stop capture
		if (extButtonPressed())
			break;

		//Receive the temperatures over SPI
		getTemperatures();
		//Compensate calibration with object temp
		compensateCalib();

		//Refresh the temp points if required
		if (pointsEnabled)
			refreshTempPoints();

		//Find min and max if not in manual mode and limits not locked
		if ((autoMode) && (!limitsLocked))
			limitValues();

		//Normal  mode
		if (videoInterval == 0) {
			//Save video raw frame
			saveRawData(false, dirname, framesCaptured);
			//Raise capture counter
			framesCaptured = framesCaptured + 1;
			//Display current frames captured
			sprintf(buffer, "Frames captures: %5d", framesCaptured);
			display.print(buffer, CENTER, 160);
		}
		//Time interval save
		else {
			videoCaptureInterval(&delayTime, &framesCaptured, dirname);
		}

		//Maximum amount of frames reached or disk space out, return
		if ((framesCaptured == 65535) || (getSDSpace() < 1000)) {
			showFullMessage((char*) "Not enough space, abort!");
			break;
		}
	}

	//Turn the display on if it was off before
	if (!checkScreenLight())
		enableScreenLight();

	//Post processing for interval videos if enabled
	if (framesCaptured > 0) {
		videoSave = true;
		//Ask the user if he wants to convert that video
		if (convertPrompt()) {
			showFullMessage((char*) "Capture finished ! Converting..");
			delay(1000);
			//Process video frames
			proccessVideoFrames(framesCaptured, dirname);
		}
	}

	//Show finished message
	else {
		showFullMessage((char*) "Video capture finished !");
		delay(1000);
	}

	//Disable mode
	videoSave = false;
}

/* Video mode, choose intervall or normal */
bool videoMode() {
	//Show message that video is only possible in thermal mode
	if (displayMode != displayMode_thermal) {
		showFullMessage((char*) "Video only possible in thermal mode!");
		//Disable mode
		videoSave = false;
		return false;
	}
	//For old HW, check if the SD card is there
	if (mlx90614Version == mlx90614Version_old) {
		if (!checkSDCard()) {
			//Disable mode
			videoSave = false;
			return false;
		}
	}
	//Check if there is at least 1MB of space left
	if (getSDSpace() < 1000) {
		showFullMessage((char*) "Int. space full!");
		//Disable mode
		videoSave = false;
		return false;
	}

redraw:
	//Title & Background
	mainMenuBackground();
	mainMenuTitle((char*)"Video mode");
	//Draw the buttons
	touchButtons.deleteAllButtons();
	touchButtons.setTextFont(bigFont);
	touchButtons.addButton(15, 47, 140, 120, (char*) "Normal");
	touchButtons.addButton(165, 47, 140, 120, (char*) "Interval");
	touchButtons.addButton(15, 188, 140, 40, (char*) "Back");
	touchButtons.drawButtons();

	//Touch handler
	while (true) {
		//If touch pressed
		if (touch.touched() == true) {
			int pressedButton = touchButtons.checkButtons(true);

			//Normal
			if (pressedButton == 0) {
				//Set video interval to zero, means normal
				videoInterval = 0;
				//Start capturing a video
				videoCapture();
				break;
			}

			//Interval
			else if (pressedButton == 1) {
				//Choose the time interval
				if (!videoIntervalChooser())
					//Redraw video mode if user pressed back
					goto redraw;
				//Start capturing a video
				videoCapture();
				break;
			}

			//Back
			else if (pressedButton == 2) {
				//Disable mode
				videoSave = false;
				return false;
			}
		}
	}
	return true;
}