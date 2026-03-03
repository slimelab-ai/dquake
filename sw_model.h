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
// sw_model.h -- Model structures for software/terminal rendering
// Derived from gl_model.h, GL-specific parts removed/adapted

#ifndef __SW_MODEL__
#define __SW_MODEL__

#include "modelgen.h"
#include "spritegn.h"

/*

d*_t structures are on-disk representations
m*_t structures are in-memory

*/

// entity effects
#define	EF_BRIGHTFIELD			1
#define	EF_MUZZLEFLASH 			2
#define	EF_BRIGHTLIGHT 			4
#define	EF_DIMLIGHT 			8

/*
==============================================================================

BRUSH MODELS

==============================================================================
*/

// in memory representation
typedef struct
{
	vec3_t		position;
} mvertex_t;

#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2

// plane_t structure
typedef struct mplane_s
{
	vec3_t	normal;
	float	dist;
	byte	type;			// for texture axis selection and fast side tests
	byte	signbits;		// signx + signy<<1 + signz<<1
	byte	pad[2];
} mplane_t;

// texture chain types
typedef enum {
	chain_world = 0,
	chain_model = 1
} texchain_t;

// Software texture handle (replaces gltexture_s)
typedef unsigned int swtexture_t;

typedef struct texture_s
{
	char				name[16];
	unsigned			width, height;
	swtexture_t			texhandle;		// software texture handle
	swtexture_t			fullbright;		// fullbright mask
	swtexture_t			warpimage;		// for water animation
	qboolean			update_warp;	// update warp this frame
	struct msurface_s	*texturechains[2];	// for texture chains
	int					anim_total;		// total tenths in sequence ( 0 = no)
	int					anim_min, anim_max;	// time for this frame min <=time< max
	struct texture_s	*anim_next;		// in the animation sequence
	struct texture_s	*alternate_anims;	// bmodels in frame 1 use these
	unsigned			offsets[MIPLEVELS];	// four mip maps stored
} texture_t;

// Surface flags
#define	SURF_PLANEBACK		2
#define	SURF_DRAWSKY		4
#define SURF_DRAWSPRITE		8
#define SURF_DRAWTURB		0x10
#define	SURF_DRAWTILED		0x20
#define SURF_DRAWBACKGROUND	0x40
#define SURF_UNDERWATER		0x80
#define SURF_NOTEXTURE		0x100
#define SURF_DRAWFENCE		0x200
#define SURF_DRAWLAVA		0x400
#define SURF_DRAWSLIME		0x800
#define SURF_DRAWTELE		0x1000
#define SURF_DRAWWATER		0x2000

// Edge structure
typedef struct
{
	unsigned int	v[2];
	unsigned int	cachededgeoffset;
} medge_t;

// Texture info
typedef struct
{
	float		vecs[2][4];
	texture_t	*texture;
	int			flags;
} mtexinfo_t;

// Software polygon (replaces glpoly_t)
#define	VERTEXSIZE	7

typedef struct swpoly_s
{
	struct	swpoly_s	*next;
	struct	swpoly_s	*chain;
	int		numverts;
	float	verts[4][VERTEXSIZE];	// variable sized (xyz s1t1 s2t2)
} swpoly_t;

// Surface structure
typedef struct msurface_s
{
	int			visframe;		// should be drawn when node is crossed
	float		mins[3];		// for frustum culling
	float		maxs[3];		// for frustum culling

	mplane_t	*plane;
	int			flags;

	int			firstedge;	// look up in model->surfedges[], negative numbers
	int			numedges;	// are backwards edges

	short		texturemins[2];
	short		extents[2];

	int			light_s, light_t;	// lightmap coordinates

	swpoly_t	*polys;				// multiple if warped
	struct	msurface_s	*texturechain;

	mtexinfo_t	*texinfo;

// lighting info
	int			dlightframe;
	unsigned int	dlightbits[(MAX_DLIGHTS + 31) >> 5];

	int			lightmaptexturenum;
	byte		styles[MAXLIGHTMAPS];
	int			cached_light[MAXLIGHTMAPS];
	qboolean	cached_dlight;
	byte		*samples;		// [numstyles*surfsize]
} msurface_t;

