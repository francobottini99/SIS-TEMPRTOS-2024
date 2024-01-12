#include "../Inc/sysTasks.h"

static TaskHandle_t xSensorTaskHandle;
static TaskHandle_t xFilterTaskHandle;
static TaskHandle_t xGraphTaskHandle;
static TaskHandle_t xStatsTaskHandle;
static TaskHandle_t xCommandTaskHandle;

static QueueHandle_t xSensorQueue;
static QueueHandle_t xFilterQueue;

static TaskStatus_t *pxTaskStatusArray;

static filterFactor = DEFAULT_FILTER_FACTOR;

static void vSysTasksSensor(void *pvParameters) 
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    int tempValue = rand() % 51;

    while (1) 
    {
        tempValue += rand() % 3 - 1;

        tempValue = tempValue > 50 ? 50 : tempValue;
        tempValue = tempValue < 0 ? 0 : tempValue;

        xQueueSend(xSensorQueue, &tempValue, portMAX_DELAY);

        vTaskDelayUntil(&xLastWakeTime, SENSOR_TASK_DELAY);
    }
}

static void vSysTasksFilter(void *pvParameters) 
{
    int tempValue, filteredValue;
    int sum = 0;

    while (1) 
    {
        xQueueReceive(xSensorQueue, &tempValue, portMAX_DELAY);

        sum = sum - (sum / filterFactor) + tempValue;
        
        filteredValue = sum / filterFactor;

        xQueueSend(xFilterQueue, &filteredValue, portMAX_DELAY);
    }
}

static void vSysTasksGraph(void *pvParameters) 
{
    int tempValue;

    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1) 
    {
        xQueueReceive(xFilterQueue, &tempValue, portMAX_DELAY);

        vMicroHandlerDisplayAddValue(tempValue);
    }
}

static void vSysTasksStats(void *pvParameters) 
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    volatile UBaseType_t uxArraySize = uxTaskGetNumberOfTasks();
    pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );
    
    while (1) 
    {
        volatile UBaseType_t x;
        unsigned long ulTotalRunTime, ulStatsAsPercentage;

        if( pxTaskStatusArray != NULL )
        {
            uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime );

            ulTotalRunTime /= 100UL;

            if( ulTotalRunTime > 0 )
            {
                vMicroHandlerUARTSendStr("\n", 0);

                vMicroHandlerUARTSendStr("TASK", 0);

                for (int i = 0; i < configMAX_TASK_NAME_LEN - strlen("TASK"); i++)
                    vMicroHandlerUARTSendStr(" ", 0);

                vMicroHandlerUARTSendStr("\t|TICKS\t\t|CPU %\t|STACK FREE\r\n", 0);

                for( x = 0; x < uxArraySize; x++ )
                {
                    ulStatsAsPercentage = pxTaskStatusArray[ x ].ulRunTimeCounter / ulTotalRunTime;

                    vMicroHandlerUARTSendStr(pxTaskStatusArray[ x ].pcTaskName, 0);
                    
                    for (int i = 0; i < configMAX_TASK_NAME_LEN - strlen(pxTaskStatusArray[ x ].pcTaskName); i++)
                        vMicroHandlerUARTSendStr(" ", 0);

                    vMicroHandlerUARTSendStr("\t|", 0);
                    vMicroHandlerUARTSendInt(pxTaskStatusArray[ x ].ulRunTimeCounter, 0);
                    vMicroHandlerUARTSendStr("\t\t|", 0);

                    if( ulStatsAsPercentage > 0UL )
                        vMicroHandlerUARTSendInt(ulStatsAsPercentage, 0);
                    else
                        vMicroHandlerUARTSendInt(0, 0);

                    vMicroHandlerUARTSendStr("%\t|", 0);
                    vMicroHandlerUARTSendInt(pxTaskStatusArray[x].usStackHighWaterMark, 0);
                    vMicroHandlerUARTSendStr(" Words\r\n", 0);
                }
            }
        }

        vTaskDelayUntil(&xLastWakeTime, STATS_TASK_DELAY);
    }
}

static void vSysTasksCommand(void *pvParameters) 
{
    char buffer[32];
    int result;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1) 
    {
        result = vMicroHandlerUARTGetStr(buffer, 32);

        if (result == -1)
            vMicroHandlerUARTSendStr("\nCommand too long\n", 0);
        else if (result == 1)  
        {
            if (strstr(buffer, "ff ") != NULL)
            {
                int newFilterFactor = vUtilsStrToInt(buffer + 3);

                if (newFilterFactor > 0)
                {
                    filterFactor = newFilterFactor;
                    vMicroHandlerUARTSendStr("\nFilter factor set in: ", 0);
                    vMicroHandlerUARTSendInt(filterFactor, 1);
                }
                else
                {
                    vMicroHandlerUARTSendStr("\nInvalid filter factor ! Please set a valid integer value greater than zero.\n", 0);
                    vMicroHandlerUARTSendStr("Initialization with default value: ", 0);
                    vMicroHandlerUARTSendInt(filterFactor, 1);
                }
            }
            else
                vMicroHandlerUARTSendStr("\nInvalid command.\n", 0);
        }

        vTaskDelayUntil(&xLastWakeTime, COMMAND_TASK_DELAY);
    }
}

void vSysTasksInit(void)
{
    xSensorQueue = xQueueCreate(10, sizeof(int));
    xFilterQueue = xQueueCreate(10, sizeof(int));

    xTaskCreate(vSysTasksSensor, "SysTasksSensor", SENSOR_TASK_STACK_SIZE, NULL, SENSOR_TASK_PRIORITY, &xSensorTaskHandle);
    xTaskCreate(vSysTasksFilter, "SysTasksFilter", FILTER_TASK_STACK_SIZE, NULL, FILTER_TASK_PRIORITY, &xFilterTaskHandle);
    xTaskCreate(vSysTasksGraph, "SysTasksGraph", GRAPH_TASK_STACK_SIZE, NULL, GRAPH_TASK_PRIORITY, &xGraphTaskHandle);
    xTaskCreate(vSysTasksStats, "SysTasksStats", STATS_TASK_STACK_SIZE, NULL, STATS_TASK_PRIORITY, &xStatsTaskHandle);
    xTaskCreate(vSysTasksCommand, "SysTasksCommand", COMMAND_TASK_STACK_SIZE, NULL, COMMAND_TASK_PRIORITY, &xCommandTaskHandle);

    srand(RAND_SEED);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    vMicroHandlerUARTSendStr("", 1);
    vMicroHandlerUARTSendStr("---------------- Task Error ----------------", 1);
    vMicroHandlerUARTSendStr("Stack Overflow: ", 0);
    vMicroHandlerUARTSendStr(pcTaskName, 1);
    vMicroHandlerUARTSendStr("--------------------------------------------", 1);

    vTaskDelete(xTask);
}