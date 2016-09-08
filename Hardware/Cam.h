/*
*
* CAM - Access the VC0706 camera module and display JPEGs
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

//JPEG Decompressor structure
typedef struct {
	const byte* jpic;
	unsigned short jsize;
	unsigned short joffset;
} IODEV;

//JPEG Decompressor
void* jdwork;
JDEC jd;
IODEV iodev;

//Combined mode for decompressor
bool combinedDecomp = false;

/* Methods */

//Resize the pixels of the optical image
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

/* Move the optical image down by one pixel */
void moveOpticalDown() {
	//First for all pixels
	for (int col = 0; col < 160; col++)
		for (int row = 119; row > 0; row--)
			image[col + (row * 160)] = image[col + ((row - 1) * 160)];
	//And then fill the last one with white
	for (int col = 0; col < 160; col++)
		image[col] = 65535;
}

/* Move the optical image up by one pixel */
void moveOpticalUp() {
	//First for all pixels
	for (int col = 0; col < 160; col++)
		for (int row = 0; row < 119; row++)
			image[col + (row * 160)] = image[col + ((row + 1) * 160)];
	//And then fill the last one with white
	for (int col = 0; col < 160; col++)
		image[col + (119 * 160)] = 65535;
}

/* Move the optical image right by one pixel */
void moveOpticalRight() {
	//First for all pixels
	for (int col = 159; col > 0; col--)
		for (int row = 0; row < 120; row++)
			image[col + (row * 160)] = image[col - 1 + (row * 160)];
	//And then fill the last one with white
	for (int row = 0; row < 120; row++)
		image[row * 160] = 65535;
}

/* Move the optical image left by one pixel */
void moveOpticalLeft() {
	//First for all pixels
	for (int col = 0; col < 159; col++)
		for (int row = 0; row < 120; row++)
			image[col + (row * 160)] = image[col + 1 + (row * 160)];
	//And then fill the last one with white
	for (int row = 0; row < 120; row++)
		image[159 + (row * 160)] = 65535;
}

/* A method to move the optical image in each direction */
void moveOptical() {
	//Left
	for (int i = 0; i < (adjCombLeft * 5); i++) {
		moveOpticalLeft();
	}
	//Right
	for (int i = 0; i < (adjCombRight * 5); i++) {
		moveOpticalRight();
	}
	//Down
	for (int i = 0; i < (adjCombDown * 5); i++) {
		moveOpticalDown();
	}
	//Up
	for (int i = 0; i < (adjCombUp * 5); i++) {
		moveOpticalUp();
	}
}

/* Resize the optical image */
void resizeOptical() {
	uint16_t newWidth = round(adjCombFactor * 160);
	uint16_t newHeight = round(adjCombFactor * 120);
	resizePixels(image, 160, 120, newWidth, newHeight);
	uint16_t rightMove = round((160 - newWidth) / 2.0);
	//Move the image down
	for (int i = 0; i < rightMove; i++) {
		moveOpticalRight();
	}
	uint16_t downMove = round((120 - newHeight) / 2.0);
	for (int i = 0; i < downMove; i++) {
		moveOpticalDown();
	}
}

/* Change the resolution of the device */
void changeCamRes(byte size) {
	//Change resolution
	cam.setImageSize(size);
	//Reset the device to change the resolution
	cam.reset();
	//Wait some time
	delay(300);
	//Re-establish the connection to the device
	cam.begin(115200);
	//Set camera compression
	cam.setCompression(95);
}

/* Init the camera module */
void initCamera() {
	//Start connection at 115.2k Baud
	cam.begin(115200);
	//Set camera compression
	cam.setCompression(95);
	//Test if the camera works
	if (!cam.takePicture()) {
		// try it again after 1 second...
		delay(1000);
		cam.reset();
		//Start connection at 115.2k Baud
		cam.begin(115200);
		//Set camera compression
		cam.setCompression(95);
		delay(1000);
		if (!cam.takePicture()) {
			showFullMessage((char*) "Visual camera is not working!");
			delay(1000);
			setDiagnostic(diag_camera);
		}
	}
	//Skip the picture
	cam.end();
	//Check if the resolution is set to big
	if (cam.getImageSize() != VC0706_640x480)
		changeCamRes(VC0706_640x480);
}

