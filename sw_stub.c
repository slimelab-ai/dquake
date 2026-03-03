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
// sw_stub.c -- Stub implementations for software/terminal rendering
// Provides minimal/no-op versions of GL-dependent functions

#include "quakedef.h"

// GL-compatible global variables (used by game code)
int		glx, gly, glwidth, glheight;

// Video state (defined in vid.h as extern)
viddef_t		vid;

// Video mode state
modestate_t	modestate = MS_WINDOWED;

// Render definition
refdef_t	r_refdef;

// View vectors
vec3_t		vup, vpn, vright;
vec3_t		modelorg, r_entorigin;
vec3_t		r_origin;

// Frame counters
int		r_framecount = 0;
int		r_visframecount = 0;

// Current entity
entity_t	*currententity = NULL;

// View leaf
mleaf_t		*r_viewleaf = NULL;
mleaf_t		*r_oldviewleaf = NULL;

// Frustum (for clipping)
mplane_t	frustum[4];

// Light style values
int		d_lightstylevalue[256];

// Cache state
qboolean	r_cache_thrash = false;

// Lightmap globals
int		lightmap_bytes = 4;
lightmap_t	*lightmap = NULL;
int		lightmap_count = 0;
int		allocated[LMBLOCK_WIDTH];

//============================================================
// Cvars for GL settings (stubs for terminal mode)
//============================================================

cvar_t	r_norefresh = {"r_norefresh", "0"};
cvar_t	r_drawentities = {"r_drawentities", "1"};
cvar_t	r_drawworld = {"r_drawworld", "1"};
cvar_t	r_drawviewmodel = {"r_drawviewmodel", "1"};
cvar_t	r_speeds = {"r_speeds", "0"};
cvar_t	r_pos = {"r_pos", "0"};
cvar_t	r_waterwarp = {"r_waterwarp", "1"};
cvar_t	r_fullbright = {"r_fullbright", "0"};
cvar_t	r_lightmap = {"r_lightmap", "0"};
cvar_t	r_shadows = {"r_shadows", "0"};
cvar_t	r_wateralpha = {"r_wateralpha", "1"};
cvar_t	r_lavaalpha = {"r_lavaalpha", "1"};
cvar_t	r_telealpha = {"r_telealpha", "1"};
cvar_t	r_slimealpha = {"r_slimealpha", "1"};
cvar_t	r_dynamic = {"r_dynamic", "1"};
cvar_t	r_novis = {"r_novis", "0"};
cvar_t	r_scale = {"r_scale", "1"};

// GL-specific cvars
cvar_t	gl_clear = {"gl_clear", "0"};
qboolean gl_glsl_gamma_able = false;
cvar_t	gl_cull = {"gl_cull", "1"};
cvar_t	gl_smoothmodels = {"gl_smoothmodels", "1"};
cvar_t	gl_affinemodels = {"gl_affinemodels", "0"};
cvar_t	gl_polyblend = {"gl_polyblend", "1"};
cvar_t	gl_flashblend = {"gl_flashblend", "1"};
cvar_t	gl_playermip = {"gl_playermip", "0"};
cvar_t	gl_nocolors = {"gl_nocolors", "0"};
cvar_t	gl_keeptjunctions = {"gl_keeptjunctions", "1"};
cvar_t	gl_reporttjunctions = {"gl_reporttjunctions", "0"};
cvar_t	gl_doubleeyes = {"gl_doubleeyes", "1"};
cvar_t	gl_zfix = {"gl_zfix", "0"};
cvar_t	gl_overbright = {"gl_overbright", "1"};
cvar_t	gl_overbright_models = {"gl_overbright_models", "1"};
cvar_t	gl_fullbrights = {"gl_fullbrights", "1"};
cvar_t	gl_farclip = {"gl_farclip", "16384"};
cvar_t	gl_oldskyleaf = {"gl_oldskyleaf", "0"};
cvar_t	gl_brush_polygon_offset = {"gl_brush_polygon_offset", "0.05"};
cvar_t	gl_texturemode = {"gl_texturemode", "GL_LINEAR_MIPMAP_LINEAR"};
cvar_t	gl_texture_anisotropy = {"gl_texture_anisotropy", "1"};
cvar_t	gl_max_size = {"gl_max_size", "2048"};
cvar_t	gl_anisotropy = {"gl_anisotropy", "1"};
cvar_t	gl_subdivide_size = {"gl_subdivide_size", "128"};
cvar_t	gl_palettedtexture = {"gl_palettedtexture", "0"};
cvar_t	gl_multitexture = {"gl_multitexture", "0"};

