/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
Copyright (C) 2010-2014 QuakeSpasm developers
Copyright (C) 2025 DQuake contributors (terminal adaptations)

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
// swquake.h -- Software/Terminal rendering definitions
// Derived from glquake.h, GL-specific parts removed/adapted

#ifndef __SWQUAKE_H
#define __SWQUAKE_H

// Software rendering functions
void SW_BeginRendering (int *x, int *y, int *width, int *height);
void SW_EndRendering (void);
void SW_Set2D (void);

// GL-compatible globals (for code that references them)
extern	int glx, gly, glwidth, glheight;
extern	int sw_x, sw_y, sw_width, sw_height;

// Software texture handle (invalid marker)
#define	SW_UNUSED_TEXTURE		((unsigned int)-1)

// Model effect flags (from gl_model.h)
#define	EF_ROCKET		1			// leave a trail
#define	EF_GRENADE		2			// leave a trail
#define	EF_GIB			4			// leave a trail
#define	EF_ROTATE		8			// rotate (bonus items)
#define	EF_TRACER		16			// green split trail
#define	EF_ZOMGIB		32			// small blood trail
#define	EF_TRACER2		64			// orange split trail + rotate
#define	EF_TRACER3		128			// purple trail

// Private refresh defs

#define ALIAS_BASE_SIZE_RATIO		(1.0 / 11.0)
#define	MAX_LBM_HEIGHT		480

#define TILE_SIZE		128

#define SKYSHIFT		7
#define	SKYSIZE			(1 << SKYSHIFT)
#define SKYMASK			(SKYSIZE - 1)

#define BACKFACE_EPSILON	0.01

void R_TimeRefresh_f (void);
void R_ReadPointFile_f (void);
texture_t *R_TextureAnimation (texture_t *base, int frame);

typedef struct surfcache_s
{
	struct surfcache_s	*next;
	struct surfcache_s 	**owner;
	int			lightadj[MAXLIGHTMAPS];
	int			dlight;
	int			size;
	unsigned		width;
	unsigned		height;
	float			mipscale;
	struct texture_s	*texture;
	byte			data[4];
} surfcache_t;

typedef struct
{
	pixel_t		*surfdat;
	int		rowbytes;
	msurface_t	*surf;
	fixed8_t	lightadj[MAXLIGHTMAPS];
	texture_t	*texture;
	int		surfmip;
	int		surfwidth;
	int		surfheight;
} drawsurf_t;

typedef enum {
	pt_static, pt_grav, pt_slowgrav, pt_fire, pt_explode, pt_explode2, pt_blob, pt_blob2
} ptype_t;

typedef struct particle_s
{
	vec3_t		org;
	float		color;
	struct particle_s	*next;
	vec3_t		vel;
	float		ramp;
	float		die;
	ptype_t		type;
} particle_t;

//====================================================

extern	qboolean	r_cache_thrash;
extern	vec3_t		modelorg, r_entorigin;
extern	entity_t	*currententity;
extern	int		r_visframecount;
extern	int		r_framecount;
extern	mplane_t	frustum[4];

// view origin
extern	vec3_t	vup;
extern	vec3_t	vpn;
extern	vec3_t	vright;
extern	vec3_t	r_origin;

// screen size info
extern	refdef_t	r_refdef;
extern	mleaf_t		*r_viewleaf, *r_oldviewleaf;
extern	int		d_lightstylevalue[256];

extern	cvar_t	r_norefresh;
extern	cvar_t	r_drawentities;
extern	cvar_t	r_drawworld;
extern	cvar_t	r_drawviewmodel;
extern	cvar_t	r_speeds;
extern	cvar_t	r_pos;
extern	cvar_t	r_waterwarp;
extern	cvar_t	r_fullbright;
extern	cvar_t	r_lightmap;
extern	cvar_t	r_shadows;
extern	cvar_t	r_wateralpha;
extern	cvar_t	r_lavaalpha;
extern	cvar_t	r_telealpha;
extern	cvar_t	r_slimealpha;
extern	cvar_t	r_dynamic;
extern	cvar_t	r_novis;
extern	cvar_t	r_scale;

