#include <sys/times.h>
#include "lift.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "stdbool.h"
#include "pid.h"
#include "pole.h"

#define UP		0
#define DOWN	1
#define COMM_TASK_PRIORITY ( configMAX_PRIORITIES - 2 )

extern QueueHandle_t lift_queue;
extern QueueHandle_t queue_pole;

static void comm_task(void *par)
{
	bool dir = UP;
	struct lift_msg *lift_msg_snd;
	struct pole_msg *pole_msg_snd;

	while (1) {
		dir = !dir;
		lift_msg_snd = (struct lift_msg*) malloc(sizeof(struct lift_msg));
		lift_msg_snd->type = dir;
		if (xQueueSend(lift_queue, &lift_msg_snd,
				(TickType_t) 10) == pdPASS) {
			printf("comm: lift command sent \n");
		} else {
			printf("comm: unable to send lift command \n");
		}

		struct tms time;
		srandom(times(&time));

		pole_msg_snd = (struct pole_msg*) malloc(sizeof(struct pole_msg*));
		pole_msg_snd->closed_loop_setpoint = random() % ((2 ^ 16) - 1);
		if (xQueueSend(queue_pole, &pole_msg_snd,
				(TickType_t) 10) == pdPASS) {
			printf("comm: pole command sent \n");
		} else {
			printf("comm: unable to send pole command \n");
		}

		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void comm_init()
{
	xTaskCreate(comm_task, "Comm", configMINIMAL_STACK_SIZE, NULL,
	COMM_TASK_PRIORITY, NULL);
}

void task_status_get_all() {
	union {
		//	struct arm_status;
			struct pole_status;
			struct lift_status;
	} status;

	status = arm_status_get();

//	status.dirArm;
//	status.posCmdArm;
//	status.posActArm;
//	status.velArm;
//	status.cwLimitArm;
//	status.ccwLimitArm;
//	status.stalled;


	status = pole_status_get();

//	status.dirPole;
//	status.posCmdPole;
//	status.posActPole;
//	status.velPole;
//	status.cwLimitPole;
//	status.ccwLimitPole;
//	status.stalled;

	status = lift_status_get();

//	status.dir;
//	status.limitUp;
//	status.limitDown;


}
