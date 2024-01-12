#ifndef __MICRO_HANDLER__
#define __MICRO_HANDLER__

#include "../Drivers/LM3S811/DriverLib.h"
#include "../Drivers/LM3S811/hw_memmap.h"
#include "../Drivers/LM3S811/timer.h"
#include "../Drivers/LM3S811/hw_timer.h"

#include "../Inc/utils.h"

#define UART_BAUD_RATE (19200)

#define DISPLAY_WIDTH 96
#define DISPLAY_HEIGHT 16
#define DISPLAY_MAX_VALUES 83

extern unsigned long runtimeCounter;

static int iMicroHandlerRowFromValue(int value);

static const unsigned char* pucMicroHandlerNormalizeValue(int value);

static void vMicroHandlerRefreshDisplay(int displayIndex);
static void vMicroHandlerTmr0ISR(void);

unsigned long vMicroHandlerGetRunTimeCounter(void);

int vMicroHandlerUARTGetStr(unsigned char *buffer, int maxLen);

void vMicroHandlerUARTSendStr(const unsigned char *str, int newLine);
void vMicroHandlerUARTSendInt(int value, int newLine);
void vMicroHandlerInit(void);
void vMicroHandlerDisplayAddValue(int value);
void vMicroHandlerConfigStatsTmr(void);


#endif // __MICRO_HANDLER__