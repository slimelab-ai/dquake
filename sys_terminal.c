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
// sys_terminal.c -- System functions for terminal (no-SDL) build

#include "quakedef.h"

#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

qboolean		isDedicated = false;
cvar_t		sys_throttle = {"sys_throttle", "0.02", CVAR_ARCHIVE};

#define	MAX_HANDLES		32
static FILE		*sys_handles[MAX_HANDLES];

// Terminal signal handling
static volatile sig_atomic_t want_quit = 0;

static void signal_handler(int sig)
{
	want_quit = 1;
}

// ========================================
// File I/O (from sys_sdl_unix.c)
// ========================================

static int findhandle (void)
{
	int i;
	for (i = 1; i < MAX_HANDLES; i++)
	{
		if (!sys_handles[i])
			return i;
	}
	Sys_Error ("out of handles");
	return -1;
}

long Sys_filelength (FILE *f)
{
	long	pos, end;
	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);
	return end;
}

int Sys_FileOpenRead (const char *path, int *hndl)
{
	FILE	*f;
	int	i;

	i = findhandle ();
	f = fopen(path, "rb");
	if (!f)
	{
		*hndl = -1;
		return -1;
	}
	sys_handles[i] = f;
	*hndl = i;
	return Sys_filelength(f);
}

int Sys_FileOpenWrite (const char *path)
{
	FILE	*f;
	int	i;

	i = findhandle ();
	f = fopen(path, "wb");
	if (!f)
		Sys_Error ("Error opening %s: %s", path, strerror(errno));
	sys_handles[i] = f;
	return i;
}

void Sys_FileClose (int handle)
{
	if (handle >= 0 && handle < MAX_HANDLES && sys_handles[handle])
	{
		fclose (sys_handles[handle]);
		sys_handles[handle] = NULL;
	}
}

void Sys_FileSeek (int handle, int position)
{
	if (handle >= 0 && handle < MAX_HANDLES && sys_handles[handle])
		fseek (sys_handles[handle], position, SEEK_SET);
}

int Sys_FileRead (int handle, void *dest, int count)
{
	if (handle >= 0 && handle < MAX_HANDLES && sys_handles[handle])
		return fread (dest, 1, count, sys_handles[handle]);
	return -1;
}

int Sys_FileWrite (int handle, const void *data, int count)
{
	if (handle >= 0 && handle < MAX_HANDLES && sys_handles[handle])
		return fwrite (data, 1, count, sys_handles[handle]);
	return -1;
}

int	Sys_FileTime (const char *path)
{
	FILE	*f;
	f = fopen(path, "rb");
	if (f)
	{
		fclose(f);
		return 1;
	}
	return -1;
}

void Sys_mkdir (const char *path)
{
	mkdir (path, 0755);
}

// ========================================
// System functions
// ========================================

void Sys_Init (void)
{
	// Set up signal handlers for clean exit
	signal (SIGINT, signal_handler);
	signal (SIGTERM, signal_handler);

	Sys_Printf ("Sys_Init: Terminal mode initialized\n");
}

void Sys_Quit (void)
{
	Host_Shutdown();
	exit (0);
}

void Sys_Error (const char *error, ...)
{
	va_list	argptr;
	char	string[1024];

	va_start (argptr, error);
	vsprintf (string, error, argptr);
	va_end (argptr);

	fprintf (stderr, "\nFATAL ERROR: %s\n", string);
	fprintf (stderr, "\n");

	Host_Shutdown ();
	exit (1);
}

void Sys_Printf (const char *fmt, ...)
{
	va_list	argptr;
	char	string[4096];

	va_start (argptr, fmt);
	vsprintf (string, fmt, argptr);
	va_end (argptr);

	printf ("%s", string);
}

double Sys_DoubleTime (void)
{
	struct timeval	tv;
	gettimeofday (&tv, NULL);
	return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

char *Sys_GetClipboardData (void)
{
	// Terminal mode doesn't have clipboard support
	return NULL;
}

void Sys_Sleep (unsigned long ms)
{
	usleep (ms * 1000);
}

// ========================================
// Terminal-specific functions
// ========================================

/*
=================
Sys_TerminalResize

Handle terminal resize signal (SIGWINCH).
=================
*/
void Sys_TerminalResize (void)
{
	// Would notify video subsystem of size change
	// VID_Update needs to handle new dimensions
}

/*
=================
Sys_SetupTerm

Initialize terminal for raw input and output.
=================
*/
void Sys_SetupTerm (void)
{
	// Initial setup - video driver handles terminal modes
	Sys_Printf ("Terminal mode: raw I/O enabled\n");
}