// Node structure
typedef struct mnode_s
{
// common with leaf
	int			contents;		// 0, to differentiate from leafs
	int			visframe;		// node needs to be traversed if current

	float		minmaxs[6];		// for bounding box culling

	struct mnode_s	*parent;

// node specific
	mplane_t	*plane;
	struct mnode_s	*children[2];

	unsigned int	firstsurface;
	unsigned int	numsurfaces;
} mnode_t;

// Leaf structure
typedef struct mleaf_s
{
// common with node
	int			contents;		// will be a negative contents number
	int			visframe;		// node needs to be traversed if current

	float		minmaxs[6];		// for bounding box culling

	struct mnode_s	*parent;

// leaf specific
	byte		*compressed_vis;
	struct efrag_s	*efrags;

	int			area;			// Q2 BSP support

	msurface_t	**firstmarksurface;
	int			nummarksurfaces;
	int			key;			// BSP sequence number for leaf's contents
	byte		ambient_sound_level[NUM_AMBIENTS];
} mleaf_t;

// Clipnode structure for hull collision
typedef struct mclipnode_s
{
	int			planenum;
	int			children[2]; // negative numbers are contents
} mclipnode_t;

// Hull structure for collision
typedef struct hull_s
{
	mclipnode_t	*clipnodes;
	mplane_t	*planes;
	int			firstclipnode;
	int			lastclipnode;
	vec3_t		clip_mins;
	vec3_t		clip_maxs;
} hull_t;

/*
==============================================================================

SPRITE MODELS

==============================================================================
*/

// Sprite frame
typedef struct mspriteframe_s
{
	int					width;
	int					height;
	float				up, down, left, right;
	swtexture_t			gl_texturenum;	// software texture
} mspriteframe_t;

typedef struct mspriteframedesc_s
{
	int					numframes;
	float				interval;
	struct mspriteframe_s	*frames;
} mspriteframedesc_t;

typedef struct msprite_s
{
	int					type;
	int					maxwidth;
	int					maxheight;
	int					numframes;
	float				beamsync;	// Q2 sprite support
	int					synctype;	// Q2 sprite support
	mspriteframedesc_t	frames[1];
} msprite_t;

/*
==============================================================================

ALIAS MODELS

==============================================================================
*/

/*
trivertx_t is the representation of a single triangular vertex
it's transformations are:
  vertex = trivert + frame_pos
  skin = trivert + frame_pos  // for now
it is transformed by the frame and by the tag
*/

// Note: trivertx_t is defined in modelgen.h

// Alias model types
typedef struct
{
	int					firstpose;
	int					numposes;
	float				interval;
	trivertx_t			bboxmin;
	trivertx_t			bboxmax;
	int					frame;
	char				name[16];
} maliasframedesc_t;

typedef struct
{
	trivertx_t			bboxmin;
	trivertx_t			bboxmax;
	int					frame;
} maliasgroupframedesc_t;

typedef struct
{
	int						numframes;
	int						intervals;
	maliasgroupframedesc_t	frames[1];
} maliasgroup_t;

// Triangle structure
typedef struct mtriangle_s {
	int					facesfront;
	int					vertindex[3];
} mtriangle_t;

#define	MAX_SKINS	32

// Alias model header (software version - no GL textures)
typedef struct {
	int			ident;
	int			version;
	vec3_t		scale;
	vec3_t		scale_origin;
	float		boundingradius;
	vec3_t		eyeposition;
	int			numskins;
	int			skinwidth;
	int			skinheight;
	int			numverts;
	int			numtris;
	int			numframes;
	synctype_t	synctype;
	int			flags;
	float		size;

	// Software rendering data
	int			numverts_vbo;
	intptr_t		meshdesc;
	int			numindexes;
	intptr_t		indexes;
	intptr_t		vertexes;

	int					numposes;
	int					poseverts;
	int					posedata;
	int					commands;
	swtexture_t			gltextures[MAX_SKINS][4];	// software texture handles
	swtexture_t			fbtextures[MAX_SKINS][4];	// fullbright textures
	int					texels[MAX_SKINS];
	maliasframedesc_t	frames[1];		// variable sized
} aliashdr_t;

