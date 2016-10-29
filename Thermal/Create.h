/*
*
* CREATE - Functions to create and display the thermal image
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

/* Methods*/

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

/* Reset the SPI bus to re-initiate Lepton communication */
void resetLeptonSPIBus() {
	//End Lepton SPI
	leptonEndSPI();
	//Short delay
	delay(186);
	//Begin Lepton SPI
	leptonBeginSPI();
}

/* Get one image from the Lepton module */
void getTemperatures() {
	byte leptonError, segmentNumbers, line, row;
	//For Lepton2 sensor, get only one segment per frame
	if (leptonVersion != leptonVersion_3_Shutter)
		segmentNumbers = 1;
	//For Lepton3 sensor, get four packages per frame
	else
		segmentNumbers = 4;
	//Begin SPI transmission
	leptonBeginSPI();
	for (byte segment = 1; segment <= segmentNumbers; segment++) {
		// reset error count per segment
		leptonError = 0;

		// this is a label to reset the line number to 0
	frameLoop:
		line = 0;
		while (line < 60) {
			//If show menu was entered
			if (showMenu) {
				leptonEndSPI();
				return;
			}

			row = line;
			switch (leptonReadFrame(&row, segment)) {
				//line matched the received row number
			case NONE:
				if (!savePackage(line, segment)) {
					//Stabilize framerate
					delayMicroseconds(800);
					//Raise lepton error
					leptonError++;
				}
				line++;

				//Discard packet received
			case DISCARD:
				//Wait for next valid packet
				break;

				//line did not match received row number
			case ROW_ERROR:
				if (leptonError == 255) {
					//Reset segment
					segment = 1;
					//Reset lepton error
					leptonError = 0;
					//Reset the VOSPI stream
					resetLeptonSPIBus();
				}
				else {
					//Stabilize framerate
					delayMicroseconds(800);
					//Raise lepton error
					leptonError++;
				}
				goto frameLoop;

				//Received wrong segment number
			case SEGMENT_ERROR:
				//Reset segment
				segment = 1;
				//Reset lepton error
				leptonError = 0;
				//Reset the VOSPI stream
				resetLeptonSPIBus();
				goto frameLoop;

				//Received segment number outside of 1-4 (typically 0)
			case SEGMENT_INVALID:
				//Wait for all rows to pass or a discard packet
				while (row < 60) {
					//If show menu was entered
					if (showMenu) {
						leptonEndSPI();
						return;
					}

					switch (leptonReadFrame(&row, segment)) {
					case DISCARD: goto frameLoop;
					default: row++; break;
					}
				}
				goto frameLoop;
			}
		}
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
	if ((hotColdMode != hotColdMode_disabled) && (displayMode != displayMode_combined))
		hotColdRawLevel = tempToRaw(hotColdLevel);

	for (int i = 0; i < 19200; i++) {
		uint16_t value = image[i];

		//Limit values
		if (image[i] > maxTemp)
			image[i] = maxTemp;
		if (image[i] < minTemp)
			image[i] = minTemp;

		//Hot
		if ((hotColdMode == hotColdMode_hot) && (value >= hotColdRawLevel) && (calStatus != cal_warmup) && (displayMode != displayMode_combined))
			getHotColdColors(&red, &green, &blue);
		//Cold
		else if ((hotColdMode == hotColdMode_cold) && (value <= hotColdRawLevel) && (calStatus != cal_warmup) && (displayMode != displayMode_combined))
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

//Resize the pixels of thermal image
void resizePixels(unsigned short* pixels, int w1, int h1, int w2, int h2) {
	//Calculate the ratio
	int x_ratio = (int)((w1 << 16) / w2) + 1;
	int y_ratio = (int)((h1 << 16) / h2) + 1;
	int x2, y2;
	for (int i = 0; i < h2; i++) {
		for (int j = 0; j < w2; j++) {
			x2 = ((j * x_ratio) >> 16);
			y2 = ((i * y_ratio) >> 16);
			pixels[(i * w1) + j] = pixels[(y2 * w1) + x2];
		}
	}
	//Set the other pixels to zero
	for (int j = 0; j < h2; j++) {
		for (int i = w2; i < w1; i++) {
			pixels[i + (j * 160)] = 65535;
		}
	}
	for (int j = h2; j < h1; j++) {
		for (int i = 0; i < w1; i++) {
			pixels[i + (j * 160)] = 65535;
		}
	}
}

/* Resize the thermal image */
void resizeImage() {
	uint16_t newWidth = round(adjCombFactor * 160);
	uint16_t newHeight = round(adjCombFactor * 120);
	resizePixels(image, 160, 120, newWidth, newHeight);
	uint16_t rightMove = round((160 - newWidth) / 2.0);
	//Move the image right
	for (int i = 0; i < rightMove; i++) {
		//First for all pixels
		for (int col = 159; col > 0; col--)
			for (int row = 0; row < 120; row++)
				image[col + (row * 160)] = image[col - 1 + (row * 160)];
		//And then fill the last one with white
		for (int row = 0; row < 120; row++)
			image[row * 160] = 65535;
	}
	//Move the image down
	uint16_t downMove = round((120 - newHeight) / 2.0);
	for (int i = 0; i < downMove; i++) {
		//First for all pixels
		for (int col = 0; col < 160; col++)
			for (int row = 119; row > 0; row--)
				image[col + (row * 160)] = image[col + ((row - 1) * 160)];
		//And then fill the last one with white
		for (int col = 0; col < 160; col++)
			image[col] = 65535;
	}
}

/* Calculates the fill pixel for visual/combined */
void calcFillPixel(uint16_t x, uint16_t y) {
	uint16_t pixel;
	byte red, green, blue;

	//Combined - set to darker thermal
	if (displayMode == displayMode_combined) {
		//Get the thermal image color
		pixel = image[x + (y * 160)];
		//And extract the RGB values out of it
		byte redT = (pixel & 0xF800) >> 8;
		byte greenT = (pixel & 0x7E0) >> 3;
		byte blueT = (pixel & 0x1F) << 3;
		//Mix both
		red = (byte)redT * (1 - adjCombAlpha) + 127 * adjCombAlpha;
		green = (byte)greenT * (1 - adjCombAlpha) + 127 * adjCombAlpha;
		blue = (byte)blueT * (1 - adjCombAlpha) + 127 * adjCombAlpha;
	}
	//Visual - set to black
	else {
		red = 0;
		green = 0;
		blue = 0;
	}

	//Set image to that calculated RGB565 value
	pixel = (((red & 248) | green >> 5) << 8)
		| ((green & 28) << 3 | blue >> 3);
	//Save
	image[x + (y * 160)] = pixel;
}

/* Fill out the edges in combined or visual mode */
void fillEdges() {
	//Fill the edges
	uint16_t  x, y;

	//Top & Bottom edges
	for (x = 0; x < 160; x++) {
		//Top edge
		for (y = 0; y < (5 * adjCombDown); y++) {
			calcFillPixel(x, y);
		}

		//Bottom edge
		for (y = 119; y > (119 - (5 * adjCombUp)); y--) {
			calcFillPixel(x, y);
		}
	}

	//Left & right edges
	for (y = 5 * adjCombDown; y < (120 - (5 * adjCombUp)); y++) {
		//Left edge
		for (x = 0; x < (5 * adjCombRight); x++) {
			calcFillPixel(x, y);
		}

		//Right edge
		for (x = 159; x > (159 - (5 * adjCombLeft)); x--) {
			calcFillPixel(x, y);
		}
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

	//Find min / max position
	if (minMaxPoints != minMaxPoints_disabled)
		findMinMaxPositions();

	//Find min and max if not in manual mode and limits not locked
	if ((autoMode) && (!limitsLocked))
		limitValues();

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

	//Resize the thermal image
	resizeImage();

	//Fill the edges
	fillEdges();

	//Get the visual image and decompress it combined
	getVisualImage();
}

/* Creates a thermal image and stores it in the array */
void createThermalImg() {
	//Receive the temperatures over SPI
	getTemperatures();
	//Compensate calibration with object temp
	compensateCalib();

	//Refresh the temp points if required
	if (pointsEnabled)
		refreshTempPoints();

	//Find min / max position
	if (minMaxPoints != minMaxPoints_disabled)
		findMinMaxPositions();

	//Find min and max if not in manual mode and limits not locked
	if ((autoMode) && (!limitsLocked))
		limitValues();

	//If image save, save the raw data
	if (imgSave == imgSave_create)
		saveRawData(true, saveFilename);

	//Apply low-pass filter
	if (filterType == filterType_box)
		boxFilter();
	else if (filterType == filterType_gaussian)
		gaussianFilter();

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
	bool removed = false;

	//If remove points, check if there are some first
	if (remove) {
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 12; j++) {
				if (((j * 16 + i) >= 0) && ((j * 16 + i) < 192)) {
					if (showTemp[(j * 16) + i] != 0)
						removed = true;
				}
			}
		}
		if (!removed) {
			showFullMessage((char*) "No points available!", true);
			delay(1000);
			return;
		}
	}

redraw:
	//Safe delay
	delay(10);
	//Create thermal image
	if (displayMode == displayMode_thermal)
		createThermalImg();
	//Create visual or combined image
	else
		createVisCombImg();

	//Show it on the screen
	display.writeScreen(image);

	//Set text color, font and background
	setTextColor();
	display.setBackColor(VGA_TRANSPARENT);
	display.setFont(smallFont);
	//Show current temperature points
	showTemperatures();
	//Display title
	display.setFont(bigFont);
	display.print((char*) "Select position", CENTER, 210);

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

	//Remove point
	if (remove) {
		removed = false;
	outerloop:
		for (int i = x - 1; i <= x + 1; i++) {
			for (int j = y - 1; j <= y + 1; j++) {
				if (((j * 16 + i) >= 0) && ((j * 16 + i) < 192)) {
					if (showTemp[(j * 16) + i] != 0) {
						showTemp[(j * 16) + i] = 0;
						removed = true;
						goto outerloop;
					}
				}
			}
		}
		//Show message
		if (removed)
			showFullMessage((char*) "Point removed!", true);
		else {
			showFullMessage((char*) "Invalid position!", true);
			delay(1000);
			goto redraw;
		}
	}

	//Add point
	else {
		showTemp[(y * 16) + x] = 1;
		showFullMessage((char*) "Point added!", true);
	}
	//Wait some time
	delay(1000);
}

/* Calculate the x and y position for min/max out of the pixel index */
void calculateMinMaxPoint(uint16_t* xpos, uint16_t* ypos, uint16_t pixelIndex) {
	//Get xpos and ypos
	*xpos = (pixelIndex % 160) * 2;
	*ypos = (pixelIndex / 160) * 2;

	//Limit position
	if (*ypos > 240)
		*ypos = 240;
	if (*xpos > 320)
		*xpos = 320;
	if (*xpos < 0)
		*xpos = 0;
	if (*ypos < 0)
		*ypos = 0;
}

/* Display the minimum and maximum point on the screen */
void displayMinMaxPoint(uint16_t pixelIndex, const char *str)
{
	uint16_t xpos, ypos;
	//Calculate x and y position
	calculateMinMaxPoint(&xpos, &ypos, pixelIndex);
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