/* Output function for the JPEG Decompressor - extracts the RGB565 values into the target array */
unsigned int output_func(JDEC * jd, void * bitmap, JRECT * rect) {
	//Help Variables
	byte redV, greenV, blueV, redT, greenT, blueT, red, green, blue;
	unsigned short pixel;
	unsigned short * bmp = (unsigned short *)bitmap;
	unsigned short x, y;
	unsigned short i = 0;
	//Go through the image
	for (y = rect->top; y <= rect->bottom; y++) {
		for (x = rect->left; x <= rect->right; x++) {
			//Write into the array with transparency if combined activated
			if (combinedDecomp) {
				//Get the visual image color
				pixel = bmp[i++];
				//And extract the RGB values out of it
				redV = (pixel & 0xF800) >> 8;
				greenV = (pixel & 0x7E0) >> 3;
				blueV = (pixel & 0x1F) << 3;
				//Get the thermal image color
				if (mlx90614Version == mlx90614Version_old)
					pixel = image[(159 - x) + (y * 160)];
				else
					pixel = image[x + (y * 160)];
				//And extract the RGB values out of it
				redT = (pixel & 0xF800) >> 8;
				greenT = (pixel & 0x7E0) >> 3;
				blueT = (pixel & 0x1F) << 3;
				//Mix both
				red = redT * 0.5 + redV * 0.5;
				green = greenT * 0.5 + greenV * 0.5;
				blue = blueT * 0.5 + blueV * 0.5;
				//Set image to that calculated RGB value
				if (mlx90614Version == mlx90614Version_old)
					image[(159 - x) + (y * 160)] = (((red & 248) | green >> 5) << 8)
					| ((green & 28) << 3 | blue >> 3);
				else
					image[x + (y * 160)] = (((red & 248) | green >> 5) << 8)
					| ((green & 28) << 3 | blue >> 3);
			}
			//Write into the array if combined not activated
			else {
				if (mlx90614Version == mlx90614Version_old)
					image[(159 - x) + (y * 160)] = bmp[i++];
				else
					image[x + (y * 160)] = bmp[i++];
			}
		}
	}
	return 1;
}

/* Help function to insert the JPEG data into the decompressor */
unsigned int input_func(JDEC * jd, byte* buff, unsigned int ndata) {
	IODEV * dev = (IODEV *)jd->device;
	ndata = (unsigned int)dev->jsize - dev->joffset > ndata ?
		ndata : dev->jsize - dev->joffset;
	if (buff)
		memcpy(buff, dev->jpic + dev->joffset, ndata);
	dev->joffset += ndata;
	return ndata;
}

/* Send the capture command to the camera*/
void captureVisualImage() {
	cam.takePicture();
}

/* Receive the visual image from the camera and save it on the SD card */
void saveVisualImage() {
	uint8_t *buffer;

	uint16_t jpglen = cam.frameLength();
	//Start alternative clockline for the SD card
	startAltClockline();
	//Transfer data
	while (jpglen > 0) {
		uint8_t bytesToRead = min(jpglen, 64);
		buffer = cam.readPicture(bytesToRead);
		sdFile.write(buffer, bytesToRead);
		jpglen -= bytesToRead;
	}
	sdFile.close();
	//End SD Transmission
	endAltClockline();
	//End camera
	cam.end();
}

/* Receive the image data and display it on the screen */
void getVisualImage() {
	uint8_t *buffer;

	//Get frame length
	uint16_t jpglen = cam.frameLength();
	//Define array for the jpeg data
	uint8_t* jpegData = (uint8_t*)calloc(jpglen, sizeof(uint8_t));
	//Count variable
	uint16_t counter = 0;
	//Store length
	uint16_t length = jpglen;
	//Transfer data
	while (length > 0) {
		uint8_t bytesToRead = min(length, 64);
		buffer = cam.readPicture(bytesToRead);
		for (int i = 0; i < bytesToRead; i++) {
			jpegData[counter] = buffer[i];
			counter++;
		}
		length -= bytesToRead;
	}
	//End transmission
	cam.end();
	//Decompress the image
	iodev.jpic = jpegData;
	iodev.jsize = jpglen;
	//the offset is zero
	iodev.joffset = 0;
	//Define space for the decompressor
	jdwork = malloc(3100);
	//Prepare the image for convertion to RGB565
	jd_prepare(&jd, input_func, jdwork, 3100, &iodev);
	//Small image, do not downsize
	if (cam.getImageSize() == VC0706_160x120)
		jd_decomp(&jd, output_func, 0);
	//320x240, do downsize to 160x120
	else
		jd_decomp(&jd, output_func, 1);
	//Free the jpeg data array
	free(jpegData);
	//Free space for the decompressor
	free(jdwork);
}

/* Receive the visual image and transfer it to the serial port */
void transferVisualImage() {
	uint8_t *buffer;

	//Get frame length
	uint16_t jpglen = cam.frameLength();
	//Store length
	uint16_t length = jpglen;
	//Transfer data
	while (length > 0) {
		uint8_t bytesToRead = min(length, 64);
		buffer = cam.readPicture(bytesToRead);
		Serial.write(buffer, bytesToRead);
		length -= bytesToRead;
	}
	//End transmission
	cam.end();
}