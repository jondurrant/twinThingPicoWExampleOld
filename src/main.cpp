/***
 * Test program
 */

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <stdio.h>

#include "lwip/ip4_addr.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include "BlinkPicoWTask.h"

#include "MQTTAgent.h"
#include "MQTTRouterPing.h"
#include "MQTTRouterTwin.h"
#include <WifiHelper.h>
#include "StateExample.h"
#include "ExampleAgentObserver.h"



#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )

#ifndef MQTTHOST
#define MQTTHOST "piudev2.local.jondurrant.com"
#define MQTTPORT 1883
#define MQTTUSER "MAC"
#define MQTTPASSWD "MAC"
#endif


void main_task(void *params){

	printf("Main task started\n");

	int res = cyw43_arch_init();
	printf("cyw42 init %d\n", res);
	if (res) {
	//if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK)){
		printf("failed to initialise\n");
		return;
	}

	cyw43_wifi_pm(&cyw43_state ,CYW43_AGGRESSIVE_PM);

	cyw43_arch_enable_sta_mode();
	printf("Connecting to WiFi... %s [%s]\n", WIFI_SSID, WIFI_PASSWORD);

	int r =-1;

	while (r < 0){
		r = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 60000);
		printf("Connect r %d\n", r);
	   // if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 60000)) {
		if (r){
			printf("failed to connect.\n");
			 vTaskDelay(2000);
		}
	}



	BlinkPicoWTask blink;
	blink.start(TEST_TASK_PRIORITY+1);


	char mqttTarget[] = MQTTHOST;
	int mqttPort = MQTTPORT;
	char mqttUser[] = MQTTUSER;
	char mqttPwd[] = MQTTPASSWD;
	MQTTRouterTwin mqttRouter;
	StateExample state;
	ExampleAgentObserver agentObs;

	MQTTAgent mqttAgent;
	TwinTask xTwin;
	MQTTPingTask xPing;

	mqttAgent.credentials(mqttUser, mqttPwd);
	mqttRouter.init(mqttAgent.getId(), &mqttAgent);

	printf("Connecting to: %s(%d) as %s:%s:%s\n",
			mqttTarget, mqttPort, mqttAgent.getId(),
			mqttUser, mqttPwd
			);

	//Twin agent to manage the state
	xTwin.setStateObject(&state);
	xTwin.setMQTTInterface(&mqttAgent);
	xTwin.start(tskIDLE_PRIORITY+1);

	//Start up a Ping agent to mange ping requests
	xPing.setInterface(&mqttAgent);
	xPing.start(tskIDLE_PRIORITY+1);

	//Give the router the twin and ping agents
	mqttRouter.setTwin(&xTwin);
	mqttRouter.setPingTask(&xPing);

	//Setup and start the mqttAgent
	mqttAgent.setObserver(&agentObs);
	mqttAgent.setRouter(&mqttRouter);
	mqttAgent.mqttConnect(mqttTarget, mqttPort, true, false);
	mqttAgent.start(tskIDLE_PRIORITY+1);



    while(true) {
        vTaskDelay(1000);
        if (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) < 0){
        	printf("AP Link is down\n");
        }
    }

    cyw43_arch_deinit();
}





void vLaunch( void) {
    TaskHandle_t task;
    //xTaskCreate(main_task, "TestMainThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &task);

    xTaskCreate(main_task, "TestMainThread", 2048, NULL, TEST_TASK_PRIORITY, &task);


#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
    // we must bind the main task to one core (well at least while the init is called)
    // (note we only do this in NO_SYS mode, because cyw43_arch_freertos
    // takes care of it otherwise)
    vTaskCoreAffinitySet(task, 1);
#endif

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}


int main( void )
{
    stdio_init_all();
    sleep_ms(2000);
    printf("GO\n");




    /* Configure the hardware ready to run the demo. */
    const char *rtos_name;
#if ( portSUPPORT_SMP == 1 )
    rtos_name = "FreeRTOS SMP";
#else
    rtos_name = "FreeRTOS";
#endif

#if ( portSUPPORT_SMP == 1 ) && ( configNUM_CORES == 2 )
    printf("Starting %s on both cores:\n", rtos_name);
    vLaunch();
#elif ( RUN_FREE_RTOS_ON_CORE == 1 )
    printf("Starting %s on core 1:\n", rtos_name);
    multicore_launch_core1(vLaunch);
    while (true);
#else
    printf("Starting %s on core 0:\n", rtos_name);
    vLaunch();
#endif
    return 0;
}
