/*
 * BlinkAgent.h
 *
 * Blink an external LED as a task
 *
 *  Created on: 7-Aug-2022
 *      Author: jondurrant
 */

#ifndef BLINKAGENT_H_
#define BLINKAGENT_H_

#include "pico/stdlib.h"
#include "stdio.h"

class BlinkAgent {
public:

	/***
	 * Constructor
	 * €param name - task name
	 * @param gp - GP Pad number for LED
	 * @param delay - delay in microseconds
	 */
	BlinkAgent(const char *name, uint8_t gp, unsigned int delay = 500);
	virtual ~BlinkAgent();

	/***
	 *  create the vtask, will get picked up by scheduler
	 *
	 *  */
	virtual  bool start(UBaseType_t priority = tskIDLE_PRIORITY);

	/***
	 * Stop task
	 * @return
	 */
	virtual void stop();

	/***
	 * Get high water for stack
	 * @return close to zero means overflow risk
	 */
	virtual unsigned int getStakHighWater();

protected:

	void run();

	static void vTask( void * pvParameters );


private:
	//The task
	TaskHandle_t xHandle = NULL;

	uint8_t xGP = 0;

	char pName[20];

	uint xDelay;
};

#endif /* BLINKAGENT_H_ */
