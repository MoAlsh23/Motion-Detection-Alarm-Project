

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
