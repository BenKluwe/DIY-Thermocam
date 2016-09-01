/*
* Save images and videos to the internal storage
*/

/* Creates a filename from the current time & date */
void createSDName(char* filename, bool folder) {
	char buffer[5];
	//Year
	itoa(year(), buffer, 10);
	strncpy(&filename[0], buffer, 4);
	//Month
	itoa(month(), buffer, 10);
	if (month() < 10) {
		filename[4] = '0';
		strncpy(&filename[5], buffer, 1);
	}
	else {
		strncpy(&filename[4], buffer, 2);
	}
	//Day
	itoa(day(), buffer, 10);
	if (day() < 10) {
		filename[6] = '0';
		strncpy(&filename[7], buffer, 1);
	}
	else {
		strncpy(&filename[6], buffer, 2);
	}
	//Hour
	itoa(hour(), buffer, 10);
	if (hour() < 10) {
		filename[8] = '0';
		strncpy(&filename[9], buffer, 1);
	}
	else {
		strncpy(&filename[8], buffer, 2);
	}
	//Minute
	itoa(minute(), buffer, 10);
	if (minute() < 10) {
		filename[10] = '0';
		strncpy(&filename[11], buffer, 1);
	}
	else {
		strncpy(&filename[10], buffer, 2);
	}
	//Second
	itoa(second(), buffer, 10);
	if (second() < 10) {
		filename[12] = '0';
		if (!folder)
			strncpy(&filename[13], buffer, 1);
		else
			strcpy(&filename[13], buffer);
	}
	else {
		if (!folder)
			strncpy(&filename[12], buffer, 2);
		else
			strcpy(&filename[12], buffer);
	}
}

/* Checks the requirements for image save */
void checkImageSave() {
	//Old hardware - init SD card
	if (mlx90614Version == mlx90614Version_old) {
		if (!checkSDCard()) {
			imgSave = imgSave_disabled;
			return;
		}
	}

	//Check if there is at least 1MB of space left
	if (getSDSpace() < 1000) {
		showMsg((char*) "Int. space full!");
		imgSave = imgSave_disabled;
		return;
	}
}

/* Shows on the screen that we saved an image */
void showMsg(char* msg, bool bottom) {
	//Set Text Color
	setTextColor();
	//set Background transparent
	display.setBackColor(VGA_TRANSPARENT);
	//Give the user a hint that it tries to save
	display.setFont(bigFont);
	if (bottom)
		display.print(msg, CENTER, 160);
	else
		display.print(msg, CENTER, 80);
	display.setFont(smallFont);
	//Wait some time to read the text
	delay(1000);
}

/* Creates a bmp file for the thermal image */
void createBMPFile(char* filename) {
	//File extension and open
	strcpy(&filename[14], ".BMP");
	sdFile.open(filename, O_RDWR | O_CREAT | O_AT_END);
	//640 x 480 file header
	const char bmp_header[66] = { 0x42, 0x4D, 0x36, 0x60, 0x09, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
		0x80, 0x02, 0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0x01, 0x00, 0x10,
		0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x60, 0x09, 0x00, 0xC4, 0x0E,
		0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00,
		0x1F, 0x00, 0x00, 0x00 };
	//Write the BMP header
	for (int i = 0; i < 66; i++) {
		char ch = bmp_header[i];
		sdFile.write((uint8_t*)&ch, 1);
	}
}

/* Creates a jpg file for the visual image */
void createJPGFile(char* filename, char* dirname) {
	//Begin SD Transmission
	startAltClockline(true);
	//Go into the video folder if required
	if (dirname != NULL)
		sd.chdir(dirname);
	//File extension and open
	strcpy(&filename[14], ".JPG");
	sdFile.open(filename, O_RDWR | O_CREAT | O_AT_END);
	//Switch clockline
	endAltClockline();
}

/* Creates a folder for the video capture */
void createVideoFolder(char* dirname) {
	startAltClockline(true);
	//Build the dir from the current time & date
	createSDName(dirname, true);
	//Create folder
	sd.mkdir(dirname);
	//Go into that folder
	sd.chdir(dirname);
	endAltClockline();
}

