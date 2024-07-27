//------------------------------------------------------------------------------
//
//	by RHL (robs-basement.de) 2007 - 2008
//
//	-> PLEASE READ THE README! <-
//
//------------------------------------------------------------------------------


#include <nds.h>
#include <nds/arm9/console.h>

#include <stdio.h>


#include "defines.hpp"
#include "types.hpp"
#include "mesh.hpp"


// all the "evil" globals needed in this unit
extern bool g_bLight, g_bTextures;

static bool s_bMaskPoly = false, s_bShadowPoly = false;
static uint32 s_nPolyFormat = 0;


void CMesh::draw(uint8 shift, ERenderMode mode)
{
	glPushMatrix();
	glTranslate3f32(x, y, z);
	glRotatef32i(ax, (1 << 12), 0, 0);
	glRotatef32i(ay, 0, (1 << 12), 0);
	glRotatef32i(az, 0, 0, (1 << 12));

	if(g_bTextures)
		glBindTexture(GL_TEXTURE_2D, texID);

	// Set the polygon format and enable states according to the render mode.
	switch(mode)
	{
		case RM_FILL:
			s_nPolyFormat = POLY_ALPHA(31) | POLY_CULL_BACK | POLY_ID(1) | POLY_FOG;
			s_bMaskPoly = s_bShadowPoly = false;
			break;

		case RM_WIRE:
			s_nPolyFormat = POLY_ALPHA(0) | POLY_CULL_BACK | POLY_ID(1);
			s_bMaskPoly = s_bShadowPoly = false;
			break;

		// all shadow primitives *MUST* be translutient!
		// sort your translutient objects prior to rendering!
		case RM_MASK:
			/// @note Remember to call glEnable(GL_BLEND); before rendering shadows!
			// Enable "masking" primitives by setting bits 4 and 5 (shadow poly mode). POLY_ID must be 0 and alpha must *not* be 0 or 31! ...
			s_nPolyFormat = POLY_ALPHA(1) | POLY_CULL_FRONT | POLY_ID(0) | POLY_SHADOW;
			s_bMaskPoly = true;
			s_bShadowPoly = false;
			break;

		case RM_SHAD:
			/// @note Remember to call glEnable(GL_BLEND); before rendering shadows!
			// ... The same applies to the "shadowing" primitives, but this time POLY_ID must not be 0!
			s_nPolyFormat = POLY_ALPHA(20) | POLY_CULL_BACK | POLY_ID(63) | POLY_SHADOW | POLY_FOG;
			s_bMaskPoly = false;
			s_bShadowPoly = true;
			break;
	}

	if(g_bLight && !(s_bMaskPoly || s_bShadowPoly))
	{
		// Add the light parameters to the polygon format and set it
		glPolyFmt(s_nPolyFormat | POLY_FORMAT_LIGHT0);

		for(int loop_m = 0; loop_m < nTris; loop_m++)
		{
			glBegin(GL_TRIANGLES);
				glNormal(NORMAL_PACK(pTris[loop_m].vertex[0].normal[0], pTris[loop_m].vertex[0].normal[1], pTris[loop_m].vertex[0].normal[2]));
				glTexCoord2t16(pTris[loop_m].texCoordU[0] << shift, pTris[loop_m].texCoordV[0] << shift);
				glVertex3v16(pTris[loop_m].vertex[0].x, pTris[loop_m].vertex[0].y, pTris[loop_m].vertex[0].z);

				glNormal(NORMAL_PACK(pTris[loop_m].vertex[1].normal[0], pTris[loop_m].vertex[1].normal[1], pTris[loop_m].vertex[1].normal[2]));
				glTexCoord2t16(pTris[loop_m].texCoordU[1] << shift, pTris[loop_m].texCoordV[1] << shift);
				glVertex3v16(pTris[loop_m].vertex[1].x, pTris[loop_m].vertex[1].y, pTris[loop_m].vertex[1].z);

				glNormal(NORMAL_PACK(pTris[loop_m].vertex[2].normal[0], pTris[loop_m].vertex[2].normal[1], pTris[loop_m].vertex[2].normal[2]));
				glTexCoord2t16(pTris[loop_m].texCoordU[2] << shift, pTris[loop_m].texCoordV[2] << shift);
				glVertex3v16(pTris[loop_m].vertex[2].x, pTris[loop_m].vertex[2].y, pTris[loop_m].vertex[2].z);
			glEnd();
		}
	}
	else
	{
		// We have already set all needed parameters, so just set the polygon format
		glPolyFmt(s_nPolyFormat);

		for(int loop_m = 0; loop_m < nTris; loop_m++)
		{
			glBegin(GL_TRIANGLES);
				glTexCoord2t16(pTris[loop_m].texCoordU[0] << shift, pTris[loop_m].texCoordV[0] << shift);
				glVertex3v16(pTris[loop_m].vertex[0].x, pTris[loop_m].vertex[0].y, pTris[loop_m].vertex[0].z);

				glTexCoord2t16(pTris[loop_m].texCoordU[1] << shift, pTris[loop_m].texCoordV[1] << shift);
				glVertex3v16(pTris[loop_m].vertex[1].x, pTris[loop_m].vertex[1].y, pTris[loop_m].vertex[1].z);

				glTexCoord2t16(pTris[loop_m].texCoordU[2] << shift, pTris[loop_m].texCoordV[2] << shift);
				glVertex3v16(pTris[loop_m].vertex[2].x, pTris[loop_m].vertex[2].y, pTris[loop_m].vertex[2].z);
			glEnd();
		}
	}

	glPopMatrix(1);
}