// Video cvars
cvar_t	vid_gamma = {"vid_gamma", "1"};
cvar_t	vid_contrast = {"vid_contrast", "1"};
cvar_t	scr_sbaralpha = {"scr_sbaralpha", "0.59"};

// Screen cvars
cvar_t	scr_viewsize = {"viewsize", "100"};
cvar_t	scr_conscale = {"con_scale", "1"};
cvar_t	scr_sbarscale = {"sb_scale", "1"};
cvar_t	scr_centertime = {"scr_centertime", "2"};

// Lerp cvars
cvar_t	r_lerpmodels = {"r_lerpmodels", "1"};
cvar_t	r_lerpmove = {"r_lerpmove", "1"};

// Screen state
qboolean	scr_disabled_for_loading = false;
int		scr_tileclear_updates = 0;
float		scr_con_current = 0;
int		clearnotify = 0;
float		scr_centertime_off = 0;

// Picture globals
qpic_t	*draw_disc = NULL;
qpic_t	*pic_ins = NULL;
qpic_t	*pic_ovr = NULL;

// BGM
qboolean	bgm_extmusic = false;

//============================================================
// Rendering stubs
//============================================================

void R_AnimateLight(void)
{
	int i;
	for (i = 0; i < 256; i++)
		d_lightstylevalue[i] = 256;
}

void R_MarkLeaves(void) {}
void R_PushDlights(void) {}
void R_DrawWorld(void) {}
void R_RenderDlights(void) {}
void R_DrawParticles(void) {}
void R_DrawEntitiesOnList(void) {}
void R_DrawViewModel(void) {}

void R_TimeRefresh_f(void)
{
	Con_Printf("R_TimeRefresh: Not implemented for terminal mode\n");
}

void R_ReadPointFile_f(void)
{
	Con_Printf("R_ReadPointFile: Not implemented for terminal mode\n");
}

texture_t *R_TextureAnimation(texture_t *base, int frame)
{
	return base;
}

void R_RenderView(void) {}

void R_NewMap(void) {}
void R_UploadLightmap(int num) {}
void R_Init(void) { Con_Printf("R_Init: Terminal mode\n"); }
void R_InitEfrags(void) {}
void R_CheckEfrags(void) {}
void R_AddEfrags(entity_t *ent) { (void)ent; }
void R_ViewChanged(vrect_t *pvrect, int lineadj, float aspect) { (void)pvrect; (void)lineadj; (void)aspect; }

void R_SetVrect(vrect_t *pvrect, vrect_t *pvrectin, int lineadj)
{
	pvrect->x = 0;
	pvrect->y = 0;
	pvrect->width = glwidth;
	pvrect->height = glheight;
	pvrectin->x = pvrect->x;
	pvrectin->y = pvrect->y;
	pvrectin->width = pvrect->width;
	pvrectin->height = pvrect->height;
}

//============================================================
// Particle stubs
//============================================================

void CL_RunParticles(void) {}
void R_ParseParticleEffect(void) {}
void R_RunParticleEffect(vec3_t org, vec3_t dir, int color, int count) { (void)org; (void)dir; (void)color; (void)count; }
void R_RocketTrail(vec3_t start, vec3_t end, int type) { (void)start; (void)end; (void)type; }
void R_EntityParticles(entity_t *ent) { (void)ent; }
void R_BlobExplosion(vec3_t org) { (void)org; }
void R_ParticleExplosion(vec3_t org) { (void)org; }
void R_ParticleExplosion2(vec3_t org, int cs, int cl) { (void)org; (void)cs; (void)cl; }
void R_LavaSplash(vec3_t org) { (void)org; }
void R_TeleportSplash(vec3_t org) { (void)org; }

//============================================================
// Texture stubs
//============================================================

swtexture_t SW_LoadTexture(const char *name, byte *data, int w, int h)
{
	static swtexture_t next = 1;
	return next++;
}

void SW_UnloadTexture(swtexture_t tex) { (void)tex; }
void D_FlushCaches(void) {}
void D_DeleteSurfaceCache(void) {}
void D_InitCaches(void *buf, int size) { (void)buf; (void)size; }

//============================================================
// Player skin stubs
//============================================================

void R_TranslatePlayerSkin(int slot) { (void)slot; }
void R_TranslateNewPlayerSkin(int slot) { (void)slot; }

//============================================================
// Sky/Fog stubs
//============================================================

void Sky_LoadSkyBox(const char *name) { Con_Printf("Sky_LoadSkyBox: %s - not supported\n", name); }
void Fog_ParseServerMessage(void) {}

