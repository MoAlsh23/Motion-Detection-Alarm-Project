/*
 * motion.c
 *
 *  Created on: Feb 25, 2025
 *      Author: moham
 */

#include "motion.h"


void MotionInit(){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4); //port pc4 instead of pc5


}
