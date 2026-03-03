/*
Copyright (C) 2025 DQuake contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// in_terminal.c -- Terminal input driver for dquake
// Handles keyboard input from terminal using raw mode and ANSI escape sequences

#include "quakedef.h"
#include "input.h"
#include "keys.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

// Escape sequence state machine
#define SEQ_MAXLEN 16
typedef enum {
	SEQ_NONE,       // Normal input
	SEQ_ESC,        // Received ESC
	SEQ_CSI,        // ESC [ - control sequence
	SEQ_SS3,        // ESC O - function key
	SEQ_COMPLETE    // Have complete sequence
} seq_state_t;

static struct {
	seq_state_t state;
	char buffer[SEQ_MAXLEN];
	int len;
	int key;  // Parsed key code
} seq;

// Key mapping from terminal sequences to Quake keycodes
static struct {
	const char *sequence;
	int keycode;
} key_sequences[] = {
	// Arrow keys
	{"\x1b[A", K_UPARROW},
	{"\x1b[B", K_DOWNARROW},
	{"\x1b[C", K_RIGHTARROW},
	{"\x1b[D", K_LEFTARROW},

	// Function keys (VT100 style)
	{"\x1bOP", K_F1},
	{"\x1bOQ", K_F2},
	{"\x1bOR", K_F3},
	{"\x1bOS", K_F4},
	{"\x1b[15~", K_F5},
	{"\x1b[17~", K_F6},
	{"\x1b[18~", K_F7},
	{"\x1b[19~", K_F8},
	{"\x1b[20~", K_F9},
	{"\x1b[21~", K_F10},
	{"\x1b[23~", K_F11},
	{"\x1b[24~", K_F12},

	// Navigation keys
	{"\x1b[1~", K_HOME},
	{"\x1b[4~", K_END},
	{"\x1b[5~", K_PGUP},
	{"\x1b[6~", K_PGDN},

	// Insert/Delete
	{"\x1b[2~", K_INS},
	{"\x1b[3~", K_DEL},

	{NULL, 0}  // Sentinel
};

// Map printable ASCII to Quake keys
static int ascii_to_key[] = {
	// Standard ASCII mappings (32-126)
	[' '] = ' ',
	['!'] = '!',
	['"'] = '"',
	['#'] = '#',
	['$'] = '$',
	['%'] = '%',
	['&'] = '&',
	['\''] = '\'',
	['('] = '(',
	[')'] = ')',
	['*'] = '*',
	['+'] = '+',
	[','] = ',',
	['-'] = '-',
	['.'] = '.',
	['/'] = '/',
	['0'] = '0', ['1'] = '1', ['2'] = '2', ['3'] = '3', ['4'] = '4',
	['5'] = '5', ['6'] = '6', ['7'] = '7', ['8'] = '8', ['9'] = '9',
	[':'] = ':',
	[';'] = ';',
	['<'] = '<',
	['='] = '=',
	['>'] = '>',
	['?'] = '?',
	['@'] = '@',
	['A'] = 'A', ['B'] = 'B', ['C'] = 'C', ['D'] = 'D', ['E'] = 'E',
	['F'] = 'F', ['G'] = 'G', ['H'] = 'H', ['I'] = 'I', ['J'] = 'J',
	['K'] = 'K', ['L'] = 'L', ['M'] = 'M', ['N'] = 'N', ['O'] = 'O',
	['P'] = 'P', ['Q'] = 'Q', ['R'] = 'R', ['S'] = 'S', ['T'] = 'T',
	['U'] = 'U', ['V'] = 'V', ['W'] = 'W', ['X'] = 'X', ['Y'] = 'Y',
	['Z'] = 'Z',
	['['] = '[',
	['\\'] = '\\',
	[']'] = ']',
	['^'] = '^',
	['_'] = '_',
	['`'] = '`',
	['a'] = 'a', ['b'] = 'b', ['c'] = 'c', ['d'] = 'd', ['e'] = 'e',
	['f'] = 'f', ['g'] = 'g', ['h'] = 'h', ['i'] = 'i', ['j'] = 'j',
	['k'] = 'k', ['l'] = 'l', ['m'] = 'm', ['n'] = 'n', ['o'] = 'o',
	['p'] = 'p', ['q'] = 'q', ['r'] = 'r', ['s'] = 's', ['t'] = 't',
	['u'] = 'u', ['v'] = 'v', ['w'] = 'w', ['x'] = 'x', ['y'] = 'y',
	['z'] = 'z',
	['{'] = '{',
	['|'] = '|',
	['}'] = '}',
	['~'] = '~',
};

// Mouse state (for SGR mouse mode - future extension)
static struct {
	qboolean enabled;
	int x, y;
	int buttons;
} mouse;

/*
=================
IN_Init

Initialize terminal input.
=================
*/
void IN_Init (void)
{
	Sys_Printf ("IN_Init: Initializing terminal input\n");
	memset (&seq, 0, sizeof (seq));
	memset (&mouse, 0, sizeof (mouse));
}

