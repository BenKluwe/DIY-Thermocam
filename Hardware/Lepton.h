/*
*
* LEPTON - Access the FLIR Lepton LWIR module
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
//Array to store one Lepton frame
byte leptonFrame[164];

/* Methods */

/* Start Lepton SPI Transmission */
void leptonBeginSPI() {
	//Lepton3 - 30 Mhz minimum and SPI mode 0
	if (leptonVersion == leptonVersion_3_Shutter)
		SPI.beginTransaction(SPISettings(30000000, MSBFIRST, SPI_MODE0));
	//Lepton2 - 20 Mhz maximum and SPI mode 1
	else
		SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE1));
	//Start alternative clock line, except for old HW
	if (mlx90614Version == mlx90614Version_new)
		startAltClockline();
	//Start transfer  - CS LOW
	digitalWrite(pin_lepton_cs, LOW);
}

/* End Lepton SPI Transmission */
void leptonEndSPI() {
	//End transfer - CS HIGH
	digitalWriteFast(pin_lepton_cs, HIGH);
	//End SPI Transaction
	SPI.endTransaction();
	//End alternative clock line, except for old HW
	if (mlx90614Version == mlx90614Version_new)
		endAltClockline();
}

/* Reads one line (164 Bytes) from the lepton over SPI */
bool leptonReadFrame(byte line, byte seg) {
	//Receive one frame over SPI
	SPI.transfer(leptonFrame, 164);
	//Repeat as long as the frame is not valid, equals sync
	if ((leptonFrame[0] & 0x0F) == 0x0F) {
		return false;
	}
	//Check if the line number matches the expected line
	if (leptonFrame[1] != line) {
		return false;
	}
	//For the Lepton3, check if the segment number matches
	if ((line == 20) && (leptonVersion == leptonVersion_3_Shutter)) {
		byte segment = (leptonFrame[0] >> 4);
		if (segment != seg)
			return false;
	}
	return true;
}

/* Trigger a flat-field-correction on the Lepton */
void leptonRunCalibration() {
	byte error;
	byte errorCounter = 0;
	do {
		Wire.beginTransmission(0x2A);
		Wire.write(0x00);
		Wire.write(0x04);
		Wire.write(0x02);
		Wire.write(0x42);
		error = Wire.endTransmission();
		if (error) {
			errorCounter++;
			delay(10);
		}
		//Trigger error and continue
		if ((error) && (errorCounter > 10)) {
			showFullMessage((char*) "Lepton I2C FFC not working!");
			delay(1000);
			setDiagnostic(diag_lep_conf);
			return;
		}
	} while (error != 0);
	//Wait some time
	delay(2000);
}

/* Select I2C Register on the Lepton */
void leptonSetReg(byte reg) {
	Wire.beginTransmission(0x2A);
	Wire.write(reg >> 8 & 0xff);
	Wire.write(reg & 0xff);
	Wire.endTransmission();
}

/* Read I2C Register on the lepton */
int leptonReadReg(byte reg) {
	uint16_t reading = 0;
	leptonSetReg(reg);
	Wire.requestFrom(0x2A, 2);
	reading = Wire.read();
	reading = reading << 8;
	reading |= Wire.read();
	return reading;
}

/* Set the shutter operation to manual/auto */
void leptonSetFFCMode(bool automatic)
{
	//Array for the package
	byte package[64];
	//Read command
	Wire.beginTransmission(0x2A);
	Wire.write(0x00);
	Wire.write(0x04);
	Wire.write(0x02);
	Wire.write(0x3C);
	Wire.endTransmission();
	//Read old FFC package first
	Wire.beginTransmission(0x2A);
	while (leptonReadReg(0x2) & 0x01);
	uint8_t length = leptonReadReg(0x6);
	Wire.requestFrom((uint8_t)0x2A, length);

	for (byte i = 0; i < length; i++)
	{
		package[i] = Wire.read();
		Serial.println(package[i]);
	}
	//Alter the second bit to set FFC to manual
	package[0] = automatic;
	//Transmit the new package
	Wire.beginTransmission(0x2A);
	Wire.write(0x00);
	Wire.write(0x08);
	for (int i = 0; i < length; i++) {
		Wire.write(package[i]);
	}
	Wire.endTransmission();
	//Package length, use 4 here
	Wire.beginTransmission(0x2A);
	Wire.write(0x00);
	Wire.write(0x06);
	Wire.write(0x00);
	Wire.write(0x04);
	Wire.endTransmission();
	//Module and command ID
	Wire.beginTransmission(0x2A);
	Wire.write(0x00);
	Wire.write(0x04);
	Wire.write(0x02);
	Wire.write(0x3D);
	Wire.endTransmission();
}

