/*
 * RGBpwmTask.h
 *
 * This is a task class to manage an RGB led. Common Cathode
 *
 *  Created on: 2 Aug 2021
 *      Author: jondurrant
 */

#ifndef RGBLEDAGENT_H_
#define RGBLEDAGENT_H_

#include "RGBpwm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


class RGBLEDAgent  {
public:

	/***
	 * Construct an RGB LED Agent to manage LED colour
	 * PINS are GPIO Pins for the Common Cathode LED
	 * @param redPin
	 * @param greenPin
	 * @param bluePin
	 */
	RGBLEDAgent(unsigned char redPin, unsigned char greenPin, unsigned char bluePin);


	/***
	 * Destructor
	 */
	virtual ~RGBLEDAgent();


	/***
	 * Queue a change to the LED mode and colour
	 * 8 bit colour used
	 * @param m - Mode
	 * @param r - Red
	 * @param g - Green
	 * @param b - Blue
	 * @return
	 */
	BaseType_t set(RGBMode m, unsigned char r, unsigned char g, unsigned char b);

	/***
	 *  create the vtask
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
	/***
	 * Internal function to run the task from within the object
	 */
	void run();

	/***
	 * Internal function used by FreeRTOS to run the task
	 * @param pvParameters
	 */
	static void vTask( void * pvParameters );

private:
	RGBpwm rgbPWM;
	QueueHandle_t xRGBQueue = NULL;

	//The task
	TaskHandle_t xHandle = NULL;


};

#endif /* RGBLEDAGENT_H_ */
