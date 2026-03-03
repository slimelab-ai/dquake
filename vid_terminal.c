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
// vid_terminal.c -- Terminal video driver for dquake
// Renders Quake to terminal using truecolor ANSI escapes

#include "quakedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

// Constants from gl_vidsdl.c
#ifndef WARP_WIDTH
#define WARP_WIDTH		320
#endif
#ifndef WARP_HEIGHT
#define WARP_HEIGHT		200
#endif

// Terminal capabilities
static struct {
	qboolean initialized;
	qboolean has_truecolor;
	int cols;
	int rows;
	int cell_width;   // character cell width (for aspect)
	int cell_height;  // character cell height
	struct termios orig_termios;
} term_state;

// Framebuffer for terminal (downsampled from game)
static pixel_t *term_buffer = NULL;
static int term_buffer_size = 0;

// Forward declarations
static void Terminal_DetectCapabilities (void);
static void Terminal_SetRawMode (void);
static void Terminal_RestoreMode (void);
static void Terminal_InitBuffer (int width, int height);
static void Terminal_ClearScreen (void);
static void Terminal_ShowCursor (qboolean show);

/*
=================
VID_Init

Initialize the terminal video subsystem.
=================
*/
void VID_Init (void)
{
	Sys_Printf ("VID_Init: Initializing terminal video driver\n");

	// Detect terminal capabilities
	Terminal_DetectCapabilities ();

	// Get terminal dimensions
	struct winsize w;
	ioctl (STDOUT_FILENO, TIOCGWINSZ, &w);
	term_state.cols = w.ws_col ? w.ws_col : 80;
	term_state.rows = w.ws_row ? w.ws_row : 24;

	// Assume character cells are 2:1 height:width ratio for aspect
	term_state.cell_width = 1;
	term_state.cell_height = 2;

	Sys_Printf ("VID_Init: Terminal size %dx%d\n", term_state.cols, term_state.rows);

	// Set raw mode for input (disable echo, line buffering)
	Terminal_SetRawMode ();

	// Clear screen and hide cursor
	Terminal_ClearScreen ();
	Terminal_ShowCursor (false);

	// Initialize video state
	memset (&vid, 0, sizeof(vid));
	vid.width = term_state.cols;
	vid.height = term_state.rows;
	vid.maxwarpwidth = WARP_WIDTH;
	vid.maxwarpheight = WARP_HEIGHT;
	vid.numpages = 1;
	vid.rowbytes = vid.width;
	vid.recalc_refdef = 1;

	// Allocate initial buffer (will be resized by game)
	Terminal_InitBuffer (vid.width, vid.height);

	// Allocate colormap
	// TODO: Build proper colormap for terminal color mapping
	vid.colormap = NULL;  // Software renderer would provide this
	vid.fullbright = 0;

	term_state.initialized = true;
	modestate = MS_WINDOWED;  // Treat terminal as "windowed"
}

/*
=================
VID_Shutdown

Clean up terminal video.
=================
*/
void VID_Shutdown (void)
{
	if (!term_state.initialized)
		return;

	Sys_Printf ("VID_Shutdown: Cleaning up terminal video\n");

	// Show cursor and reset attributes
	Terminal_ShowCursor (true);
	printf ("\x1b[0m");  // Reset all attributes
	Terminal_ClearScreen ();

	// Restore original terminal mode
	Terminal_RestoreMode ();

	// Free buffer
	if (term_buffer)
	{
		free (term_buffer);
		term_buffer = NULL;
	}

	term_state.initialized = false;
}

/*
=================
VID_Update

Flush the framebuffer to the terminal.
This is where we convert pixel data to ANSI escape sequences.
=================
*/
void VID_Update (vrect_t *rects)
{
	// NOTE: This is a scaffold implementation.
	// The full implementation will:
	// 1. Get the rendered framebuffer from the software renderer
	// 2. Downsample to terminal dimensions
	// 3. Map pixels to characters and colors
	// 4. Output truecolor ANSI escape sequences

	// Placeholder: output a simple status line
	// Move cursor to bottom of screen
	printf ("\x1b[%d;1H", term_state.rows);
	printf ("\x1b[0m");  // Reset
	printf("DQuake Terminal Mode - Frame Update");
	fflush (stdout);

	// For now, this is a no-op render
	// The game logic runs but no visual output yet
}

/*
=================
VID_SyncCvars

Sync video cvars (not needed for terminal).
=================
*/
void VID_SyncCvars (void)
{
	// No SDL cvars to sync for terminal mode
}

/*
=================
VID_Toggle

Toggle fullscreen/windowed (no-op for terminal).
=================
*/
void VID_Toggle (void)
{
	// Terminal doesn't have fullscreen/windowed modes
}

/*
=================
VID_GetWindow

Return window handle (NULL for terminal).
=================
*/
void *VID_GetWindow (void)
{
	return NULL;  // No window in terminal mode
}

/*
=================
VID_HasMouseOrInputFocus

Always returns true for terminal (we have focus).
=================
*/
qboolean VID_HasMouseOrInputFocus (void)
{
	return true;  // Terminal always has input focus when running
}

/*
=================
VID_IsMinimized

Always returns false for terminal.
=================
*/
qboolean VID_IsMinimized (void)
{
	return false;  // Terminal can't be "minimized"
}

