/*
* Functions to create and display the thermal image
*/

/* Filter the image with a box blur filter (LP) */
void boxFilter() {
	int sum;

	for (int y = 1; y < 119; y++) {
		for (int x = 1; x < 159; x++) {
			sum = 0;
			for (int k = -1; k <= 1; k++) {
				for (int j = -1; j <= 1; j++) {
					sum += image[(y - j) * 160 + (x - k)];
				}
			}
			image[(y * 160) + x] = (unsigned short)(sum / 9.0);
		}
	}
}

/* Filter the image with a gaussian blur filter (LP) */
void gaussianFilter() {
	byte gaussianKernel[3][3] = {
		{ 1, 2, 1 },
		{ 2, 4, 2 },
		{ 1, 2, 1 }
	};
	int sum;

	for (int y = 1; y < 119; y++) {
		for (int x = 1; x < 159; x++) {
			sum = 0;
			for (int k = -1; k <= 1; k++) {
				for (int j = -1; j <= 1; j++) {
					sum += gaussianKernel[j + 1][k + 1] * image[(y - j) * 160 + (x - k)];
				}
			}
			image[(y * 160) + x] = (unsigned short)(sum / 16.0);
		}
	}
}


/* Store one package of 80 columns into RAM */
bool savePackage(byte line, byte segment = 0) {
	//Go through the video pixels for one video line
	for (int column = 0; column < 80; column++) {
		uint16_t result = (uint16_t)(leptonFrame[2 * column + 4] << 8
			| leptonFrame[2 * column + 5]);
		if (result == 0) {
			return false;
		}
		//Lepton2
		if (leptonVersion != leptonVersion_3_Shutter) {
			if (((mlx90614Version == mlx90614Version_old) && (rotationEnabled == false)) ||
				((mlx90614Version == mlx90614Version_new) && (rotationEnabled == true))) {
				image[(line * 2 * 160) + (column * 2)] = result;
				image[(line * 2 * 160) + (column * 2) + 1] = result;
				image[(line * 2 * 160) + 160 + (column * 2)] = result;
				image[(line * 2 * 160) + 160 + (column * 2) + 1] = result;
			}
			else {
				image[19199 - ((line * 2 * 160) + (column * 2))] = result;
				image[19199 - ((line * 2 * 160) + (column * 2) + 1)] = result;
				image[19199 - ((line * 2 * 160) + 160 + (column * 2))] = result;
				image[19199 - ((line * 2 * 160) + 160 + (column * 2) + 1)] = result;
			}
		}
		//Lepton3
		else {
			//Fill array non rotated
			if (!rotationEnabled) {
				switch (segment) {
				case 1:
					image[19199 - (((line / 2) * 160) + ((line % 2) * 80) + (column))] = result;
					break;
				case 2:
					image[14399 - (((line / 2) * 160) + ((line % 2) * 80) + (column))] = result;
					break;
				case 3:
					image[9599 - (((line / 2) * 160) + ((line % 2) * 80) + (column))] = result;
					break;
				case 4:
					image[4799 - (((line / 2) * 160) + ((line % 2) * 80) + (column))] = result;
					break;
				}
			}
			//Fill array rotated
			else {
				switch (segment) {
				case 1:
					image[((line / 2) * 160) + ((line % 2) * 80) + (column)] = result;
					break;
				case 2:
					image[4800 + (((line / 2) * 160) + ((line % 2) * 80) + (column))] = result;
					break;
				case 3:
					image[9600 + (((line / 2) * 160) + ((line % 2) * 80) + (column))] = result;
					break;
				case 4:
					image[14400 + (((line / 2) * 160) + ((line % 2) * 80) + (column))] = result;
					break;
				}
			}
		}
	}
	return true;
}

/* Refresh the temperature points*/
void refreshTempPoints() {
	for (int y = 0; y < 12; y++) {
		for (int x = 0; x < 16; x++) {
			if (showTemp[(x + (16 * y))] != 0) {
				showTemp[(x + (16 * y))] = image[(x * 10) + (y * 10 * 160) + 805];
			}
		}
	}
}

/* Get one image from the Lepton module */
void getTemperatures() {
	byte leptonError, segmentNumbers, line;
	//For Lepton2 sensor, get only one segment per frame
	if (leptonVersion != leptonVersion_3_Shutter)
		segmentNumbers = 1;
	//For Lepton3 sensor, get four packages per frame
	else
		segmentNumbers = 4;
	//Begin SPI transmission
	leptonBeginSPI();
	for (byte segment = 1; segment <= segmentNumbers; segment++) {
		leptonError = 0;
		do {
			for (line = 0; line < 60; line++) {
				//If line matches expectation
				if (leptonReadFrame(line, segment)) {
					if (!savePackage(line, segment)) {
						//Stabilize framerate
						delayMicroseconds(800);
						//Raise lepton error
						leptonError++;
						break;
					}
				}
				//Reset if the expected line does not match the answer
				else {
					if (leptonError == 255) {
						//If show menu was entered
						if (showMenu) {
							leptonEndSPI();
							return;
						}
						//Reset segment
						segment = 1;
						//Reset lepton error
						leptonError = 0;
						//End Lepton SPI
						leptonEndSPI();
						//Short delay
						delay(186);
						//Begin Lepton SPI
						leptonBeginSPI();
						break;
					}
					else {
						//Stabilize framerate
						delayMicroseconds(800);
						//Raise lepton error
						leptonError++;
						break;
					}
				}
			}
		} while (line != 60);
	}
	//End Lepton SPI
	leptonEndSPI();
}


