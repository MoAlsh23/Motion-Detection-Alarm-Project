/*
 * motion.h
 *
 *  Created on: Feb 25, 2025
 *      Author: moham
 */

#ifndef MOTION_H_
#define MOTION_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>

void MotionInit();  // motion sensor initialize

bool MotionDetected();  //motion detection func



#endif /* MOTION_H_ */