/* Save video frame to image file */
void saveVideoFrame(char* filename, char* dirname) {
	//Begin SD Transmission
	startAltClockline(true);
	//Switch to video folder 
	sd.chdir(dirname);
	// Open the file for writing
	sdFile.open(filename, O_RDWR | O_CREAT | O_AT_END);
	//Header for frame content
	const char bmp_header[66] = { 0x42, 0x4D, 0x36, 0x58, 0x02, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00,
		0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01,
		0x00, 0x10, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x58, 0x02,
		0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00,
		0x00, 0xE0, 0x07, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00 };
	//Write the BMP header
	for (int i = 0; i < 66; i++) {
		char ch = bmp_header[i];
		sdFile.write((uint8_t*)&ch, 1);
	}
	//Help variables for saving
	unsigned short pixel;
	//Allocate space for sd buffer
	uint8_t* sdBuffer = (uint8_t*)calloc(640, sizeof(uint8_t));
	//Save 320*60 pixels from the screen at one time
	for (int i = 3; i >= 0; i--) {
		endAltClockline();
		//Read pixels from the display
		display.readScreen(i, image);
		startAltClockline();
		for (byte y = 0; y < 60; y++) {
			//Write them into the sd buffer
			for (uint16_t x = 0; x < 320; x++) {
				pixel = image[((59 - y) * 320) + x];
				sdFile.write(pixel & 0x00FF);
				sdFile.write((pixel & 0xFF00) >> 8);
			}
		}
	}
	//De-allocate space
	free(sdBuffer);
	//Close the file
	sdFile.close();
	//Switch Clock back to Standard
	endAltClockline();
}

/* Saves a thermal or combined image to the sd card */
void saveDisplayImage(char* filename, char* dirname) {
	//Begin SD Transmission
	startAltClockline(true);
	//Switch to video folder if video
	if (dirname != NULL)
		sd.chdir(dirname);
	//Create file
	createBMPFile(filename);
	//Allocate space for sd buffer
	uint8_t* sdBuffer = (uint8_t*)calloc(640, sizeof(uint8_t));
	unsigned short pixel;
	//Save 320*60 pixels from the screen at one time
	for (int i = 3; i >= 0; i--) {
		endAltClockline();
		//Read pixels from the display
		display.readScreen(i, image);
		startAltClockline();
		for (byte y = 0; y < 60; y++) {
			//Write them into the sd buffer
			for (uint16_t x = 0; x < 320; x++) {
				pixel = image[((59 - y) * 320) + x];
				sdBuffer[x * 2] = pixel & 0x00FF;
				sdBuffer[(x * 2) + 1] = (pixel & 0xFF00) >> 8;
			}
			//Write them to the sd card with 640x480 resolution
			for (int i = 0; i < 2; i++) {
				for (uint16_t x = 0; x < 320; x++) {
					sdFile.write(sdBuffer[x * 2]);
					sdFile.write(sdBuffer[(x * 2) + 1]);
					sdFile.write(sdBuffer[x * 2]);
					sdFile.write(sdBuffer[(x * 2) + 1]);
				}
			}
		}
	}
	//De-allocate space
	free(sdBuffer);
	//Close file
	sdFile.close();
	//End SD Transmission
	endAltClockline();
}

/* Saves images to the internal storage */
void saveImage() {
	//Detach the interrupts
	detachInterrupts();

	//Capture image command if we are in thermal mode
	if ((visualEnabled == true) && (displayMode == displayMode_thermal))
		captureVisualImage();

	//Save Bitmap image if activated or in visual / combined mode
	if ((convertEnabled == true) || (displayMode == displayMode_visual) || (displayMode == displayMode_combined))
		saveDisplayImage(saveFilename);

	//Eventually save optical image
	if ((visualEnabled == true) && (displayMode == displayMode_thermal)) {
		//Create file
		createJPGFile(saveFilename);
		//Display message
		showMsg((char*) "Save Visual JPG..");
		//For old hardware, save mirrored image
		if (mlx90614Version == mlx90614Version_old) {
			changeCamRes(VC0706_160x120);
			createVisCombImg();
			display.writeScreen(image);
			saveDisplayImage(saveFilename);
			changeCamRes(VC0706_640x480);
		}
		else
			//Save visual image in full-res for new HW
			saveVisualImage();
	}

	//Show Message on screen
	if (((visualEnabled == true) || (convertEnabled)) && (displayMode == displayMode_thermal))
		showMsg((char*) "All saved!", true);
	else if (displayMode == displayMode_thermal)
		showMsg((char*) "Thermal Raw saved!");
	else if (displayMode == displayMode_visual)
		showMsg((char*) "Visual BMP saved!");
	else if (displayMode == displayMode_combined)
		showMsg((char*) "Combined BMP saved!");

	//Disable image save marker
	imgSave = imgSave_disabled;
	//Re-attach the interrupts
	attachInterrupts();
}

