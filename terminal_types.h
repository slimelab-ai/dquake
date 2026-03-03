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
// terminal_types.h -- Type definitions for terminal mode
// Provides minimal type stubs needed when not using OpenGL

#ifndef TERMINAL_TYPES_H
#define TERMINAL_TYPES_H

// Hull types from gl_model.h (software rendering would need full implementation)
typedef struct hull_s {
	struct mclipnode_s *clipnodes;
	struct mplane_s *planes;
	int firstclipnode;
	int lastclipnode;
	vec3_t clip_mins;
	vec3_t clip_maxs;
	int clip_nodes_count;
} hull_t;

// Texture modes (stub for terminal rendering)
#define	GL_NEAREST		0x2600
#define	GL_LINEAR		0x2601
#define	GL_NEAREST_MIPMAP_NEAREST	0x2700
#define	GL_LINEAR_MIPMAP_NEAREST	0x2701
#define	GL_NEAREST_MIPMAP_LINEAR	0x2702
#define	GL_LINEAR_MIPMAP_LINEAR	0x2703

// Additional types that may be needed
typedef byte pixel_t;

// Model types (minimal stubs)
typedef enum {
	MOD_BAD,
	MOD_BRUSH,
	MOD_ALIAS,
	MOD_SPRITE
} modtype_t;

// Render texture handle (stub)
typedef int texture_t;

// Surface flags (minimal)
#define	SURF_PLANEBACK		1
#define	SURF_DRAWSKY		2
#define	SURF_DRAWTURB		4
#define	SURF_DRAWTILED		8

#endif // TERMINAL_TYPES_H