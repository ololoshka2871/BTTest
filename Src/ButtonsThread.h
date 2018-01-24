#ifndef BUTTONS_H_
#define BUTTONS_H_

// Forward declaration
typedef uint32_t TickType_t;

/// ButtonIDs
enum ButtonID
{
    NO_BUTTON = 0,
    LEFT_BUTTON = 1,
    RIGHT_BUTTON = 2,
    SEL_BUTTON = 3,
    RETURN_BUTTON = 4,
    POWER_BUTTON = 5,
};

/// Button event happened
enum ButtonEvent
{
	BUTTON_CLICK,
	BUTTON_LONG_PRESS,
	BUTTON_VERY_LONG_PRESS
};

/// Message structure that is passed from buttons thread to UI task
struct ButtonMessage
{
	ButtonID button;
	ButtonEvent event;
};

// Initialize buttons, ports, queue and related stuff
void initButtons();

// Buttons are being monitored in a dedicated task.
// When button click or long press is detected corresponding message is sent to UI task
void vButtonsThread(void *pvParameters);

// Wait until a button is pressed
bool waitForButtonMessage(ButtonMessage * msg, TickType_t xTicksToWait);

#endif /* BUTTONS_H_ */
