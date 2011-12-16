//
//  immvertex
//
//  GLES Immediate mode function API for building vertex arrays
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


#import <OpenGLES/ES1/gl.h>
#import "immvertex.h"

#include <stdlib.h>

#define BUFFER_CHUNK 65536

typedef struct {
    char *buffer;
    char *ptr;
    int buffer_size;
    
    int format;
    int vertex_size;
    int verts;
    
    int geom;
    
    GLint vbo;
    
    float cur_normal[3];
    float cur_color[3];
    float cur_texcoord[2];
    
    // TODO: need to remember normal, color & texcoord states!
    float vtx[4][3];
    
} vstat_t;

static vstat_t *vstate = 0;

void vxCreateState()
{
    vstate = malloc(sizeof(vstat_t));
    
    vstate->buffer = malloc(BUFFER_CHUNK);
    vstate->buffer_size = BUFFER_CHUNK;
    
    vstate->format = 0;
    vstate->verts = 0;
    vstate->vertex_size = 0;
    vstate->ptr = vstate->buffer;
    
    vstate->geom = -1;
    vstate->vbo = 0;
}

void vxDestroyState()
{
    if (vstate == NULL)
        return;
    
    free(vstate->buffer);
    free(vstate);
    vstate = NULL;
}

void vxIncrementSize()
{
    int offs = vstate->ptr - vstate->buffer;
    vstate->buffer = realloc(vstate->buffer, vstate->buffer_size + BUFFER_CHUNK);
    vstate->buffer_size += BUFFER_CHUNK;
    vstate->ptr = vstate->buffer + offs;
}

int vxFormatSize()
{
    int calc = 3*sizeof(float);
    
    if (vstate->format & VARR_COLOR)
        calc += 3*sizeof(float);
    if (vstate->format & VARR_ALPHA)
        calc += sizeof(float);
    
    if (vstate->format & VARR_NORMAL)
        calc += 3*sizeof(float);
    
    if (vstate->format & VARR_TEXCOORD0)
        calc += 3*sizeof(float);
    
    return calc;
}

void vxBeginVertexArray(int format)
{
    if (!vstate)
        vxCreateState();
    
    vstate->verts = 0;
    vstate->format = format;
    vstate->vertex_size = vxFormatSize();
}

void vxBeginVertexBuffer(GLint buffer, int format)
{
    if (!vstate)
        vxCreateState();
    
    vxBeginVertexArray(format);
    vstate->vbo = buffer;
}

void *vxCurrentVertexArray()
{
    return vstate->buffer;
}

int vxEndVertexArray()
{
    // You want size with that?
    return vstate->buffer - vstate->ptr;
}

int vxEndVertexBuffer()
{
    vstate->vbo = 0;
    if (vstate->geom == GL_QUAD_STRIP && vstate->verts % 2 != 0)
        return vstate->verts-1;
    else
        return vstate->verts;
}

// Vertex builder
void vxBegin(int flags)
{
    // no state or previous state?
    if (vstate == NULL || vstate->geom != 0)
        vxBeginVertexArray(0);
    
    vstate->geom = flags;
    vstate->ptr = vstate->buffer;
    vstate->verts = 0;
}