/* Find the position of the minimum and maximum value */
void findMinMaxPositions()
{
	uint16_t min = 65535;
	uint16_t max = 0;
	//Go through the image
	for (int i = 0; i < 19200; i++)
	{
		if (image[i] < min)
		{
			minTempPos = i;
			min = image[i];
		}
		if (image[i] > max)
		{
			maxTempPos = i;
			max = image[i];
		}
	}
}

/* Go through the array of temperatures and find min and max temp */
void limitValues() {
	maxTemp = 0;
	minTemp = 65535;
	uint16_t temp;
	for (int i = 0; i < 19200; i++) {
		//Get value
		temp = image[i];
		//Find maximum temp
		if (temp > maxTemp)
			maxTemp = temp;
		//Find minimum temp
		if (temp < minTemp)
			minTemp = temp;
	}
}

/* Get the colors for hot / cold mode selection */
void getHotColdColors(byte* red, byte* green, byte* blue) {
	switch (hotColdColor) {
		//White
	case hotColdColor_white:
		*red = 255;
		*green = 255;
		*blue = 255;
		break;
		//Black
	case hotColdColor_black:
		*red = 0;
		*green = 0;
		*blue = 0;
		break;
		//White
	case hotColdColor_red:
		*red = 255;
		*green = 0;
		*blue = 0;
		break;
		//White
	case hotColdColor_green:
		*red = 0;
		*green = 255;
		*blue = 0;
		break;
		//White
	case hotColdColor_blue:
		*red = 0;
		*green = 0;
		*blue = 255;
		break;
	}
}

/* Convert the lepton values to RGB colors */
void convertColors() {
	uint8_t red, green, blue;

	//Calculate the scale
	float scale = (colorElements - 1.0) / (maxTemp - minTemp);

	//For hot and cold mode, calculate rawlevel
	float hotColdRawLevel = 0.0;
	if (hotColdMode != hotColdMode_disabled)
		hotColdRawLevel = tempToRaw(hotColdLevel);

	for (int i = 0; i < 19200; i++) {
		uint16_t value = image[i];

		//Limit values
		if (image[i] > maxTemp)
			image[i] = maxTemp;
		if (image[i] < minTemp)
			image[i] = minTemp;

		//Hot
		if ((hotColdMode == hotColdMode_hot) && (value >= hotColdRawLevel) && (calStatus != cal_warmup))
			getHotColdColors(&red, &green, &blue);
		//Cold
		else if ((hotColdMode == hotColdMode_cold) && (value <= hotColdRawLevel) && (calStatus != cal_warmup))
			getHotColdColors(&red, &green, &blue);
		//Apply colorscheme
		else {
			value = (image[i] - minTemp) * scale;
			red = colorMap[3 * value];
			green = colorMap[3 * value + 1];
			blue = colorMap[3 * value + 2];
		}
		//Convert to RGB565
		image[i] = (((red & 248) | green >> 5) << 8) | ((green & 28) << 3 | blue >> 3);
	}
}

/* Create the visual or combined image display */
void createVisCombImg() {
	//Send capture command
	captureVisualImage();
	//Receive the temperatures over SPI
	getTemperatures();
	//Compensate calibration with object temp
	compensateCalib();

	//Refresh the temp points if required
	if (pointsEnabled)
		refreshTempPoints();

	//Find min and max if not in manual mode and limits not locked
	if ((agcEnabled) && (!limitsLocked))
		limitValues();

	//Find min / max position
	if (minMaxPoints != minMaxPoints_disabled)
		findMinMaxPositions();

	//For combined only
	if (displayMode == displayMode_combined) {
		//Apply low-pass filter
		if (filterType == filterType_box)
			boxFilter();
		else if (filterType == filterType_gaussian)
			gaussianFilter();
		//Convert lepton data to RGB565 colors
		convertColors();
	}

	//Resize the image
	resizeOptical();
	//Move the image
	moveOptical();
	//Get the visual image and decompress it combined
	getVisualImage();
}

