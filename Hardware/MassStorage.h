/*
*
* MASS STORAGE -  Boot into the mass storage mode to connect the internal storage to the PC
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

/* Defines */

#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART() (*CPU_RESTART_ADDR = CPU_RESTART_VAL);
#define IS_JUMP_TO_OFFSET_FLAG_SET()  (eeprom_read_byte(0x00) == 0xAE)
#define CLEAR_JUMP_FLAG()             eeprom_write_byte(0x00, 0)
#define SET_JUMP_FLAG()               eeprom_write_byte(0x00, 0xAE)


/* Methods */

//The assembly code hook must be run inside a C, not C++ function
extern "C" {

	/* Load stack pointer and program counter from start of new program */
	void jumpToApplicationAt0x38980() {
		asm("movw r0, #0x8880");
		asm("movt r0, #0x0003");
		asm("ldr sp, [r0]");
		asm("ldr pc, [r0, #4]");
	}

	/* Set peripherals back to reset conditions */
	void resetPeripherals() {
		//Set (some of) USB back to normal
		NVIC_DISABLE_IRQ(IRQ_USBOTG);
		NVIC_CLEAR_PENDING(IRQ_USBOTG);
		SIM_SCGC4 &= ~(SIM_SCGC4_USBOTG);

		//Disable all GPIO interrupts
		NVIC_DISABLE_IRQ(IRQ_PORTA);
		NVIC_DISABLE_IRQ(IRQ_PORTB);
		NVIC_DISABLE_IRQ(IRQ_PORTC);
		NVIC_DISABLE_IRQ(IRQ_PORTD);
		NVIC_DISABLE_IRQ(IRQ_PORTE);

		//Set (some of) ADC1 back to normal
		//Wait until calibration is complete
		while (ADC1_SC3 & ADC_SC3_CAL)
			;
		//Clear flag if calibration failed
		if (ADC1_SC3 & 1 << 6)
			ADC1_SC3 |= 1 << 6;

		//Clear conversion complete flag (which could trigger ISR otherwise)
		if (ADC1_SC1A & 1 << 7)
			ADC1_SC1A |= 1 << 7;

		//Set some clocks back to default/reset settings
		MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(4);
		SIM_CLKDIV1 = 0;
		SIM_CLKDIV2 = 0;
	}

	/* Look for the condition that indicates we want to jump to the application with offset */
	void startup_late_hook(void) {
		if (IS_JUMP_TO_OFFSET_FLAG_SET()) {
			//Clear the condition
			CLEAR_JUMP_FLAG();
			//Set peripherals (mostly) back to normal then jump
			__disable_irq();
			resetPeripherals();
			jumpToApplicationAt0x38980();
		}
	}
}

/* Jump to the mass storage section */
void restartAndJumpToApp(void) {
	SET_JUMP_FLAG();
	CPU_RESTART()
		;
}

/* Asks the user if he really wants to enter mass storage mode */
bool massStoragePrompt() {
	//Title & Background
	drawTitle((char*) "USB File Transfer");
	display.setColor(VGA_BLACK);
	display.setFont(smallFont);
	display.setBackColor(200, 200, 200);
	display.print((char*)"Do you want to enter mass storage", CENTER, 65);
	display.print((char*)"to transfer files/videos to the PC?", CENTER, 85);
	display.print((char*)"Do not use this for firmware updates", CENTER, 105);
	display.print((char*)"or for the USB serial connection.", CENTER, 125);
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

/* Go into mass storage mode */
void massStorage() {
	//Old hardware
	if (mlx90614Version == mlx90614Version_old) {
		//Display error msg for 1sec
		showFullMessage((char*) "Your HW does not support this!");
		delay(1000);
		//Go back
		return;
	}
	//Check if the user really wants to do it
	if (!massStoragePrompt())
		return;
	//Show message
	showFullMessage((char*) "Disconnect USB cable to return!");
	delay(1500);
	//Set marker
	EEPROM.write(eeprom_massStorage, eeprom_setValue);
	restartAndJumpToApp();
}