/*
=================
IN_Shutdown

Clean up terminal input.
=================
*/
void IN_Shutdown (void)
{
	// Nothing to clean up - terminal mode handled by vid_terminal.c
}

/*
=================
IN_Commands

Process input commands.
Called each frame to poll input.
=================
*/
void IN_Commands (void)
{
	// This is where we'd handle joystick/hat events if we had them
	// For terminal input, Key_Event is called directly from IN_Read
}

/*
=================
IN_Move

Process mouse movement.
For terminal mode, only useful if SGR mouse mode is enabled.
=================
*/
void IN_Move (usercmd_t *cmd)
{
	// Mouse input not yet implemented for terminal
	// Future: parse SGR mouse sequences and update cmd->viewangles
}

/*
=================
ParseEscapeSequence

Parse accumulated escape sequence and return keycode.
Returns 0 if sequence not recognized or not complete.
=================
*/
static int ParseEscapeSequence (void)
{
	int i;

	// Check if we have a CSI sequence (ESC [)
	if (seq.buffer[1] == '[')
	{
		// Find matching sequence
		for (i = 0; key_sequences[i].sequence != NULL; i++)
		{
			if (strcmp (seq.buffer, key_sequences[i].sequence) == 0)
				return key_sequences[i].keycode;
		}

		// Unknown CSI sequence - ignore
		Sys_Printf ("IN: Unknown CSI sequence: %s\n", seq.buffer);
		return 0;
	}
	else if (seq.buffer[1] == 'O')
	{
		// SS3 sequence (function keys F1-F4)
		for (i = 0; key_sequences[i].sequence != NULL; i++)
		{
			if (strcmp (seq.buffer, key_sequences[i].sequence) == 0)
				return key_sequences[i].keycode;
		}

		Sys_Printf ("IN: Unknown SS3 sequence: %s\n", seq.buffer);
		return 0;
	}

	// Unknown escape sequence
	return 0;
}