// Software renderer cvars
extern	cvar_t	sw_clear;
extern	cvar_t	sw_cull;
extern	cvar_t	sw_polyblend;
extern	cvar_t	sw_flashblend;
extern	cvar_t	sw_playermip;
extern	cvar_t	sw_nocolors;

// Developer stats tracking
extern	cvar_t	devstats;
typedef struct {
	int		packetsize;
	int		edicts;
	int		visedicts;
	int		efrags;
	int		tempents;
	int		beams;
	int		dlights;
} devstats_t;
extern	devstats_t dev_stats, dev_peakstats;

// GL compatibility cvars (stubs for terminal mode)
extern	cvar_t	gl_clear;
extern	qboolean	gl_glsl_gamma_able; // Not a cvar, used as boolean
extern	cvar_t	gl_polyblend;
extern	cvar_t	gl_flashblend;
extern	cvar_t	gl_cull;
extern	cvar_t	vid_gamma;
extern	cvar_t	vid_contrast;
extern	cvar_t	scr_sbaralpha;

// GL helper functions (stubs)
void GL_DisableMultitexture (void);
void GL_EnableMultitexture (void);
void GL_SelectTexture (int target);

// Canvas function (defined in draw.h)
// void GL_SetCanvas (canvastype newcanvas);

// Render view
void R_RenderView (void);

// Lerp cvars
extern	cvar_t r_lerpmodels;
extern	cvar_t r_lerpmove;

// Clear notify
extern	int clearnotify;

// BGM functions
void BGM_Init (void);
void BGM_Shutdown (void);
void BGM_PlayCDtrack (int track, qboolean looping);
void BGM_Stop (void);
void BGM_Pause (void);
void BGM_Resume (void);
void BGM_Update (void);
extern	qboolean bgm_extmusic;

// Sound DMA stubs (signatures must match q_sound.h)
// qboolean SNDDMA_Init(dma_t *dma);
// int SNDDMA_GetDMAPos (void);
// void SNDDMA_Shutdown (void);
// void SNDDMA_LockBuffer (void);
// void SNDDMA_Submit (void);
// void SNDDMA_BlockSound (void);
// void SNDDMA_UnblockSound (void);

// System input stubs - already declared in sys.h
// const char *Sys_ConsoleInput (void);
// void Sys_SendKeyEvents (void);

// Overflow tracking
typedef struct {
	double	packetsize;
	double	efrags;
	double	beams;
	double	varstring;
} overflowtimes_t;
extern	overflowtimes_t dev_overflows;
#define CONSOLE_RESPAM_TIME 3

// Lightmap definitions for software rendering
#define LMBLOCK_WIDTH	256
#define LMBLOCK_HEIGHT	256

typedef struct swRect_s {
	unsigned short l,t,w,h;
} swRect_t;

typedef struct lightmap_s
{
	swtexture_t	texture;
	swpoly_t		*polys;
	qboolean	modified;
	swRect_t		rectchange;
	byte		*data;
} lightmap_t;

extern	int		lightmap_bytes;
extern	lightmap_t	*lightmap;
extern	int			lightmap_count;
extern	int			allocated[LMBLOCK_WIDTH];

void R_AnimateLight (void);
void R_MarkLeaves (void);
void R_PushDlights (void);
void R_DrawWorld (void);
void R_RenderDlights (void);
void R_DrawParticles (void);
void R_DrawEntitiesOnList (void);
void R_DrawViewModel (void);

// Surface cache
void D_FlushCaches (void);
void D_DeleteSurfaceCache (void);
void D_InitCaches (void *buffer, int size);

// Texture loading for software renderer
swtexture_t SW_LoadTexture (const char *name, byte *data, int width, int height);
void SW_UnloadTexture (swtexture_t tex);

// Terminal-specific output
void SW_OutputToTerminal (void);

// Palette
extern	byte	*host_colormap;

//====================================================
// Light style animation
//====================================================

extern	int		r_framecount;

void R_NewMap (void);
void R_UploadLightmap (int lightmapnum);

#endif // __SWQUAKE_H__