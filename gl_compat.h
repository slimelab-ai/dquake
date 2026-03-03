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
// gl_compat.h -- GL compatibility stubs for terminal mode
// Provides empty GL defines and inline stubs

#ifndef __GL_COMPAT_H__
#define __GL_COMPAT_H__

#ifndef USE_TERMINAL
#error "gl_compat.h should only be included in terminal mode"
#endif

// GL type definitions (minimal)
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef unsigned char GLubyte;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef char GLchar;
typedef void GLvoid;

// GL boolean
#define GL_FALSE 0
#define GL_TRUE 1

// GL primitives
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_STRIP 0x0003
#define GL_LINE_LOOP 0x0002
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_QUADS 0x0007
#define GL_POLYGON 0x0009

// GL matrix modes
#define GL_MODELVIEW 0x1700
#define GL_PROJECTION 0x1701
#define GL_TEXTURE 0x1702

// GL enables
#define GL_BLEND 0x0BE2
#define GL_ALPHA_TEST 0x0BC0
#define GL_DEPTH_TEST 0x0B71
#define GL_SCISSOR_TEST 0x0C11
#define GL_TEXTURE_2D 0x0DE1
#define GL_CULL_FACE 0x0B44
#define GL_FOG 0x0B60
#define GL_LIGHTING 0x0B50

// GL texture parameters
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_REPEAT 0x2901
#define GL_CLAMP 0x2900
#define GL_CLAMP_TO_EDGE 0x812F

// GL blend functions
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_ONE 1
#define GL_ZERO 0

// GL depth function
#define GL_LEQUAL 0x0203
#define GL_LESS 0x0201

// GL pixel formats
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_UNSIGNED_BYTE 0x1401

// GL misc
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100

// Inline stub functions for GL calls - these do nothing in terminal mode

static inline void glBegin(GLenum mode) { (void)mode; }
static inline void glEnd(void) {}
static inline void glVertex3f(GLfloat x, GLfloat y, GLfloat z) { (void)x; (void)y; (void)z; }
static inline void glVertex2f(GLfloat x, GLfloat y) { (void)x; (void)y; }
static inline void glVertex3fv(const GLfloat *v) { (void)v; }
static inline void glVertex2fv(const GLfloat *v) { (void)v; }
static inline void glTexCoord2f(GLfloat s, GLfloat t) { (void)s; (void)t; }
static inline void glTexCoord2fv(const GLfloat *v) { (void)v; }
static inline void glColor3f(GLfloat r, GLfloat g, GLfloat b) { (void)r; (void)g; (void)b; }
static inline void glColor3fv(const GLfloat *v) { (void)v; }
static inline void glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { (void)r; (void)g; (void)b; (void)a; }
static inline void glColor4ub(GLubyte r, GLubyte g, GLubyte b, GLubyte a) { (void)r; (void)g; (void)b; (void)a; }
static inline void glColor3ub(GLubyte r, GLubyte g, GLubyte b) { (void)r; (void)g; (void)b; }
static inline void glColor4fv(const GLfloat *v) { (void)v; }
static inline void glDisable(GLenum cap) { (void)cap; }
static inline void glEnable(GLenum cap) { (void)cap; }
static inline void glMatrixMode(GLenum mode) { (void)mode; }
static inline void glLoadIdentity(void) {}
static inline void glPushMatrix(void) {}
static inline void glPopMatrix(void) {}
static inline void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val) { (void)left; (void)right; (void)bottom; (void)top; (void)near_val; (void)far_val; }
static inline void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val) { (void)left; (void)right; (void)bottom; (void)top; (void)near_val; (void)far_val; }
static inline void glTranslatef(GLfloat x, GLfloat y, GLfloat z) { (void)x; (void)y; (void)z; }
static inline void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) { (void)angle; (void)x; (void)y; (void)z; }
static inline void glScalef(GLfloat x, GLfloat y, GLfloat z) { (void)x; (void)y; (void)z; }
static inline void glClear(GLbitfield mask) { (void)mask; }
static inline void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) { (void)red; (void)green; (void)blue; (void)alpha; }
static inline void glClearDepth(GLdouble depth) { (void)depth; }
static inline void glDepthFunc(GLenum func) { (void)func; }
static inline void glBlendFunc(GLenum sfactor, GLenum dfactor) { (void)sfactor; (void)dfactor; }
static inline void glScissor(GLint x, GLint y, GLsizei width, GLsizei height) { (void)x; (void)y; (void)width; (void)height; }
static inline void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) { (void)x; (void)y; (void)width; (void)height; }
static inline void glTexParameteri(GLenum target, GLenum pname, GLint param) { (void)target; (void)pname; (void)param; }
static inline void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) { (void)target; (void)level; (void)internalformat; (void)width; (void)height; (void)border; (void)format; (void)type; (void)pixels; }
static inline void glBindTexture(GLenum target, GLuint texture) { (void)target; (void)texture; }
static inline void glGenTextures(GLsizei n, GLuint *textures) { (void)n; if (textures) *textures = 0; }
static inline void glDeleteTextures(GLsizei n, const GLuint *textures) { (void)n; (void)textures; }
static inline void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) { (void)red; (void)green; (void)blue; (void)alpha; }
static inline void glDepthMask(GLboolean flag) { (void)flag; }
static inline void glDrawBuffer(GLenum mode) { (void)mode; }
static inline void glReadBuffer(GLenum mode) { (void)mode; }
static inline void glGetIntegerv(GLenum pname, GLint *params) { (void)pname; if (params) *params = 0; }
static inline void glGetFloatv(GLenum pname, GLfloat *params) { (void)pname; (void)params; }
static inline void glFlush(void) {}
static inline void glFinish(void) {}

// GLU functions (rarely used)
static inline void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar) { (void)fovy; (void)aspect; (void)zNear; (void)zFar; }
static inline void gluOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top) { (void)left; (void)right; (void)bottom; (void)top; }

// GL extension definitions used by QuakeSpasm
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

// GLSL types (for modern GL - stubs)
typedef unsigned int GLhandleARB;
typedef char GLcharARB;
typedef void (APIENTRY *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

#endif // __GL_COMPAT_H__