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
// pl_terminal.c -- Platform functions for terminal mode
// No SDL dependency

#include "quakedef.h"

void PL_SetWindowIcon (void)
{
	// No window in terminal mode
}

void PL_VID_Shutdown (void)
{
	// No video system to shutdown in terminal mode
}

char *PL_GetClipboardData (void)
{
	// Terminal mode doesn't have clipboard access
	return NULL;
}

void PL_ErrorDialog (const char *errorMsg)
{
	// In terminal mode, just print to stderr
	fprintf(stderr, "ERROR: %s\n", errorMsg);
}