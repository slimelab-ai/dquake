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
// main_terminal.c -- Main entry point for terminal (no-SDL) build

#include "quakedef.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#define DEFAULT_MEMORY (256 * 1024 * 1024)

static quakeparms_t	parms;

// Signal handler for Ctrl+C
static volatile sig_atomic_t shutdown_requested = 0;

static void handle_signal (int sig)
{
	shutdown_requested = 1;
}

int main (int argc, char *argv[])
{
	int		t;
	double		time, oldtime, newtime;

	host_parms = &parms;
	parms.basedir = ".";

	parms.argc = argc;
	parms.argv = argv;

	parms.errstate = 0;

	COM_InitArgv (parms.argc, parms.argv);

	// No dedicated mode for terminal - we're always "windowed"
	isDedicated = false;

	Sys_Init ();

	parms.memsize = DEFAULT_MEMORY;
	if (COM_CheckParm ("-heapsize"))
	{
		t = COM_CheckParm ("-heapsize") + 1;
		if (t < com_argc)
			parms.memsize = Q_atoi (com_argv[t]) * 1024;
	}

	parms.membase = malloc (parms.memsize);

	if (!parms.membase)
		Sys_Error ("Not enough memory free; check disk space\n");

	Sys_Printf ("DQuake - Quake in Terminal\n");
	Sys_Printf ("Based on QuakeSpasm " QUAKESPASM_VER_STRING "\n");
	Sys_Printf ("Terminal video driver initialized\n");

	// Set up signal handlers
	signal (SIGINT, handle_signal);
	signal (SIGTERM, handle_signal);

	Sys_Printf ("Host_Init\n");
	Host_Init ();

	oldtime = Sys_DoubleTime ();

	// Main game loop
	while (!shutdown_requested)
	{
		newtime = Sys_DoubleTime ();
		time = newtime - oldtime;

		// Read terminal input
		IN_SendKeyEvents ();

		// Run game frame
		Host_Frame (time);

		// Throttle if running too fast
		if (time < sys_throttle.value)
			usleep (1000);  // 1ms

		oldtime = newtime;
	}

	Sys_Printf ("\nShutting down...\n");
	Sys_Quit ();

	return 0;
}