/* Converts a float to four bytes */
void floatToBytes(uint8_t* farray, float val)
{
	unsigned long d = *(unsigned long *)&val;
	farray[0] = d & 0x00FF;
	farray[1] = (d & 0xFF00) >> 8;
	farray[2] = (d & 0xFF0000) >> 16;
	farray[3] = (d & 0xFF000000) >> 24;
}

/* Converts four bytes back to float */
float bytesToFloat(uint8_t* farray)
{
	unsigned long d;
	d = (farray[3] << 24) | (farray[2] << 16)
		| (farray[1] << 8) | (farray[0]);
	float val = *(float *)&d;
	return val;
}

/* Creates the filename for the video frames */
void frameFilename(char* filename, uint16_t count) {
	filename[0] = '0' + count / 10000 % 10;
	filename[1] = '0' + count / 1000 % 10;
	filename[2] = '0' + count / 100 % 10;
	filename[3] = '0' + count / 10 % 10;
	filename[4] = '0' + count % 10;
}

/* Proccess video frames */
void proccessVideoFrames(uint16_t framesCaptured, char* dirname) {
	char buffer[14];
	char filename[] = "00000.DAT";
	display.setBackColor(VGA_TRANSPARENT);
	for (uint16_t count = 0; count < framesCaptured; count++) {
		//Check if there is at least 1MB of space left
		if (getSDSpace() < 1000) {
			drawMessage((char*) "No space, stop conversion..");
			delay(1000);
			return;
		}
		//Check if the user wants to abort conversion
		if (videoSave == false) {
			drawMessage((char*) "Video proccessing aborted!");
			delay(1000);
			return;
		}
		//Get filename
		frameFilename(filename, count);
		strcpy(&filename[5], ".DAT");
		//Load Raw data
		loadRawData(filename, dirname);
		//Display Raw Data
		displayRawData();
		//Set font size to small
		display.setFont(smallFont);
		//Show the image number
		sprintf(buffer, "%5d / %-5d", count + 1, framesCaptured);
		display.setColor(VGA_WHITE);
		display.print(buffer, CENTER, 225);
		//Save frame to image file
		strcpy(&filename[5], ".BMP");
		saveVideoFrame(filename, dirname);
	}
	//All images converted!
	drawMessage((char*) "Video proccessing finished !");
	delay(1000);
}

