#include "SDL.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "doomtype.h"
#include "d_event.h"
#include "i_joystick.h"
#include "i_system.h"

#include "m_config.h"
#include "m_misc.h"

// When an axis is within the dead zone, it is set to zero.
// This is 5% of the full range:

#define DEAD_ZONE (32768 / 3)

//static SDL_Joystick *joystick = NULL;

// Configuration variables:

// Standard default.cfg Joystick enable/disable

static int usejoystick = 0;

// Joystick to use, as an SDL joystick index:

static int joystick_index = -1;

// Which joystick axis to use for horizontal movement, and whether to
// invert the direction:

static int joystick_x_axis = 0;
static int joystick_x_invert = 0;

// Which joystick axis to use for vertical movement, and whether to
// invert the direction:

static int joystick_y_axis = 1;
static int joystick_y_invert = 0;

// Which joystick axis to use for strafing?

static int joystick_strafe_axis = -1;
static int joystick_strafe_invert = 0;

// Virtual to physical button joystick button mapping. By default this
// is a straight mapping.
static int joystick_physical_buttons[NUM_VIRTUAL_BUTTONS] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
};

void I_InitJoystick(void) {
	/*if (!usejoystick || joystick_index < 0)
	{
		return;
	}

	if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
	{
		return;
	}

	if (joystick_index >= SDL_NumJoysticks())
	{
		printf("I_InitJoystick: Invalid joystick ID: %i\n", joystick_index);
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
		return;
	}

	// Open the joystick

	joystick = SDL_JoystickOpen(joystick_index);

	if (joystick == NULL)
	{
		printf("I_InitJoystick: Failed to open joystick #%i\n",
			joystick_index);
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
		return;
	}

	if (!IsValidAxis(joystick_x_axis)
		|| !IsValidAxis(joystick_y_axis)
		|| !IsValidAxis(joystick_strafe_axis))
	{
		printf("I_InitJoystick: Invalid joystick axis for joystick #%i "
			"(run joystick setup again)\n",
			joystick_index);

		SDL_JoystickClose(joystick);
		joystick = NULL;
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	}

	SDL_JoystickEventState(SDL_ENABLE);

	// Initialized okay!

	printf("I_InitJoystick: %s\n", SDL_JoystickName(joystick_index));*/

	I_AtExit(I_ShutdownJoystick, true);
}
void I_ShutdownJoystick(void)
{
	/*if (joystick != NULL)
	{
		SDL_JoystickClose(joystick);
		joystick = NULL;
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	}*/
}

void I_BindJoystickVariables(void)
{
	/*int i;

	M_BindIntVariable("use_joystick", &usejoystick);
	M_BindIntVariable("joystick_index", &joystick_index);
	M_BindIntVariable("joystick_x_axis", &joystick_x_axis);
	M_BindIntVariable("joystick_y_axis", &joystick_y_axis);
	M_BindIntVariable("joystick_strafe_axis", &joystick_strafe_axis);
	M_BindIntVariable("joystick_x_invert", &joystick_x_invert);
	M_BindIntVariable("joystick_y_invert", &joystick_y_invert);
	M_BindIntVariable("joystick_strafe_invert", &joystick_strafe_invert);

	for (i = 0; i < NUM_VIRTUAL_BUTTONS; ++i)
	{
		char name[32];
		M_snprintf(name, sizeof(name), "joystick_physical_button%i", i);
		M_BindIntVariable(name, &joystick_physical_buttons[i]);
	}*/
}