#include "../Inc/microHandler.h"
#include "../Inc/sysTasks.h"

int main() 
{
    vMicroHandlerInit();

    vSysTasksInit();

    vTaskStartScheduler();
    
    return 0;
}