#include "../Inc/utils.h"

static void vUtilsReverseStr(char* str, int length) 
{
    int start = 0;
    int end = length - 1;

    while (start < end) 
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void vUtilsIntToStr(int num, char* str) 
{
    int i = 0;

    if (num == 0)
        str[i++] = '0';
    else
    {
        while (num) 
        {
            str[i++] = num % 10 + '0';
            num = num / 10;
        }
    }

    str[i] = '\0';

    vUtilsReverseStr(str, i);
}

int vUtilsStrToInt(const char *str) 
{
    int value = 0;

    while (*str >= '0' && *str <= '9') 
    {
        value = value * 10 + (*str - '0');
        ++str;
    }

    if (str != NULL && *str == '\0')
        return value;
    else
        return -1;
}

void vUtilsShiftLeftArray(int *array, int size) 
{
    for (int i = 0; i < size - 1; i++) 
        array[i] = array[i + 1];
}