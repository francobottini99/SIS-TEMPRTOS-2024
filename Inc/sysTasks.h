#ifndef __SYS_TASKS_H__
#define __SYS_TASKS_H__

#include "../FreeRTOS/Source/include/FreeRTOS.h"
#include "../FreeRTOS/Source/include/task.h"
#include "../FreeRTOS/Source/include/queue.h"

#include "microHandler.h"

#define PI        3.14159265358979323846
#define RAND_SEED 987654321

#define DEFAULT_FILTER_FACTOR  (5)

#define SENSOR_TASK_PRIORITY   (configMAX_PRIORITIES - 1)
#define FILTER_TASK_PRIORITY   (configMAX_PRIORITIES - 2)
#define COMMAND_TASK_PRIORITY  (configMAX_PRIORITIES - 3)
#define GRAPH_TASK_PRIORITY    (configMAX_PRIORITIES - 4)
#define STATS_TASK_PRIORITY    (configMAX_PRIORITIES - 5)

#define SENSOR_TASK_STACK_SIZE  (40)
#define FILTER_TASK_STACK_SIZE  (48)
#define GRAPH_TASK_STACK_SIZE   (52)
#define STATS_TASK_STACK_SIZE   (60)
#define COMMAND_TASK_STACK_SIZE (60)

#define SENSOR_TASK_DELAY      ( ( TickType_t ) 100 / portTICK_PERIOD_MS )
#define STATS_TASK_DELAY       ( ( TickType_t ) 2000 / portTICK_PERIOD_MS )
#define COMMAND_TASK_DELAY     ( ( TickType_t ) 1000 / portTICK_PERIOD_MS )

static void vSysTasksSensor(void *pvParameters);
static void vSysTasksFilter(void *pvParameters);
static void vSysTasksGraph(void *pvParameters);
static void vSysTasksStats(void *pvParameters);
static void vSysTasksCommand(void *pvParameters);

void vSysTasksInit(void);

#endif // __SYS_TASKS_H__