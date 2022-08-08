/*
 * StateObserverExample.h
 *
 *  Created on: 8 Aug 2022
 *      Author: jondurrant
 */

#ifndef SRC_STATEOBSERVEREXAMPLE_H_
#define SRC_STATEOBSERVEREXAMPLE_H_

#include <StateObserver.h>
#include "RGBLEDAgent.h"
#include "StateExample.h"

class StateObserverExample : public StateObserver {
public:
	StateObserverExample(RGBLEDAgent *agent, StateExample *state);
	virtual ~StateObserverExample();

	/***
	 * Notification of a change of a state item with the State object.
	 * @param dirtyCode - Representation of item changed within state. Used to pull back delta
	 */
	virtual void notifyState(uint16_t dirtyCode);

private:
	RGBLEDAgent *pAgent = NULL;
	StateExample *pState = NULL;
};

#endif /* SRC_STATEOBSERVEREXAMPLE_H_ */
