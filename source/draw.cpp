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
#include "scissorBox.hpp"
#include "mesh.hpp"


#define GL_FOG_SHIFT(n)			((n) << 8)
#define FIX_GFX_RDLINES_COUNT	(*(uint8*)0x04000320) // Rendered Line Count Register (from gbatek - not yet defined in libnds)


// all the "evil" globals needed in this unit
extern int8 g_nCapturemode;
extern bool g_bTextures, g_bShadows, g_bLight;
extern uint32 g_nFramesDrawn, g_nFPS;

extern CMesh g_doll, g_shadow, g_room;
extern CView g_view;
extern CScissorBox g_scissor;

/// Set this to whatever you need it to be.
static const uint16 c_always_glEnabled = GL_TOON_HIGHLIGHT | GL_ANTIALIAS | GL_BLEND | GL_COLOR_UNDERFLOW | GL_POLY_OVERFLOW | GL_FOG | GL_FOG_SHIFT(9);


static inline void glFogColor(uint8 red, uint8 green, uint8 blue, uint8 alpha)
{
	GFX_FOG_COLOR = ((alpha & 31) << 16) | ((blue & 31) << 10) | ((green & 31) << 5) | (red & 31);
}

static inline void glFogDepth(uint16 depth)
{
	GFX_FOG_OFFSET = depth;
}


void updateFogTable(uint32 mass)
{
	int32 density = -2 * mass;	// ... because we need a 0 in the first fog table entry!

	glFogColor(8, 12, 15, 7);
	glFogDepth(2);

	for(uint32 i = 0; i < 32; i++)
	{
		density += 2 * mass;
		density = min(density, 127);	// entries are 7bit, so cap the density to 127
		GFX_FOG_TABLE[i] = density;
	}
}


// This was stripped from draw() to improve readability. No new variables etc. are declared here, so it should be absolutely save to inline, without affecting the stack.

static inline void drawScene(void)
{
	glRotatef32i(g_view.angleX, (1 << 12), 0, 0);
	glRotatef32i(LUT_SIZE - g_view.angleY, 0, (1 << 12), 0);
	glTranslate3f32(-g_view.x, g_view.y, -g_view.z);

	/// @note remember to set lights AFTER the view has been rotated!
	if(g_bLight)
	{
		glLight(0, RGB15(31, 31, 31), floattov10(-1.0), floattov10(-1.0), floattov10(-1.0));
	}

	g_scissor.scissor();

	// Material properties need to be set, since the NDS does not have them set by default. This creates Graphics FIFO commands!
	glMaterialf(GL_DIFFUSE, RGB15(31, 31, 31) | BIT(15)); /// Bit 15 enables the diffuse color to act like being set with glColor(), only with lighting support. When not using lighting, this is going to be the default color, just like being set with glColor().
	glMaterialf(GL_AMBIENT, RGB15(4, 4, 5));
	glMaterialf(GL_SPECULAR, RGB15(0, 0, 0)); /// Bit 15 would have to be set here to enable a custom specularity table, instead of the default linear one.
	glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

	// draw all the regular geometry
	g_room.draw(4, RM_FILL);

	g_doll.ay += 2;
	g_doll.draw(5, RM_FILL);

	// now draw the shadows, if desired
	if(g_bShadows)
	{
	glMaterialf(GL_DIFFUSE, RGB15(0, 0, 2) | BIT(15)); // let's make it slightly blue
	glMaterialf(GL_AMBIENT, RGB15(0, 0, 0));
		g_shadow.draw(0, RM_MASK);
		g_shadow.draw(0, RM_SHAD);
	}

	g_scissor.restore();

	// finish drawing, flush to screen and swap (geometry) buffers
	/// @note VERY IMPORTANT: MANUAL SORT (bit 0 = 1) WITH W-BUFFERING (bit 2 = 1) or else you'll get mixed up geometry with shadows or even a blank screen!
	glFlush(GL_TRANS_MANUALSORT | GL_WBUFFERING);
}


void draw(void)
{
	glResetMatrixStack();

	glEnable(c_always_glEnabled);

	if(g_bTextures)
	{
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspectivef32(80, (unsigned int)((256.0 / 192.0) * (1 << 12)), 16, (64 * 256));

	// The next one has to be called while in projection matrix mode!
	g_scissor.save();

	// setup capturing
	REG_DISPCAPCNT =
		DCAP_ENABLE |
		DCAP_MODE(g_nCapturemode << 1) |	// Capture Source    (0=Source A, 1=Source B, 2/3=Sources A+B blended)
		DCAP_DST(0) |						// VRAM Write Offset (0=00000h, 0=08000h, 0=10000h, 0=18000h)
		DCAP_SRC(1) |						// Source A          (0=Graphics Screen BG+3D+OBJ, 1=3D Screen)
		DCAP_SRC(0 << 1) |					// Source B          (0=VRAM, 1=Main Memory Display FIFO)
		DCAP_SIZE(3) |						// Capture Size      (0=128x128, 1=256x64, 2=256x128, 3=256x192 dots)
		DCAP_OFFSET(0) |					// VRAM Read Offset  (0=00000h, 0=08000h, 0=10000h, 0=18000h)
		DCAP_BANK(3) |						// VRAM Write Block  (0..3 = VRAM A..D) (VRAM must be allocated to LCDC)
		DCAP_B(8) |							// EVB (0..16, or 0..31 ?) Blending Factor for Source A (or B ?)
		DCAP_A(8);							// EVA (0..16, or 0..31 ?) Blending Factor for Source B (or A ?)


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// With the following you can create some pseudo blooming / depth of field ect. effects, if you like to.

	// Example: Quick and dirty depth of field (needs some more accuracy to look good, though!)
//	if(g_nCapturemode)
//	{
//		switch(g_nFramesDrawn % 4)
//		{
//			case 0:
//				glRotatef32i((1 << 0), (1 << 6), 0, 0);
//				break;
//			case 1:
//				glRotatef32i((1 << 0), 0, (1 << 6), 0);
//				break;
//			case 2:
//				glRotatef32i(-(1 << 0), (1 << 6), 0, 0);
//				break;
//			case 3:
//				glRotatef32i(-(1 << 0), 0, (1 << 6), 0);
//				break;
//		}
//	}

	drawScene();

	g_nFramesDrawn++;

	iprintf("\x1b[0;5H    \x1b[0;5H%i\n", g_nFPS);
	iprintf("\x1b[1;5H    \x1b[1;5H%i\n", GFX_VERTEX_RAM_USAGE);
	iprintf("\x1b[2;5H    \x1b[2;5H%i\n", GFX_POLYGON_RAM_USAGE);
	iprintf("\x1b[3;5H    \x1b[3;5H%i\n", FIX_GFX_RDLINES_COUNT);
}
