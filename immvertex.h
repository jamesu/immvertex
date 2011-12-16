#ifndef _IMMVERTEX_H_
#define _IMMVERTEX_H_

//
//  immvertex
// 
//  GLES Immediate mode function API for building vertex arrays
// 
//  example:
//  vxBegin(GL_TRIANGLE_STRIP);
//    vxColor4f(1.0, 1.0, 0.0, 1.0);
//    vxVertex2f(-0.5f, -0.5f);
//    vxColor4f(0.0, 1.0, 1.0, 1.0);
//    vxVertex2f(0.5f,  -0.5f);
//    vxColor4f(0.0, 0.0, 0.0, 0.0);
//    vxVertex2f(-0.5f,  0.5f);
//    vxColor4f(1.0, 0.0, 1.0, 1.0);
//    vxVertex2f(0.5f,   0.5f);
//  vxEnd();
//
//  Copyright 2009-2011 James Urquhart (jamesu at gmail dot com). All rights reserved.
//  Permission is hereby granted, free of charge, to any person
//  obtaining a copy of this software and associated documentation
//  files (the "Software"), to deal in the Software without
//  restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following
//  conditions:
//  
//  The above copyright notice and this permission notice shall be
//  included in all copies or substantial portions of the Software.
// 
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//  OTHER DEALINGS IN THE SOFTWARE.


#ifndef GL_QUADS
#define GL_QUADS                            0x0007
#endif

//#ifndef GL_QUAD_STRIP
//#define GL_QUAD_STRIP                       0x0008
//#endif

#ifndef GL_POLYGON
#define GL_POLYGON                          0x0009
#endif

#define VARR_COLOR 0x2
#define VARR_ALPHA 0x4
#define VARR_NORMAL 0x8
#define VARR_TEXCOORD0 0x10
#define VARR_3POINT 0x20

#ifdef __cplusplus
extern "C" {
#endif


typedef struct vxRenderState {
	// GL buffer which is required to draw
	GLint vbo;
	GLint mode;
    
    // vertex info
    int format;
    int vertex_size;
    int verts;
} vxRenderState;

//
// Vertex array state
//

// Begins a new vertex array using VARR_* flags
extern void vxBeginVertexArray(int format);

// Get pointer to current vertex array
extern void *vxCurrentVertexArray();

// Render array of verts using info
extern void vxRenderArray(vxRenderState info, void *verts);

// Frees memory used by vertex array builder
extern void vxDestroyState();

//
// Vertex buffer state
//

// Begin storing vertices to vertex buffer. Call before vxBegin
extern void vxBeginVertexBuffer(GLint buffer, int format);

// Render buffer referenced in info
extern void vxRenderBuffer(vxRenderState info);

//
// Vertex builder
// Note: vertices are generated using the current normal, color, and texture states
//

// Start a new array (equivalent to glBegin)
extern void vxBegin(int flags);

    
// Present array to current GLES context, or to the current vertex bufer (equivalent to glEnd)
extern vxRenderState vxEnd();

extern void vxVertex2f(float x, float y);
extern void vxVertex2i(int x, int y);

extern void vxVertex3f(float x, float y, float z);
extern void vxVertex3d(int x, int y, int z);
extern void vxVertex3fv(float *ptr);

extern void vxTexCoord2f(float x, float y);
extern void vxTexCoord2fv(float *ptr);

extern void vxNormal3fv(float *ptr);

extern void vxColor3f(float x, float y, float z);
extern void vxColor3fv(float *ptr);

extern void vxColor4f(float x, float y, float z, float a);
extern void vxColor4fv(const float *ptr);

#ifdef __cplusplus
}
#endif

#endif