/*
=================
VID_Lock

Lock video subsystem (no-op for terminal).
=================
*/
void VID_Lock (void)
{
	// Not needed for terminal
}

// ========================================================================
// Terminal helper functions
// ========================================================================

/*
=================
Terminal_DetectCapabilities

Detect terminal features (truecolor, etc.)
=================
*/
static void Terminal_DetectCapabilities (void)
{
	const char *term = getenv ("TERM");
	const char *colorterm = getenv ("COLORTERM");

	term_state.has_truecolor = false;

	// Check for truecolor support
	if (colorterm && (strstr (colorterm, "truecolor") || strstr (colorterm, "24bit")))
	{
		term_state.has_truecolor = true;
	}
	else if (term && (strstr (term, "xterm-256color") ||
			  strstr (term, "screen-256color") ||
			  strstr (term, "tmux-256color")))
	{
		// 256-color terminals often support truecolor as well
		// Modern terminals (kitty, alacritty, gnome-terminal) do
		term_state.has_truecolor = true;  // Optimistic assumption
	}

	// Check for Kitty specifically (has full graphics support)
	if (term && strcmp (term, "xterm-kitty") == 0)
	{
		term_state.has_truecolor = true;
	}

	Sys_Printf ("Terminal: TERM=%s COLORTERM=%s truecolor=%s\n",
		term ? term : "(none)",
		colorterm ? colorterm : "(none)",
		term_state.has_truecolor ? "yes" : "no");
}

/*
=================
Terminal_SetRawMode

Set terminal to raw mode for immediate input.
=================
*/
static void Terminal_SetRawMode (void)
{
	struct termios new_termios;

	// Save original settings
	tcgetattr (STDIN_FILENO, &term_state.orig_termios);

	// Set raw mode
	memcpy (&new_termios, &term_state.orig_termios, sizeof (struct termios));
	new_termios.c_lflag &= ~(ICANON | ECHO | ISIG);  // Disable canonical mode, echo, signals
	new_termios.c_iflag &= ~(IXON | ICRNL);           // Disable XON/XOFF, CR/NL translation
	new_termios.c_oflag &= ~OPOST;                     // Disable output processing
	new_termios.c_cc[VMIN] = 0;                         // Non-blocking read
	new_termios.c_cc[VTIME] = 0;

	tcsetattr (STDIN_FILENO, TCSANOW, &new_termios);
}

/*
=================
Terminal_RestoreMode

Restore original terminal mode.
=================
*/
static void Terminal_RestoreMode (void)
{
	tcsetattr (STDIN_FILENO, TCSANOW, &term_state.orig_termios);
}

/*
=================
Terminal_InitBuffer

Allocate the terminal render buffer.
=================
*/
static void Terminal_InitBuffer (int width, int height)
{
	term_buffer_size = width * height;
	term_buffer = (pixel_t *) malloc (term_buffer_size);
	if (!term_buffer)
		Sys_Error ("VID_Init: Failed to allocate terminal buffer");

	memset (term_buffer, 0, term_buffer_size);
}

/*
=================
Terminal_ClearScreen

Clear terminal screen.
=================
*/
static void Terminal_ClearScreen (void)
{
	printf ("\x1b[2J");   // Clear screen
	printf ("\x1b[H");    // Move cursor to home
	fflush (stdout);
}

/*
=================
Terminal_ShowCursor

Show or hide terminal cursor.
=================
*/
static void Terminal_ShowCursor (qboolean show)
{
	printf ("\x1b[?25%c", show ? 'h' : 'l');
	fflush (stdout);
}

// ========================================================================
// Menu callbacks (stub implementations)
// ========================================================================

void (*vid_menudrawfn)(void) = NULL;
void (*vid_menukeyfn)(int key) = NULL;
void (*vid_menucmdfn)(void) = NULL;

/*
=================
Terminal_GetCols

Get terminal width in columns.
=================
*/
int Terminal_GetCols (void)
{
	return term_state.cols;
}

/*
=================
Terminal_GetRows

Get terminal height in rows.
=================
*/
int Terminal_GetRows (void)
{
	return term_state.rows;
}

/*
=================
Terminal_HasTruecolor

Check if terminal supports 24-bit color.
=================
*/
qboolean Terminal_HasTruecolor (void)
{
	return term_state.has_truecolor;
}

/*
=================
Terminal_OutputTruecolor

Output a truecolor character at position.
Using ANSI 24-bit color escape sequences.
=================
*/
void Terminal_OutputTruecolor (int x, int y, char ch, byte r, byte g, byte b)
{
	// Position cursor
	printf ("\x1b[%d;%dH", y + 1, x + 1);  // 1-indexed

	// Set foreground color and output character
	printf ("\x1b[38;2;%d;%d;%dm%c", r, g, b, ch);

	// Note: caller should batch these and reset with \x1b[0m when done
}

/*
=================
Terminal_OutputTruecolorBG

Output a truecolor cell with foreground and background colors.
=================
*/
void Terminal_OutputTruecolorBG (int x, int y, char ch, byte fg_r, byte fg_g, byte fg_b, byte bg_r, byte bg_g, byte bg_b)
{
	printf ("\x1b[%d;%dH", y + 1, x + 1);
	printf ("\x1b[38;2;%d;%d;%d;48;2;%d;%d;%dm%c",
		fg_r, fg_g, fg_b,
		bg_r, bg_g, bg_b,
		ch);
}