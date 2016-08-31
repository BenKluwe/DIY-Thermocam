#ifndef _Touchscreen_h_
#define _Touchscreen_h_

#include "FT6206_Touchscreen.h"
#include "XPT2046_Touchscreen.h"

class Touchscreen {
public:
	//Resistive Touch Controller
	XPT2046_Touchscreen resTouch;
	//Capacitive Touch Controller
	FT6206_Touchscreen capTouch;
	//Pointer to the screen pressed marker
	volatile bool* screenPressed;
	//Choose the right touch screen
	bool capacitive = false;


	/* Check if the capacitive touch can be started, otherwise use resistive */
	void begin(volatile bool* screen_Pressed) {
		//Link to screen pressed marker
		screenPressed = screen_Pressed;
		//Capacitive screen
		if (capTouch.begin()) 
			capacitive = true;
		//Resistive screen or none
		else {
			resTouch.begin();
		}
	}

	/* Returns if the screen is currently touched */
	bool touched() {
		bool touch;
		//Check for touch, capacitive or resistive
		if (capacitive)
			touch = capTouch.touched();
		else
			touch = resTouch.touched();
		//If touch registered, set screen pressed marker
		if (touch)
			*screenPressed = true;
		return touch;
	}

	/* Set rotation for touch screen */
	void setRotation(bool rotated) {
		if (capacitive)
			capTouch.rotated = rotated;
		else
			resTouch.rotated = rotated;
	}

	/* Returns the coordinates of the touched point */
	TS_Point getPoint() {
		if (capacitive)
			return capTouch.getPoint();
		else
			return resTouch.getPoint();
	}
};

#endif