#define	MAXALIASVERTS	2000
#define	MAXALIASFRAMES	256
#define	MAXALIASTRIS	4096

extern	aliashdr_t	*pheader;

// Alias mesh vertex data
typedef struct aliasmesh_s
{
	float st[2];
	unsigned short vertindex;
} aliasmesh_t;

typedef struct meshxyz_s
{
	byte xyz[4];
	signed char normal[4];
} meshxyz_t;

typedef struct meshst_s
{
	float st[2];
} meshst_t;

// Extra flags for rendering
#define	MOD_NOLERP		256		// don't lerp when animating
#define	MOD_NOSHADOW	512		// don't cast a shadow
#define	MOD_FBRIGHTHACK	1024	// fullbright hack

// Model type enum
typedef enum {mod_brush, mod_sprite, mod_alias} modtype_t;

/*
==============================================================================

MODEL STRUCTURE

==============================================================================
*/

typedef struct qmodel_s
{
	char		name[MAX_QPATH];
	unsigned int	path_id;		// path id of the game directory
	qboolean	needload;		// bmodels and sprites don't cache normally

	modtype_t	type;
	int			numframes;
	synctype_t	synctype;

	int			flags;

//
// volume occupied by the model graphics
//
	vec3_t		mins, maxs;
	vec3_t		ymins, ymaxs;	// bounds for entities with nonzero yaw
	vec3_t		rmins, rmaxs;	// bounds for entities with nonzero pitch or roll

//
// solid volume for clipping
//
	qboolean	clipbox;
	vec3_t		clipmins, clipmaxs;

//
// brush model
//
	int			firstmodelsurface, nummodelsurfaces;

	int			numsubmodels;
	dmodel_t	*submodels;

	int			numplanes;
	mplane_t	*planes;

	int			numleafs;		// number of visible leafs, not counting 0
	mleaf_t		*leafs;

	int			numvertexes;
	mvertex_t	*vertexes;

	int			numedges;
	medge_t		*edges;

	int			numnodes;
	mnode_t	*nodes;

	int			numtexinfo;
	mtexinfo_t	*texinfo;

	int			numsurfaces;
	msurface_t	*surfaces;

	int			numsurfedges;
	int			*surfedges;

	int			numclipnodes;
	mclipnode_t	*clipnodes;

	int			nummarksurfaces;
	msurface_t	**marksurfaces;

	hull_t		hulls[MAX_MAP_HULLS];

	int			numtextures;
	texture_t	**textures;

	byte		*visdata;
	byte		*lightdata;
	char		*entities;

	qboolean	viswarn;

	int			bspversion;

//
// alias model (software - no VBO handles)
//
	int			numposes;
	int			poseverts;
	int			numtris;
	int			numst;
	trivertx_t	*posedata;		// numposes * poseverts * sizeof(trivertx_t)
	int			*triangles;		// triangle indexes
	int			*stverts;		// st indexes

//
// additional model data
//
	cache_user_t	cache;		// only access through Mod_Extradata

} qmodel_t;

//============================================================================

// Function prototypes
void		Mod_Init (void);
void		Mod_ClearAll (void);
void		Mod_ResetAll (void);
qmodel_t	*Mod_ForName (const char *name, qboolean crash);
void		*Mod_Extradata (qmodel_t *mod);
void		Mod_TouchModel (const char *name);

mleaf_t		*Mod_PointInLeaf (float *p, qmodel_t *model);
byte		*Mod_LeafPVS (mleaf_t *leaf, qmodel_t *model);
byte		*Mod_NoVisPVS (qmodel_t *model);

void		Mod_SetExtraFlags (qmodel_t *mod);

#endif // __SW_MODEL__