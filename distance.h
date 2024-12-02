#ifndef DISTANCE_H
#define DISTANCE_H

#include "stm32f4xx.h"

// Define Trigger and Echo pins
#define TRIG_PIN    (1 << 6)  // Trig at pin PA6
#define ECHO_PIN    (1 << 7)  // Echo at pin PA7

// Function to measure the distance using HC-SR04
uint32_t HC_SR04_Distance(void);

#endif 