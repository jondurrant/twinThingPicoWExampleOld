/*
 * ConnectionObserver.cpp
 *
 *  Created on: 5 Aug 2022
 *      Author: jondurrant
 */

#include "ConnectionObserver.h"

ConnectionObserver::ConnectionObserver(RGBLEDAgent * p) {
	pRGBAgent = p;
}

ConnectionObserver::~ConnectionObserver() {
	// NOP
}

void ConnectionObserver::MQTTOffline(){
	pRGBAgent->set(RGBModeOn, 0xFF, 0xBF, 0x00);
}

void ConnectionObserver::MQTTOnline(){
	pRGBAgent->set(RGBModeOn, 0x00, 0x00, 0xFF);
}

void ConnectionObserver::MQTTSend(){
	pRGBAgent->set(RGBModeOnce, 0x00, 0xFF, 0x00);
}

void ConnectionObserver::MQTTRecv(){
	pRGBAgent->set(RGBModeOnce, 0xFF, 0x00, 0x00);
}

void ConnectionObserver::LinkDown(){
	pRGBAgent->set(RGBModeOn, 0xFF, 0x00, 0x00);
}

void ConnectionObserver::APJoined(){
	pRGBAgent->set(RGBModeOn, 0x00, 0xFF, 0x00);
}

void ConnectionObserver::LinkFailed(){
	pRGBAgent->set(RGBModeSlow, 0xFF, 0x00, 0x00);
}
