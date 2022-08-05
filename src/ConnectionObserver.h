/*
 * ConnectionObserver.h
 *
 *  Created on: 5 Aug 2022
 *      Author: jondurrant
 */

#ifndef SRC_CONNECTIONOBSERVER_H_
#define SRC_CONNECTIONOBSERVER_H_

#include "MQTTAgentObserver.h"
#include "RGBLEDAgent.h"

class ConnectionObserver : public  MQTTAgentObserver{
public:
	ConnectionObserver(RGBLEDAgent * p);
	virtual ~ConnectionObserver();

	virtual void MQTTOffline();

	virtual void MQTTOnline();

	virtual void MQTTSend();

	virtual void MQTTRecv();

	void LinkDown();

	void APJoined();

	void LinkFailed();

private:
	RGBLEDAgent * pRGBAgent = NULL;
};

#endif /* SRC_CONNECTIONOBSERVER_H_ */
