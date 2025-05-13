/*
 * Lab 4 starter code
 * ECE 266, Spring 2025
 *
 * lab4_main.c: Implement the tasks for a motion detection alarm.
 *
 * Created by Zhao Zhang
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "launchpad.h"
#include "buzzer.h"
#include "motion.h"

/********************************************************************************
 * Global declarations
 *******************************************************************************/

// The data structure for the system state
typedef struct {
    // Activated state: if the system is activated or not
    bool activated;

    // Alert state: If the alert is on or off
    bool alerted;

    // Sound state: If the buzzer is turned on to make a sound or not
    // Note: When the system is activated and alerted, the buzzer is turned
    // on and off periodically
    bool buzzer_on;
} SysState;

// The system state
static SysState sys =
{
    false /* not activated */,
    false /* not alerted */,
    false /* buzzer not turned on */
};

// The events
Event buzzer_play_event;
Event check_push_button_event;
Event check_motion_event;

/********************************************************************************
 * Task 1: If the system is activated and alerted, make the buzzer play
 * on/off sound. The buzzer will be repeatedly turned on and off.
 *
 * You may adjust the on and off times to change the sound pattern and intensity.
 *
 * Note: This is a callback function.
 *******************************************************************************/

void BuzzerPlay(Event *event)                   // the scheduled time
{
    uint32_t delay = 500;                       // default checking interval

    if (sys.alerted)
    {
        // Turn buzzer on and off
        if (sys.buzzer_on)
        {
            // The buzzer is on: turn it off
            BuzzerOff();
            sys.buzzer_on = false;
            delay = 2988;                       // off for 2988 ms
        }
        else
        {
            // The buzzer is off: turn it on
            BuzzerOn();
            sys.buzzer_on = true;
            delay = 12;                         // on for 12 ms
        }
    }
    else
    {
        // Make sure the buzzer is turned off
        if (sys.buzzer_on)
        {
            BuzzerOff();
            sys.buzzer_on = false;
        }
    }

    // schedule callback to buzzer play
    EventSchedule(event, event->time + delay);
}

/********************************************************************************
 * Task 2: YOUR CODE, check the motion sensor
 *
 * Check the motion sensor input. If motion is detected, put the system
 * in the alerted state. Otherwise, put it in the non-alerted state.
 *
 * This is a callback function. In the next lab, you will re-write this function
 * to make it interrupt based.
 *******************************************************************************/
void CheckMotionSensor(Event *event)
{
    if (!sys.activated) {
            // System is deactivated  Ensure LED & buzzer are off
            LedTurnOnOff(false, false, false);

            sys.alerted = false;
        } else {
            // System is activated  Check for motion
            if (MotionDetected()) {
                sys.alerted = true;

                uprintf("%s\n\r", "Motion detected");
            } else {
                sys.alerted = false;
                uprintf("%s\n\r", "No motion detected");
            }

            // LED & Buzzer control based on alert state
            if (sys.alerted) {
                LedTurnOnOff(true /* red */, false /* blue */, false /* green */);  // Red LED ON (motion detected)

            } else {
                LedTurnOnOff(false /* red */, false /* blue */, true /* green */);  // Green LED ON (no motion)

            }
        }

        // Schedule next motion check in 500ms
        EventSchedule(event, event->time + 500);
}

/********************************************************************************
 * Task 3: Check push buttons.
 *
 * In the starter code: Pushing SW1 activates the system and put it in the
 * alerted state. Pushing SW2 deactivates the system.
 *
 * IN YOUR SOLUTION CODE: Pushing SW1 activates the system. Pushing SW2
 * deactivates the system. If the system is activated, the motion sensor
 * input decides if the system is put in the alerted state or not.
 *
 * This function is called upon interrupts. It is NOT a callback function.
 *
 * YOU NEED TO REVISE THIS FUNCTION
 *
 *******************************************************************************/

void CheckPushButton(Event *event)
{
    int code = PushButtonRead();        // read the pushbutton
    switch (code)
    {
    case 1:                     // SW1 is pushed

        // Activate the system
        sys.activated = true;

        LedTurnOnOff(false /* red */, false /* blue */, true /* green */);
        uprintf("%s\n\r", "System Activated");
        break;


    case 2:                     // SW2 is pushed

        // De-activate the system and turn off LED
        sys.activated = false;
        sys.alerted = false;
        BuzzerOff();
        LedTurnOnOff(false /* red */, false /* blue */, false /* green */);
        uprintf("%s\n\r", "System Deactivated");
        break;
    }
}


/********************************************************************************
 * The main function: Print out a message, schedule the first callback event(s),
 * and then run the callback scheduler.
 *******************************************************************************/

int main(void)
{
    // Initialize the Tiva C LaunchPad and 7-seg
    LaunchPadInit();
    BuzzerInit();
    MotionInit();

    uprintf("%s\n\r", "Lab 4: Motion Detection Alarm");

    // Initialize the event objects
    EventInit(&buzzer_play_event, BuzzerPlay);
    EventInit(&check_push_button_event, CheckPushButton);
    EventInit(&check_motion_event, CheckMotionSensor); //makes motionsensor event

    // Register ISR event
    PushButtonEventRegister(&check_push_button_event);

    // Schedule time events
    EventSchedule(&buzzer_play_event, 100);
    EventSchedule(&check_motion_event, 100);

    // Run the callback scheduler
    while (true)
    {
        // Wait for interrupt events (include callback events)
        asm("   wfi");

        // Run callback events
        EventExecute();
    }
}
