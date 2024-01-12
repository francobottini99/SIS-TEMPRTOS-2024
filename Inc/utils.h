#ifndef __UTILS_H__
#define __UTILS_H__

#include <string.h>
#include <stdint.h>

static void vUtilsReverseStr(char* str, int length);

void vUtilsIntToStr(int num, char* str);
int vUtilsStrToInt(const char *str);
void vUtilsShiftLeftArray(int *array, int size);

#endif // __UTILS_H__