void vxRenderInfo(vxRenderInfo info)
{
   if (info.vbo == 0)
     return;

   glBindBuffer(GL_ARRAY_BUFFER, info.vbo);
   renderArray(NULL);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vxRenderArray(vxRenderInfo info, void *verts)
{
	// present vertex array
	GLenum correctMode;
	int correctVerts = vstate->verts;
	if (vstate->geom == GL_QUADS)
	   correctMode = GL_TRIANGLES;
	else if (vstate->geom == GL_QUAD_STRIP) {
	   correctMode = GL_TRIANGLE_STRIP;
	   if (correctVerts % 2 != 0)
	       correctVerts--;
	}
	else
	   correctMode = (GLenum)vstate->geom;

	// Set arrays
	char *ptr = vstate->buffer;
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, vstate->vertex_size, ptr);
	ptr += sizeof(float)*3;

	if (vstate->format & VARR_NORMAL) {
	   glEnableClientState(GL_NORMAL_ARRAY);
	   glNormalPointer(GL_FLOAT, vstate->vertex_size, ptr);
	   ptr += sizeof(float)*3;
	} else
	   glDisableClientState(GL_NORMAL_ARRAY);

	if (vstate->format & VARR_COLOR) {
	   glEnableClientState(GL_COLOR_ARRAY);
	   if (vstate->format & VARR_ALPHA) {
	       glColorPointer(4, GL_FLOAT, vstate->vertex_size, ptr);
	       ptr += sizeof(float)*4;
	   } else {
	       glColorPointer(3, GL_FLOAT, vstate->vertex_size, ptr);
	       ptr += sizeof(float)*3;
	   }
	} else
	   glDisableClientState(GL_COLOR_ARRAY);


	if (vstate->format & VARR_TEXCOORD0) {
	   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	   glTexCoordPointer(2, GL_FLOAT, vstate->vertex_size, ptr);
	} else
	   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawArrays(correctMode, 0, vstate->verts);
}

