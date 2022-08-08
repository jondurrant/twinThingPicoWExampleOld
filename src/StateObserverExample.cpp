/*
 * StateObserverExample.cpp
 *
 *  Created on: 8 Aug 2022
 *      Author: jondurrant
 */

#include "StateObserverExample.h"

StateObserverExample::StateObserverExample(RGBLEDAgent *agent, StateExample *state) {
	pAgent = agent;
	pState = state;
}

StateObserverExample::~StateObserverExample() {
	// NOP
}


/***
* Notification of a change of a state item with the State object.
* @param dirtyCode - Representation of item changed within state. Used to pull back delta
*/
void StateObserverExample::notifyState(uint16_t dirtyCode){
	RGBMode mode = RGBModeOff;

	if (pState->getOn()){
		mode = RGBModeOn;
	}
	const uint8_t *rgb = pState->getRGB();

	pAgent->set(mode, rgb[0], rgb[1], rgb[2]);

}
