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

/* Methods */

/* Change the resolution of the device */
void changeCamRes(byte size) {
	//Change resolution
	cam.setImageSize(size);
	//Reset the device to change the resolution
	cam.reset();
	//Wait some time
	delay(300);
	//Re-establish the connection to the device
	cam.begin();
}

/* Init the camera module */
void initCamera() {
	//Start connection at 115.2k Baud
	cam.begin();
	//Test if the camera works
	if (!cam.takePicture()) {
		//Try it again after 100ms
		delay(100);
		cam.reset();
		//Start connection at 115.2k Baud
		cam.begin();
		//Try to take a picture again
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

/* Transfer the bytestream from the VC0706 camera */
void transPicture(byte bytesToRead, uint8_t* buffer)
{
	int avail;
	//Send counter and buffer length to zero
	uint8_t counter = 0;
	uint8_t bufferLen = 0;
	//As long as no timeout and not all bytes read
	while ((10 != counter) && (bufferLen != (bytesToRead + 5))) {
		//Check how many bytes are available
		avail = Serial1.available();
		//If there are none, raise timeout counter
		if (avail <= 0) {
			delay(1);
			counter++;
			continue;
		}
		//Reset timeout counter if there is a packet
		counter = 0;
		//Add the data to the buffer
		buffer[bufferLen++] = Serial1.read();
	}
}

/* Output function for the JPEG Decompressor - extracts the RGB565 values into the target array */
unsigned int output_func(JDEC * jd, void * bitmap, JRECT * rect) {
	//Help Variables
	byte redV, greenV, blueV, redT, greenT, blueT, red, green, blue, xPos;
	unsigned short pixel, x, y, imagepos, count;
	unsigned short * bmp = (unsigned short *)bitmap;
	count = 0;
	//Go through the visual image
	for (y = rect->top; y <= rect->bottom; y++) {

		//Check if we draw inside the screen on y position
		if (((y + (5 * adjCombDown) <= 119) && (y - (5 * adjCombUp) >= 0))) {

			for (x = rect->left; x <= rect->right; x++) {

				//Mirror visual image for old HW
				if (mlx90614Version == mlx90614Version_old)
					xPos = (159 - x);
				else
					xPos = x;

				//Check if we draw inside the screen on x position
				if (((xPos + (5 * adjCombRight) <= 159) && (xPos - (5 * adjCombLeft) >= 0))) {

					//Get the image position
					imagepos = xPos + (y * 160);
					//Do the visual alignment
					imagepos += 5 * adjCombRight;
					imagepos -= 5 * adjCombLeft;
					imagepos += 5 * adjCombDown * 160;
					imagepos -= 5 * adjCombUp * 160;

					//Create combined pixel out of thermal and visual
					if (displayMode == displayMode_combined) {
						//Get the visual image color
						pixel = bmp[count++];
						//And extract the RGB values out of it
						redV = (pixel & 0xF800) >> 8;
						greenV = (pixel & 0x7E0) >> 3;
						blueV = (pixel & 0x1F) << 3;

						//Get the thermal image color
						pixel = image[imagepos];
						//And extract the RGB values out of it
						redT = (pixel & 0xF800) >> 8;
						greenT = (pixel & 0x7E0) >> 3;
						blueT = (pixel & 0x1F) << 3;

						//Mix both
						red = redT * (1 - adjCombAlpha) + redV * adjCombAlpha;
						green = greenT * (1 - adjCombAlpha) + greenV * adjCombAlpha;
						blue = blueT * (1 - adjCombAlpha) + blueV * adjCombAlpha;

						//Set the pixel to the calculated RGB565 value
						pixel = (((red & 248) | green >> 5) << 8)
							| ((green & 28) << 3 | blue >> 3);

					}

					//Set pixel to visual image only
					else
						pixel = bmp[count++];

					//Write to image buffer
					image[imagepos] = pixel;
				}
				//Raise counter if it is not inside the image
				else
					count++;
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
	//Get frame length
	uint16_t jpglen = cam.frameLength();
	//Start alternative clockline for the SD card
	startAltClockline();
	//Create the buffer
	uint8_t* buffer = (uint8_t*)malloc(128 + 5);
	//Transfer data
	while (jpglen > 0) {
		//Calculate the bytes left to read
		uint8_t bytesToRead = min(jpglen, 128);
		//Send the read command
		if (!cam.readPicture(bytesToRead))
			continue;
		//Transfer the picture
		transPicture(bytesToRead, buffer);
		//Write it to the SD card
		sdFile.write(buffer, bytesToRead);
		//Substract transfered bytes from total
		jpglen -= bytesToRead;
	}
	//Free the buffer
	free(buffer);
	//End camera
	cam.end();
	//Close the file
	sdFile.close();
	//End SD Transmission
	endAltClockline();
}

/* Receive the image data and display it on the screen */
void getVisualImage() {
	//Get frame length
	uint16_t jpglen = cam.frameLength();
	//Store length
	uint16_t length = jpglen;
	//Define array for the jpeg data
	uint8_t* jpegData = (uint8_t*)calloc(jpglen, sizeof(uint8_t));
	//Count variable
	uint16_t counter = 0;
	//Create the buffer
	uint8_t* buffer = (uint8_t*)malloc(128 + 5);
	//Transfer data
	while (jpglen > 0) {
		//Calculate the bytes left to read
		uint8_t bytesToRead = min(jpglen, 128);
		//Send the read command
		if (!cam.readPicture(bytesToRead))
			continue;
		//Transfer the picture
		transPicture(bytesToRead, buffer);
		//Add it to the jpeg data buffer
		for (int i = 0; i < bytesToRead; i++) {
			jpegData[counter] = buffer[i];
			counter++;
		}
		//Substract transfered bytes from total
		jpglen -= bytesToRead;
	}
	//Free the buffer
	free(buffer);
	//End transmission
	cam.end();

	//Decompress the image
	iodev.jpic = jpegData;
	iodev.jsize = length;
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
	//Get frame length
	uint16_t jpglen = cam.frameLength();
	//Transfer frame length
	Serial.write((jpglen & 0xFF00) >> 8);
	Serial.write(jpglen & 0x00FF);
	//Create the buffer
	uint8_t* buffer = (uint8_t*)malloc(128 + 5);
	//Transfer data
	while (jpglen > 0) {
		//Calculate the bytes left to read
		uint8_t bytesToRead = min(jpglen, 128);
		//Send the read command
		if (!cam.readPicture(bytesToRead))
			continue;
		//Transfer the picture
		transPicture(bytesToRead, buffer);
		//Write it to the serial port
		Serial.write(buffer, bytesToRead);
		//Substract transfered bytes from total
		jpglen -= bytesToRead;
	}
	//Free the buffer
	free(buffer);
	//End transmission
	cam.end();
}