void vxEnd()
{
    vxRenderInfo r_info = vstate->render_info;
    if (r_info->vbo == 0) {
		vxRenderArray(r_info, vstate->buffer);
    } else {
        // upload to vbo
        glBindBuffer(GL_ARRAY_BUFFER, r_info.vbo);
        glBufferData(GL_ARRAY_BUFFER, 
                     vstate->ptr - vstate->buffer,
                     vstate->buffer,
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

// note: optimal format is
// Position, Normal, Color, TexCoord0, TexCoord1, PointSize, Weight, MatrixIndex
void vxStoreVertex2(float x, float y)
{
    if (vstate->ptr - vstate->buffer >= vstate->buffer_size)
        vxIncrementSize();
    
    float *dest = (float*)vstate->ptr;
    *dest++ = x;
    *dest++ = y;
    
    if (vstate->format & VARR_NORMAL) {
        *dest++ = vstate->cur_normal[0];
        *dest++ = vstate->cur_normal[1];
        *dest++ = vstate->cur_normal[2];
    }
    
    if (vstate->format & VARR_COLOR) {
        unsigned char *subDest = (unsigned char*)dest;
        *subDest++ = vstate->cur_color[0] * 255 + 0.5;
        *subDest++ = vstate->cur_color[1] * 255 + 0.5;
        *subDest++ = vstate->cur_color[2] * 255 + 0.5;
        
        if (vstate->format & VARR_ALPHA) {
            *subDest++ = vstate->cur_color[3]  * 255 + 0.5;
        }
        dest = (float*)subDest;
    }
    
    if (vstate->format & VARR_TEXCOORD0) {
        *dest++ = vstate->cur_texcoord[0];
        *dest++ = vstate->cur_texcoord[1];
    }
    
    // next
    vstate->ptr += vstate->vertex_size;
    vstate->verts++;
}

// note: optimal format is
// Position, Normal, Color, TexCoord0, TexCoord1, PointSize, Weight, MatrixIndex
void vxStoreVertex3(float x, float y, float z)
{
    if (vstate->ptr - vstate->buffer >= vstate->buffer_size)
        vxIncrementSize();
    
    float *dest = (float*)vstate->ptr;
    *dest++ = x;
    *dest++ = y;
    *dest++ = z;
    
    if (vstate->format & VARR_NORMAL) {
        *dest++ = vstate->cur_normal[0];
        *dest++ = vstate->cur_normal[1];
        *dest++ = vstate->cur_normal[2];
    }
    
    if (vstate->format & VARR_COLOR) {
    	unsigned char *subDest = (unsigned char*)dest;
        *subDest++ = vstate->cur_color[0] * 255 + 0.5;
        *subDest++ = vstate->cur_color[1] * 255 + 0.5;
        *subDest++ = vstate->cur_color[2] * 255 + 0.5;
        
        if (vstate->format & VARR_ALPHA) {
            *subDest++ = vstate->cur_color[3]  * 255 + 0.5;
        }
        dest = (float*)subDest;
    }
    
    if (vstate->format & VARR_TEXCOORD0) {
        *dest++ = vstate->cur_texcoord[0];
        *dest++ = vstate->cur_texcoord[1];
    }
    
    // next
    vstate->ptr += vstate->vertex_size;
    vstate->verts++;
}

void vxVertex2f(float x, float y)
{
    vxStoreVertex2(x, y);
}

void vxVertex2i(int x, int y)
{
    vxStoreVertex2(x, y);
}

void vxVertex3f(float x, float y, float z)
{
    vxStoreVertex3(x, y, z);
}

void vxVertex3d(int x, int y, int z)
{
    vxStoreVertex3(x, y, z);
}

void vxVertex3fv(float *ptr)
{
    vxStoreVertex3(ptr[0], ptr[1], ptr[2]);
}

void vxTexCoord2f(float x, float y)
{
    // Change format if neccesary
    if (vstate->verts == 0 && !(vstate->format & VARR_TEXCOORD0))
        vxBeginVertexArray(vstate->format | VARR_TEXCOORD0);
    
    vstate->cur_texcoord[0] = x;
    vstate->cur_texcoord[1] = y;
}

void vxTexCoord2fv(float *ptr)
{
    // Change format if neccesary
    if (vstate->verts == 0 && !(vstate->format & VARR_TEXCOORD0))
        vxBeginVertexArray(vstate->format | VARR_TEXCOORD0);
    
    vstate->cur_texcoord[0] = *ptr;
    vstate->cur_texcoord[1] = *(ptr+1);
}

void vxNormal3fv(float *ptr)
{
    // Change format if neccesary
    if (vstate->verts == 0 && !(vstate->format & VARR_NORMAL))
        vxBeginVertexArray(vstate->format | VARR_NORMAL);
    
    vstate->cur_normal[0] = *ptr;
    vstate->cur_normal[1] = *(ptr+1);
    vstate->cur_normal[2] = *(ptr+2);
}


void vxColor3f(float x, float y, float z)
{
    // Change format if neccesary
    if (vstate->verts == 0 && !(vstate->format & VARR_COLOR))
        vxBeginVertexArray(vstate->format | VARR_COLOR);
    
    vstate->cur_color[0] = x;
    vstate->cur_color[1] = y;
    vstate->cur_color[2] = z;
    vstate->cur_color[3] = 1.0;
}

void vxColor4f(float x, float y, float z, float a)
{
    // Change format if neccesary
    if (vstate->verts == 0 && !(vstate->format & (VARR_COLOR | VARR_ALPHA)))
        vxBeginVertexArray(vstate->format | (VARR_COLOR | VARR_ALPHA));
    
    vstate->cur_color[0] = x;
    vstate->cur_color[1] = y;
    vstate->cur_color[2] = z;
    vstate->cur_color[3] = a;
}

void vxColor3fv(float *ptr)
{
    // Change format if neccesary
    if (vstate->verts == 0 && !(vstate->format & VARR_COLOR))
        vxBeginVertexArray(vstate->format | VARR_COLOR);
    
    vstate->cur_color[0] = *ptr;
    vstate->cur_color[1] = *(ptr+1);
    vstate->cur_color[2] = *(ptr+2);
    vstate->cur_color[3] = 1.0;
}

void vxColor4fv(const float *ptr)
{
    // Change format if neccesary
    if (vstate->verts == 0 && !(vstate->format & (VARR_COLOR | VARR_ALPHA)))
        vxBeginVertexArray(vstate->format | (VARR_COLOR | VARR_ALPHA));
    
    vstate->cur_color[0] = *ptr;
    vstate->cur_color[1] = *(ptr+1);
    vstate->cur_color[2] = *(ptr+2);
    vstate->cur_color[3] = *(ptr+3);
    vstate->cur_color[3] = 1.0;
}