/* Checks the Lepton hardware revision */
void leptonCheckVersion() {
	//Get AGC Command
	Wire.beginTransmission(0x2A);
	Wire.write(0x00);
	Wire.write(0x04);
	Wire.write(0x48);
	Wire.write(0x1C);
	byte error = Wire.endTransmission();
	//Lepton I2C error, continue
	if (error != 0) {
		showFullMessage((char*) "Lepton I2C getVersion not working!");
		delay(1000);
		setDiagnostic(diag_lep_conf);
		return;
	}
	//Transfer the new package
	Wire.beginTransmission(0x2A);
	while (leptonReadReg(0x2) & 0x01);
	Wire.requestFrom(0x2A, leptonReadReg(0x6));
	char leptonhw[33];
	Wire.readBytes(leptonhw, 32);
	Wire.endTransmission();
	//Detected Lepton2 Shuttered
	if (strstr(leptonhw, "05-060") != NULL) {
		leptonVersion = leptonVersion_2_Shutter;
	}
	//Detected Lepton3 Shuttered
	else if (strstr(leptonhw, "05-070") != NULL) {
		leptonVersion = leptonVersion_3_Shutter;
	}
	//Detected Lepton2 No-Shutter
	else {
		leptonVersion = leptonVersion_2_NoShutter;
	}
}

/* Set the shutter operation to manual */
void leptonSetShutterMode(bool automatic)
{
	//If there is no shutter, do not do anything
	if (shutterMode == shutterMode_none)
		return;

	//Set the shutter mode on the Lepton
	leptonSetFFCMode(automatic);

	//Set shutter mode
	if (automatic)
		shutterMode = shutterMode_auto;
	else
		shutterMode = shutterMode_manual;
}

/* Set the radiometry mode (not used) */
void leptonRadSet(bool enable)
{
	//Enable or disable radiometry
	Wire.beginTransmission(0x2A);
	Wire.write(0x00);
	Wire.write(0x08);
	Wire.write(0x00);
	Wire.write(enable);
	Wire.endTransmission();
	//Data length
	Wire.beginTransmission(0x2A);
	Wire.write(0x00);
	Wire.write(0x06);
	Wire.write(0x00);
	Wire.write(0x02);
	Wire.endTransmission();
	//RAD module with OEM bit and command
	Wire.beginTransmission(0x2A);
	Wire.write(0x00);
	Wire.write(0x04);
	Wire.write(0x4E);
	Wire.write(0x11);
	Wire.endTransmission();
}

/* Check which hardware revision of the FLIR Lepton is connected */
void initLepton() {
	//Short delay
	delay(1500);

	//Check the Lepton HW Revision
	leptonCheckVersion();

	//Perform FFC if shutter is attached
	if (leptonVersion != leptonVersion_2_NoShutter) {
		shutterMode = shutterMode_auto;
		leptonRunCalibration();
	}
	//No shutter attached
	else
		shutterMode = shutterMode_none;

	//Set the calibration timer
	calTimer = millis();
	//Set calibration status to warmup if not coming from mass storage
	calStatus = cal_warmup;
	//Set the compensation value to zero
	calComp = 0;

	//Activate auto mode
	autoMode = true;
	//Deactivate limits Locked
	limitsLocked = false;

	//Check if SPI works
	leptonBeginSPI();
	do {
		digitalWriteFast(pin_lepton_cs, LOW);
		SPI.transfer(leptonFrame, 164);
		digitalWriteFast(pin_lepton_cs, HIGH);
	}
	//Repeat as long as the frame is not valid, equals sync
	while (((leptonFrame[0] & 0x0F) == 0x0F) && ((millis() - calTimer) < 1000));
	leptonEndSPI();
	//If sync not received after a second, show error message
	if ((leptonFrame[0] & 0x0F) == 0x0F) {
		showFullMessage((char*) "Lepton SPI is not working!");
		delay(1000);
		setDiagnostic(diag_lep_data);
	}
}