/*
=================
IN_Read

Read and process input from stdin.
Called each frame by the main loop.
=================
*/
void IN_Read (void)
{
	char c;
	int fd = STDIN_FILENO;
	int bytes_read;
	int keycode;

	// Read all available input
	while (1)
	{
		bytes_read = read (fd, &c, 1);
		if (bytes_read <= 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;  // No more input
			return;  // Error or EOF
		}

		// Handle by state
		switch (seq.state)
		{
		case SEQ_NONE:
			if (c == 27)  // ESC
			{
				seq.state = SEQ_ESC;
				seq.len = 0;
				seq.buffer[seq.len++] = c;
			}
			else if (c >= 32 && c < 127)
			{
				// Printable ASCII - direct key event
				keycode = ascii_to_key[(int)c];
				if (keycode)
					Key_Event (keycode, true);
				// Key release is tricky in terminal mode - we may want to track held keys
			}
			else if (c == 10 || c == 13)  // Enter/Return
			{
				Key_Event (K_ENTER, true);
			}
			else if (c == 8 || c == 127)  // Backspace (may vary by terminal)
			{
				Key_Event (K_BACKSPACE, true);
			}
			else if (c == 9)  // Tab
			{
				Key_Event (K_TAB, true);
			}
			else if (c == 3)  // Ctrl+C
			{
				// Treat as ESC for menu/cancel
				Key_Event (K_ESCAPE, true);
			}
			break;

		case SEQ_ESC:
			// Got ESC, waiting for next character
			seq.buffer[seq.len++] = c;
			if (c == '[')
			{
				seq.state = SEQ_CSI;
			}
			else if (c == 'O')
			{
				seq.state = SEQ_SS3;
			}
			else
			{
				// Standalone ESC or unknown sequence
				Key_Event (K_ESCAPE, true);
				seq.state = SEQ_NONE;
				seq.len = 0;
			}
			break;

		case SEQ_CSI:
		case SEQ_SS3:
			// Accumulating control sequence
			seq.buffer[seq.len++] = c;
			seq.buffer[seq.len] = '\0';

			// Check if sequence is complete (letter A-Z or ~)
			if ((c >= 'A' && c <= 'Z') || c == '~')
			{
				keycode = ParseEscapeSequence ();
				if (keycode)
					Key_Event (keycode, true);
				seq.state = SEQ_NONE;
				seq.len = 0;
			}
			else if (seq.len >= SEQ_MAXLEN - 1)
			{
				// Sequence too long, abort
				Sys_Printf ("IN: Escape sequence too long\n");
				seq.state = SEQ_NONE;
				seq.len = 0;
			}
			break;

		default:
			// Invalid state, reset
			seq.state = SEQ_NONE;
			seq.len = 0;
			break;
		}
	}
}

/*
=================
IN_StartupMouse

Initialize mouse (no-op for terminal).
=================
*/
void IN_StartupMouse (void)
{
	// Terminal doesn't have native mouse support in basic mode
	// SGR mouse mode could be enabled in future versions
	mouse.enabled = false;
}

/*
=================
IN_MouseEvent

Handle mouse event (no-op for basic terminal).
=================
*/
void IN_MouseEvent (int mstate)
{
	// Would handle mouse buttons if SGR mode enabled
}

/*
=================
IN_ClearStates

Clear key/mouse states.
=================
*/
void IN_ClearStates (void)
{
	// Would clear any held key states
	memset (&seq, 0, sizeof (seq));
}

/*
=================
IN_AckFrame

Acknowledge frame (for input timing).
=================
*/
void IN_AckFrame (void)
{
	// Nothing needed for terminal input
}

/*
=================
IN_SetCursor

Set cursor (no-op for terminal).
=================
*/
void IN_SetCursor (qboolean visible)
{
	// Terminal cursor handled by vid_terminal.c
}

/*
=================
IN_MouseMotion

Handle mouse motion event (no-op for basic terminal).
=================
*/
void IN_MouseMotion (int dx, int dy)
{
	// Mouse motion not supported in basic terminal mode
	// Future: could use SGR mouse mode for relative motion
}

/*
=================
IN_SendKeyEvents

Send key events - main input polling function for terminal.
Called from Host_Frame in the main loop.
=================
*/
void IN_SendKeyEvents (void)
{
	IN_Read ();
}

/*
=================
IN_UpdateInputMode

Update input mode (text/non-text) for terminal.
=================
*/
void IN_UpdateInputMode (void)
{
	// Terminal always uses the same raw input mode
	// No need to switch between text/mode like SDL
}

/*
=================
IN_Activate

Called when the app becomes active.
=================
*/
void IN_Activate (void)
{
	// Terminal is always active when running
}

/*
=================
IN_Deactivate

Called when the app becomes inactive.
=================
*/
void IN_Deactivate (qboolean free_cursor)
{
	// Terminal is always active, nothing to deactivate
}