/* Creates a thermal image and stores it in the array */
void createThermalImg(bool menu) {
	//Receive the temperatures over SPI
	getTemperatures();
	//Compensate calibration with object temp
	compensateCalib();

	//Refresh the temp points if required
	if (pointsEnabled)
		refreshTempPoints();

	//Find min and max if not in manual mode and limits not locked
	if ((agcEnabled) && (!limitsLocked))
		limitValues();

	//If image save, save the raw data
	if (imgSave == imgSave_create)
		saveRawData(true, saveFilename);

	//Apply low-pass filter
	if (filterType == filterType_box)
		boxFilter();
	else if (filterType == filterType_gaussian)
		gaussianFilter();

	//Find min / max position
	if (minMaxPoints != minMaxPoints_disabled)
		findMinMaxPositions();

	//Convert lepton data to RGB565 colors
	convertColors();
}

/* Read the x and y coordinates when touch screen is pressed for Add Point */
void getTouchPos(int* x, int* y) {
	int iter = 0;
	TS_Point point;
	unsigned long tx = 0;
	unsigned long ty = 0;
	//Wait for touch press
	while (!touch.touched());
	//While touch pressed, iterate over readings
	while (touch.touched() == true) {
		point = touch.getPoint();
		if ((point.x >= 0) && (point.x <= 320) && (point.y >= 0)
			&& (point.y <= 240)) {
			tx += point.x;
			ty += point.y;
			iter++;
		}
	}
	*x = tx / iter;
	*y = ty / iter;
}

/* Function to add or remove a measurement point */
void tempPointFunction(bool remove) {
	int x, y;

	//Safe delay
	delay(10);
	//Create thermal image
	if (displayMode == displayMode_thermal)
		createThermalImg();
	//Create visual or combined image
	else
		createVisCombImg();
	//Show it on the screen
	showImage();

	//Set text color, font and background
	display.setColor(VGA_WHITE);
	display.setBackColor(VGA_TRANSPARENT);
	display.setFont(smallFont);
	//Show current temperature points
	showTemperatures();
	//Get touched coordinates
	getTouchPos(&x, &y);
	//Divide through 20 to match array size
	x /= 20;
	y /= 20;
	if (x < 0)
		x = 0;
	if (x > 15)
		x = 15;
	if (y < 0)
		y = 0;
	if (y > 11)
		y = 11;
	//Remove point from screen by setting it to zero
	if (remove) {
	outerloop:
		for (int i = x - 1; i <= x + 1; i++) {
			for (int j = y - 1; j <= y + 1; j++) {
				if (((j * 16 + i) >= 0) && ((j * 16 + i) < 192)) {
					if (showTemp[(j * 16) + i] != 0) {
						showTemp[(j * 16) + i] = 0;
						goto outerloop;
					}
				}
			}
		}
	}
	//Mark point for refreshing in the next image
	else
		showTemp[(y * 16) + x] = 1;
}

/* Display the minimum and maximum point on the screen */
void displayMinMaxPoint(uint16_t pixelIndex, const char *str)
{
	//Get xpos and ypos
	uint16_t xpos = (pixelIndex % 160) * 2;
	uint16_t ypos = (pixelIndex / 160) * 2;

	//Compensate in visual or combined mode
	if (displayMode != displayMode_thermal) {
		ypos = ypos + (5 * adjCombDown) - (5 * adjCombUp);
		xpos = xpos + (5 * adjCombRight) - (5 * adjCombLeft);
	}

	//Limit position
	if (ypos > 240)
		ypos = 240;
	if (xpos > 320)
		xpos = 320;
	if (xpos < 0)
		xpos = 0;
	if (ypos < 0)
		ypos = 0;
	//Draw the marker
	display.drawLine(xpos / 2, ypos / 2, xpos / 2, ypos / 2);
	//Draw the string
	xpos += 4;
	if (xpos >= 310)
		xpos -= 10;
	if (ypos > 230)
		ypos = 230;
	display.print(str, xpos, ypos);
}

/* Clears the show temperatures array */
void clearTemperatures() {
	//Set all showTemps to zero
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 12; y++) {
			showTemp[(y * 16) + x] = 0;
		}
	}
}

/* Shows the temperatures over the image on the screen */
void showTemperatures() {
	int xpos, ypos;
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 12; y++) {
			if (showTemp[(y * 16) + x] != 0) {
				xpos = x * 20;
				ypos = y * 20;
				//Compensate in visual or combined mode
				if (displayMode != displayMode_thermal) {
					ypos = ypos + (5 * adjCombDown) - (5 * adjCombUp);
					xpos = xpos + (5 * adjCombRight) - (5 * adjCombLeft);
				}
				if (ypos <= 12)
					ypos = 13;
				display.print((char*) ".", xpos, ypos - 10);
				xpos -= 22;
				if (xpos < 0)
					xpos = 0;
				ypos += 6;
				if (ypos > 239)
					ypos = 239;
				display.printNumF(calFunction(showTemp[(y * 16) + x]), 2, xpos, ypos);
			}
		}
	}
}