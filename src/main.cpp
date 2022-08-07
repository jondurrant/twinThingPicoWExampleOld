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


#include "MQTTAgent.h"
#include "MQTTRouterPing.h"
#include "MQTTRouterTwin.h"
#include <WifiHelper.h>
#include "StateExample.h"
#include "ConnectionObserver.h"

#include "RGBLEDAgent.h"



#define TEST_TASK_PRIORITY			( tskIDLE_PRIORITY + 1UL )

#ifndef MQTTHOST
#define MQTTHOST "piudev2.local.jondurrant.com"
#define MQTTPORT 3881 //1883
#define MQTTUSER "MAC"
#define MQTTPASSWD "MAC"
#endif

void runTimeStats(   ){
	//char pcWriteBuffer[120] = "";
	TaskStatus_t *pxTaskStatusArray;
	volatile UBaseType_t uxArraySize, x;
	unsigned long ulTotalRunTime;

   /* Make sure the write buffer does not contain a string. */
   //*pcWriteBuffer = 0x00;

   /* Take a snapshot of the number of tasks in case it changes while this
   function is executing. */
   uxArraySize = uxTaskGetNumberOfTasks();
   printf("Number of tasks %d\n", uxArraySize);

   /* Allocate a TaskStatus_t structure for each task.  An array could be
   allocated statically at compile time. */
   pxTaskStatusArray = (TaskStatus_t *)pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );

   if( pxTaskStatusArray != NULL ){
      /* Generate raw status information about each task. */
      uxArraySize = uxTaskGetSystemState( pxTaskStatusArray,
                                 uxArraySize,
                                 &ulTotalRunTime );



	 /* For each populated position in the pxTaskStatusArray array,
	 format the raw data as human readable ASCII data. */
	 for( x = 0; x < uxArraySize; x++ )
	 {
		 printf("Task: %d \t cPri:%d \t bPri:%d \t hw:%d \t%s\n",
				pxTaskStatusArray[ x ].xTaskNumber,
				pxTaskStatusArray[ x ].uxCurrentPriority ,
				pxTaskStatusArray[ x ].uxBasePriority,
				pxTaskStatusArray[ x ].usStackHighWaterMark.
				pxTaskStatusArray[ x ].pcTaskName
				);
	 }


      /* The array is no longer needed, free the memory it consumes. */
      vPortFree( pxTaskStatusArray );
   } else {
	   printf("Failed to allocate space for stats\n");
   }

   HeapStats_t heapStats;
   vPortGetHeapStats(&heapStats);
   printf("HEAP avl: %d, blocks %d, alloc: %d, free: %d\n",
		   heapStats.xAvailableHeapSpaceInBytes,
		   heapStats.xNumberOfFreeBlocks,
		   heapStats.xNumberOfSuccessfulAllocations,
		   heapStats.xNumberOfSuccessfulFrees
		   );

}


void main_task(void *params){

	printf("Main task started\n");
	printf("Main HW: %d\n", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));


	RGBLEDAgent serviceLED("Service LED", 2,3,4);
	if (!serviceLED.start(TEST_TASK_PRIORITY)){
		printf("Failed to start serviceLED Agent\n");
	}
	ConnectionObserver connObs(&serviceLED);
	connObs.LinkDown();

	int res = cyw43_arch_init();
	printf("cyw42 init %d\n", res);
	if (res) {
	//if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK)){
		printf("failed to initialise\n");
		connObs.LinkFailed();
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
			printf("Failed to join AP.\n");
			 vTaskDelay(2000);
		}
	}

	connObs.APJoined();
	printf("Main HW: %d\n", uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));





	RGBLEDAgent rgbLED("Lamp LED", 6,7,8);
	if (!rgbLED.start(TEST_TASK_PRIORITY)){
		printf("Failed to start RGB LED Agent\n");
	}
	printf("RGB HW:%d\n", rgbLED.getStakHighWater());
	printf("serviceLED HW:%d\n", serviceLED.getStakHighWater());

	rgbLED.set(RGBModeOn, 0xFF, 0xFF, 0xFF);
	serviceLED.set(RGBModeOn, 0xFF, 0x00, 0x00);

	char ipStr[20];
	WifiHelper::getIPAddressStr(ipStr);
	printf("IP ADDRESS: %s\n", ipStr);


	char mqttTarget[] = MQTTHOST;
	int mqttPort = MQTTPORT;
	char mqttUser[] = MQTTUSER;
	char mqttPwd[] = MQTTPASSWD;
	MQTTRouterTwin mqttRouter;
	StateExample state;

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
	mqttAgent.setObserver(&connObs);
	mqttAgent.setRouter(&mqttRouter);
	mqttAgent.mqttConnect(mqttTarget, mqttPort, true, false);
	mqttAgent.start(tskIDLE_PRIORITY+1);



	uint8_t i=0;
    while(true) {

    	runTimeStats();

        vTaskDelay(1000);

        if (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) < 0){
        	connObs.LinkDown();
        	printf("AP Link is down\n");
        	while (r < 0){
				r = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 60000);
				printf("Connect r %d\n", r);
			   // if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 60000)) {
				if (r){
					printf("Failed to join AP.\n");
					 vTaskDelay(2000);
				}
			}
        	connObs.APJoined();
        }

        i++;
        switch(i){
        	case 1: {
				rgbLED.set(RGBModeOn, 0xFF, 0x00, 0x00);
				printf("Red\n");
				break;
        	}
        	case 2: {
				rgbLED.set(RGBModeOn, 0x00, 0xFF, 0x00);
				printf("Green\n");
				break;
			}
        	case 3: {
				rgbLED.set(RGBModeOn, 0x00, 0x00, 0xFF);
				printf("Blue\n");
				break;
			}
        	case 4: {
				rgbLED.set(RGBModeFast, 0xFF, 0x00, 0x00);
				printf("Red\n");
				break;
        	}
        	case 5: {
				rgbLED.set(RGBModeFast, 0x00, 0xFF, 0x00);
				printf("Green\n");
				break;
			}
        	case 6: {
				rgbLED.set(RGBModeFast, 0x00, 0x00, 0xFF);
				printf("Blue\n");
				break;
			}
        	case 7: {
        		rgbLED.set(RGBModeOn, 0xFF, 0xFF, 0xFF);
        		printf("White\n");
        		break;
        	}
        	case 8: {
				rgbLED.set(RGBModeOnce, 0xFF, 0x00, 0x00);
				printf("Red\n");
				break;
			}
			case 9: {
				rgbLED.set(RGBModeOnce, 0x00, 0xFF, 0x00);
				printf("Green\n");
				break;
			}
			case 10: {
				rgbLED.set(RGBModeOnce, 0x00, 0x00, 0xFF);
				printf("Blue\n");
				break;
			}
        	default: {
        		i=0;
        	}
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
