/*
 * RGBSwitchObs.cpp
 *
 *  Created on: 8 Aug 2022
 *      Author: jondurrant
 */

#include "RGBSwitchObs.h"

RGBSwitchObs::RGBSwitchObs(StateExample *state) {
	pState = state;

}

RGBSwitchObs::~RGBSwitchObs() {
	// NOP
}



/***
 * Handle a short press from the switch
 * @param gp - GPIO number of the switch
 */
void RGBSwitchObs::handleShortPress(uint8_t gp){

	if (xSeq > xRainbowCount){
		xSeq = 0;
	}

	if (xSeq == 0){
		pState->setOn(true);
	}

	if (xSeq == xRainbowCount){
		pState->setOn(false);
	} else {
		pState->setRGB(pRainbow[xSeq]);
	}

	xSeq++;

}

/***
 * Handle a short press from the switch
 * @param gp - GPIO number of the switch
 */
void RGBSwitchObs::handleLongPress(uint8_t gp){
	handleShortPress(gp);

}