/* Saves raw data for an image or an video frame */
void saveRawData(bool isImage, char* name, uint16_t framesCaptured) {
	uint16_t result;

	//Start SD
	startAltClockline(true);
	//Create filename for image
	if (isImage) {
		strcpy(&name[14], ".DAT");
		sdFile.open(name, O_RDWR | O_CREAT | O_AT_END);
	}
	//Create filename for video frame
	else {
		char filename[] = "00000.DAT";
		frameFilename(filename, framesCaptured);
		sd.chdir(name);
		sdFile.open(filename, O_RDWR | O_CREAT | O_AT_END);
	}

	//For the Lepton2 sensor, write 4800 raw values
	if (leptonVersion != leptonVersion_3_Shutter) {
		for (int line = 0; line < 60; line++) {
			for (int column = 0; column < 80; column++) {
				result = image[(line * 2 * 160) + (column * 2)];
				sdFile.write((result & 0xFF00) >> 8);
				sdFile.write(result & 0x00FF);
			}
		}
	}
	//For the Lepton3 sensor, write 19200 raw values
	else {
		for (int i = 0; i < 19200; i++) {
			sdFile.write((image[i] & 0xFF00) >> 8);
			sdFile.write(image[i] & 0x00FF);
		}
	}

	//Write min and max
	sdFile.write((minTemp & 0xFF00) >> 8);
	sdFile.write(minTemp & 0x00FF);
	sdFile.write((maxTemp & 0xFF00) >> 8);
	sdFile.write(maxTemp & 0x00FF);

	//Write the object temp 
	uint8_t farray[4];
	floatToBytes(farray, mlx90614Temp);
	for (int i = 0; i < 4; i++)
		sdFile.write(farray[i]);

	//Write the color scheme
	sdFile.write(colorScheme);
	//Write the temperature format
	sdFile.write(tempFormat);
	//Write the show spot attribute
	sdFile.write(spotEnabled);
	//Write the show colorbar attribute
	if (calStatus == cal_warmup)
		sdFile.write((byte)0);
	else
		sdFile.write(colorbarEnabled);
	//Write the temperature points enabled attribute
	if (calStatus == cal_warmup)
		sdFile.write((byte)0);
	else
		sdFile.write(pointsEnabled);

	//Write calibration offset
	floatToBytes(farray, (float)calOffset);
	for (int i = 0; i < 4; i++)
		sdFile.write(farray[i]);
	//Write calibration slope
	floatToBytes(farray, (float)calSlope);
	for (int i = 0; i < 4; i++)
		sdFile.write(farray[i]);

	//Write temperature points
	for (int i = 0; i < 192; i++) {
		sdFile.write((showTemp[i] & 0xFF00) >> 8);
		sdFile.write(showTemp[i] & 0x00FF);
	}

	//Close the file
	sdFile.close();
	//Switch Clock back to Standard
	endAltClockline();
}

/* Save a screenshot to the sd Card */
void saveScreenshot() {
	if (Serial.available() > 0) {
		Serial.read();
		Serial.println("Saving Screenshot..");
		//Switch Clock to Alternative
		startAltClockline(true);
		//Choose a filename
		char filename[] = "SCREENSHOT000.BMP";
		for (int i = 0; i < 1000; i++) {
			filename[10] = '0' + i / 100 % 10;
			filename[11] = '0' + i / 10 % 10;
			filename[12] = '0' + i % 10;
			if (!sd.exists(filename))
				break;
		}
		sdFile.open(filename, O_RDWR | O_CREAT | O_AT_END);
		//Create the Thermal Image File header
		const char bmp_header[66] = { 0x42, 0x4D, 0x36, 0x60, 0x09, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
			0x80, 0x02, 0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0x01, 0x00, 0x10,
			0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x60, 0x09, 0x00, 0xC4, 0x0E,
			0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00,
			0x1F, 0x00, 0x00, 0x00 };
		//Write the BMP header
		for (int i = 0; i < 66; i++) {
			char ch = bmp_header[i];
			sdFile.write((uint8_t*)&ch, 1);
		}
		//Allocate space for sd buffer
		uint8_t* sdBuffer = (uint8_t*)calloc(640, sizeof(uint8_t));
		unsigned short pixel;
		//Save 320*60 pixels from the screen at one time
		for (int i = 3; i >= 0; i--) {
			endAltClockline();
			//Read pixels from the display
			display.readScreen(i, image);
			startAltClockline();
			for (byte y = 0; y < 60; y++) {
				//Write them into the sd buffer
				for (uint16_t x = 0; x < 320; x++) {
					pixel = image[((59 - y) * 320) + x];
					sdBuffer[x * 2] = pixel & 0x00FF;
					sdBuffer[(x * 2) + 1] = (pixel & 0xFF00) >> 8;
				}
				//Write them to the sd card and double for better resolution
				for (int i = 0; i < 2; i++) {
					for (uint16_t x = 0; x < 320; x++) {
						sdFile.write(sdBuffer[x * 2]);
						sdFile.write(sdBuffer[(x * 2) + 1]);
						sdFile.write(sdBuffer[x * 2]);
						sdFile.write(sdBuffer[(x * 2) + 1]);
					}
				}
			}
		}
		//De-allocate space
		free(sdBuffer);
		//Close file
		sdFile.close();
		//End SD Transmission
		endAltClockline();
		Serial.println("Screenshot saved !");
	}
}