//============================================================
// GL helper stubs
//============================================================

void GL_DisableMultitexture(void) {}
void GL_EnableMultitexture(void) {}
void GL_SelectTexture(int target) { (void)target; }
void GL_SetCanvas(canvastype canvas) { (void)canvas; }

//============================================================
// SW rendering
//============================================================

void SW_BeginRendering(int *x, int *y, int *w, int *h)
{
	*x = 0; *y = 0; *w = glwidth; *h = glheight;
}

void SW_EndRendering(void) {}
void SW_Set2D(void) {}

//============================================================
// Model stubs
//============================================================

void Mod_Init(void) { Con_Printf("Mod_Init: Terminal mode\n"); }
void Mod_ClearAll(void) {}
void Mod_ResetAll(void) {}

qmodel_t *Mod_ForName(const char *name, qboolean crash)
{
	if (crash) Sys_Error("Mod_ForName: %s not found (terminal mode)", name);
	return NULL;
}

void *Mod_Extradata(qmodel_t *mod) { return NULL; }
void Mod_TouchModel(const char *name) { (void)name; }
mleaf_t *Mod_PointInLeaf(float *p, qmodel_t *model) { return NULL; }
byte *Mod_LeafPVS(mleaf_t *leaf, qmodel_t *model) { return NULL; }
byte *Mod_NoVisPVS(qmodel_t *model) { return NULL; }
void Mod_Print(void) { Con_Printf("Mod_Print: Terminal mode\n"); }
void Mod_SetExtraFlags(qmodel_t *mod) { (void)mod; }

//============================================================
// Texture manager stubs
//============================================================

void TexMgr_Init(void) {}
void TexMgr_NewGame(void) {}
void TexMgr_FreeTexturesForOwner(qmodel_t *owner) { (void)owner; }
void R_NewGame(void) {}

//============================================================
// Screen stubs
//============================================================

void SCR_Init(void) {}
void SCR_BeginLoadingPlaque(void) { scr_disabled_for_loading = true; }
void SCR_EndLoadingPlaque(void) { scr_disabled_for_loading = false; }
void SCR_UpdateScreen(void) {}
void SCR_CenterPrint(const char *str) { Con_Printf("%s\n", str); }
int SCR_ModalMessage(const char *text, float timeout) { Con_Printf("%s\n", text); (void)timeout; return 1; }

//============================================================
// Draw stubs
//============================================================

void Draw_Init(void) {}
void Draw_NewGame(void) {}
void Draw_Character(int x, int y, int num) { (void)x; (void)y; (void)num; }
void Draw_String(int x, int y, const char *str) { (void)x; (void)y; (void)str; }
qpic_t *Draw_PicFromWad(const char *name) { static qpic_t d; return &d; }
void Draw_Pic(int x, int y, qpic_t *pic) { (void)x; (void)y; (void)pic; }
qpic_t *Draw_CachePic(const char *path) { static qpic_t d; return &d; }
void Draw_TransPicTranslate(int x, int y, qpic_t *pic, int top, int bot) { (void)x; (void)y; (void)pic; (void)top; (void)bot; }
void Draw_ConsoleBackground(void) {}
void Draw_FadeScreen(void) {}
void Draw_Fill(int x, int y, int w, int h, int c, float a) { (void)x; (void)y; (void)w; (void)h; (void)c; (void)a; }
void Draw_TileClear(int x, int y, int w, int h) { (void)x; (void)y; (void)w; (void)h; }

//============================================================
// BGM stubs
//============================================================

void BGM_Init(void) {}
void BGM_Shutdown(void) {}
void BGM_PlayCDtrack(int track, qboolean looping) { (void)track; (void)looping; }
void BGM_Stop(void) {}
void BGM_Pause(void) {}
void BGM_Resume(void) {}
void BGM_Update(void) {}

//============================================================
// Sound DMA stubs
//============================================================

qboolean SNDDMA_Init(dma_t *dma)
{
	Con_Printf("SNDDMA_Init: No sound in terminal mode\n");
	return false;
}

int SNDDMA_GetDMAPos(void) { return 0; }
void SNDDMA_Shutdown(void) {}
void SNDDMA_LockBuffer(void) {}
void SNDDMA_Submit(void) {}
void SNDDMA_BlockSound(void) {}
void SNDDMA_UnblockSound(void) {}

//============================================================
// System input stubs
//============================================================

void Sys_SendKeyEvents(void) {}
const char *Sys_ConsoleInput(void) { return NULL; }