//------------------------------------------------------------------------------
//
//	by RHL (robs-basement.de) 2007 - 2008
//
//	-> PLEASE READ THE README! <-
//
//------------------------------------------------------------------------------


#include <nds.h>
#include <nds/registers_alt.h>

#include <stdio.h>


#include "defines.hpp"
#include "types.hpp"
#include "scissorBox.hpp"
#include "mesh.hpp"


// all the "evil" globals needed in this unit
extern int8 g_nBrightness, g_nCapturemode;
extern bool g_bTextures, g_bShadows, g_bLight;

extern CMesh g_doll, g_shadow, g_room;
extern CView g_view;
extern CScissorBox g_scissor;


touchPosition g_currentTouch = {0, 0, 0, 0}, g_lastTouch = {0, 0, 0, 0};

/// @note If you want better input, use some sort of key mapping, instead of all those if/else branches!

void input(void)
{
	scanKeys();

	if((keysHeld() & KEY_L) && (keysHeld() & KEY_R))
	{
		if(keysDown() & KEY_UP)
		{
			if(++g_nBrightness > 4)
			{
				g_nBrightness = 4;
			}
		}

		if(keysDown() & KEY_DOWN)
		{
			if(--g_nBrightness < -4)
			{
				g_nBrightness = -4;
			}
		}

		if(g_nBrightness > 0)
		{
	       	BRIGHTNESS = g_nBrightness | BRIGHTNESS_INC;
		}
		else if(g_nBrightness < 0)
		{
	       	BRIGHTNESS = -g_nBrightness | BRIGHTNESS_DEC;
		}
		else
		{
	       	BRIGHTNESS = 0;
		}
	}
	else if(keysHeld() & KEY_L)
	{
		if(keysHeld() & KEY_LEFT)
		{
			g_view.x -= sinLerp(64 * (g_view.angleY + 128)) >> 7;
			g_view.z += cosLerp(64 * (g_view.angleY + 128)) >> 7;
		}

		if(keysHeld() & KEY_RIGHT)
		{
			g_view.x += sinLerp(64 * (g_view.angleY + 128)) >> 7;
			g_view.z -= cosLerp(64 * (g_view.angleY + 128)) >> 7;
		}

		if(keysHeld() & KEY_UP)
		{
			g_view.y -= 16;
		}

		if(keysHeld() & KEY_DOWN)
		{
			g_view.y += 16;
		}
	}
	else if(keysHeld() & KEY_R)
	{
		if(keysHeld() & KEY_LEFT)
		{
			g_scissor.centerX -= 1;
		}

		if(keysHeld() & KEY_RIGHT)
		{
			g_scissor.centerX += 1;
		}

		if(keysHeld() & KEY_UP)
		{
			g_scissor.centerY -= 1;
		}

		if(keysHeld() & KEY_DOWN)
		{
			g_scissor.centerY += 1;
		}
	}
	else
	{
		if(keysHeld() & KEY_LEFT)
		{
			g_view.x -= sinLerp(64 * (g_view.angleY + 128)) >> 7;
			g_view.z += cosLerp(64 * (g_view.angleY + 128)) >> 7;
		}

		if(keysHeld() & KEY_RIGHT)
		{
			g_view.x += sinLerp(64 * (g_view.angleY + 128)) >> 7;
			g_view.z -= cosLerp(64 * (g_view.angleY + 128)) >> 7;
		}

		if(keysHeld() & KEY_UP)
		{
			g_view.x += sinLerp(64 * g_view.angleY) >> 7;
			g_view.z -= cosLerp(64 * g_view.angleY) >> 7;
		}

		if(keysHeld() & KEY_DOWN)
		{
			g_view.x -= sinLerp(64 * g_view.angleY) >> 7;
			g_view.z += cosLerp(64 * g_view.angleY) >> 7;
		}
	}


	if(keysHeld() & KEY_SELECT)
	{
	 	if(keysDown() & KEY_X)
		{
	       g_bLight = !g_bLight;
		}

		if(keysDown() & KEY_Y)
		{
			g_nCapturemode = !g_nCapturemode;
		}

		if(keysDown() & KEY_A)
		{
	       g_bShadows = !g_bShadows;
		}

		if(keysDown() & KEY_B)
		{
	       g_bTextures = !g_bTextures;
		}
	}
	else
	{
		if(keysHeld() & KEY_X)
		{
			g_doll.z -= 16;
		}

		if(keysHeld() & KEY_B)
		{
			g_doll.z += 16;
		}

		if(keysHeld() & KEY_Y)
		{
			g_doll.x -= 16;
		}

		if(keysHeld() & KEY_A)
		{
			g_doll.x += 16;
		}
	}

	if(keysHeld() & KEY_START)
	{
		g_view.x = 0;
		g_view.y = -640;
		g_view.z = 2048;

		g_view.angleX = 0;
		g_view.angleY = 0;

		g_doll.x = 0;
		g_doll.y = 0;
		g_doll.z = 0;
		g_doll.ay = 0;

		g_scissor.centerX = 128;
		g_scissor.centerY = 96;
		g_scissor.width = 256;
		g_scissor.height = 192;

		g_nBrightness = 0;
		g_nCapturemode = 1;
		g_bTextures = g_bShadows = g_bLight = true;
	}


	// Camera rotation by touch screen

	if(keysDown() & KEY_TOUCH)
	{
		g_lastTouch = touchReadXY();

		// let's use some fixed point magic to improve touch accuracy
		g_lastTouch.px <<= 7;
		g_lastTouch.py <<= 7;
	}

	if(keysHeld() & KEY_TOUCH)
	{
		g_currentTouch = touchReadXY();

		// let's use some fixed point magic to improve touch smoothing accuracy
		g_currentTouch.px <<= 7;
		g_currentTouch.py <<= 7;

		int16 dx = (g_currentTouch.px - g_lastTouch.px) >> 7;
		int16 dy = (g_currentTouch.py - g_lastTouch.py) >> 7;

		// filtering too long strokes, if needed
//		if((dx < 30) && (dy < 30) && (dx > -30) && (dy > -30))
		{
			// filter too small strokes, if needed
//			if((dx > -2) && (dx < 2))
//				dx = 0;

			g_view.angleY += dx / 2;

			// filter too small strokes, if needed
//			if((dy > -1) && (dy < 1))
//				dy = 0;

			g_view.angleX -= dy / 2;
		}

		// some simple averaging / smoothing through weightened (.5 + .5) accumulation
		g_lastTouch.px = (g_lastTouch.px + g_currentTouch.px) / 2;
		g_lastTouch.py = (g_lastTouch.py + g_currentTouch.py) / 2;
	}
}
