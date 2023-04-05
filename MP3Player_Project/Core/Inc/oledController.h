#ifndef _OLED_CONTROLLER_H
#define _OLED_CONTROLLER_H
#include "ssd1306.h"
#include "controlType.h"

void opening();
void printDefault();
void printTemper(int temper);
void toggleScreen();
void startToggle();
void printHeaterState(ON_OFF_t onOff);
void printBackground();
#endif
