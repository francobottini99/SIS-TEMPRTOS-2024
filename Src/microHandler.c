#include "../Inc/microHandler.h"

unsigned long runtimeCounter = 0;

static int displayValues[DISPLAY_MAX_VALUES] = {0};

static const unsigned char dsimVerticalLine[] = { 0xFF };
static const unsigned char dsimPoint[]        = { 0x80 };
static const unsigned char dsimFive[]         = { 0x06, 0x49, 0x49, 0x30 };
static const unsigned char dsimZero[]         = { 0x3E, 0x41, 0x41, 0x3E };

static int iMicroHandlerRowFromValue(int value) 
{
    return value >= 24 ? 0 : 1;
}

static const unsigned char* pucMicroHandlerNormalizeValue(int value) 
{
    static unsigned char result;

    if (value >= 48)
        result = 0x01;
    else if (value >= 24)
    {
        value -= 24;
        result = 0x80 >> value / 3;
    }
    else
    {
        result  = 0x80 >> value / 3;
        result |= 0x80;
    }
    
    return &result;
}

static void vMicroHandlerRefreshDisplay(int displayIndex)
{
    OSRAMClear();

    //eje X
    for (int i = 0; i < DISPLAY_MAX_VALUES ; i++)
        OSRAMImageDraw(dsimPoint, i + 11, 1, 2, 1);
        
    // data
    for (int i = 0; i < DISPLAY_MAX_VALUES; i++)
        OSRAMImageDraw(pucMicroHandlerNormalizeValue(displayValues[i]), i + 11, iMicroHandlerRowFromValue(displayValues[i]), 1, 1);

    // eje Y
    OSRAMImageDraw(dsimVerticalLine, 11, 0, 1, 1);
    OSRAMImageDraw(dsimVerticalLine, 11, 1, 1, 1);

    // etiquetas
    OSRAMImageDraw(dsimFive, 1, 0, 4, 1); //Numero 5
    OSRAMImageDraw(dsimZero, 6, 0, 4, 1); //Numero 0
    OSRAMImageDraw(dsimZero, 6, 1, 4, 1); //Numero 0 inferior    
}

static void vMicroHandlerTmr0ISR(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    runtimeCounter++;
}

int vMicroHandlerUARTGetStr(unsigned char *buffer, int maxLen)
{
    int i = 0;

    while (i <= maxLen && UARTCharsAvail(UART0_BASE)) 
        buffer[i++] = UARTCharGet(UART0_BASE);

    buffer[i] = '\0';

    return i > 0 ? (i > maxLen ? -1 : 1) : 0;
}

void vMicroHandlerUARTSendStr(const unsigned char *str, int newLine)
{
    while (*str != '\0') 
    {
        UARTCharPut(UART0_BASE, *str);
        str++;
    }

    UARTCharPut(UART0_BASE, '\0');

    if (newLine)
        UARTCharPut(UART0_BASE, '\n');
}

void vMicroHandlerUARTSendInt(int value, int newLine)
{
    char str[32];
    vUtilsIntToStr(value, str);
    vMicroHandlerUARTSendStr(str, newLine);
}

unsigned long vMicroHandlerGetRunTimeCounter(void)
{
    return runtimeCounter;
}

void vMicroHandlerConfigStatsTmr(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    IntMasterEnable();
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerConfigure(TIMER0_BASE,TIMER_CFG_32_BIT_TIMER);
    TimerLoadSet(TIMER0_BASE, TIMER_A, 1500);
    TimerIntRegister(TIMER0_BASE,TIMER_A, vMicroHandlerTmr0ISR);
    TimerEnable(TIMER0_BASE,TIMER_A);
}

void vMicroHandlerInit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    UARTConfigSet(UART0_BASE, UART_BAUD_RATE, UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE);

    OSRAMInit(false);
}

void vMicroHandlerDisplayAddValue(int value)
{
    static int displayIndex = 0;

    if (displayIndex >= DISPLAY_MAX_VALUES) 
    {
        vUtilsShiftLeftArray(displayValues, DISPLAY_MAX_VALUES);
        displayIndex--;
    }

    displayValues[displayIndex++] = value;

    vMicroHandlerRefreshDisplay(displayIndex);
}