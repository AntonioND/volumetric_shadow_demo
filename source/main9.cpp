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
#include <string.h>


#include "defines.hpp"
#include "types.hpp"
#include "scissorBox.hpp"
#include "mesh.hpp"


const int c_viewportDimensions[] = {0, 0, 255, 191};

const char *c_dataPath = "fat:/data/vsd_data/";
const uint8 c_dataPathLength = strlen(c_dataPath); // yes, this actually works!


/// @note Lots of "bad" global variables! Try to encapsulate them in some structures, whenever possible.

uint32 g_nFramesDrawn = 0, g_nVBlanks = 0, g_nFPS = 0;
int8 g_nBrightness = 0, g_nCapturemode = 1;
bool g_bTextures = true, g_bShadows = true, g_bLight = true;

CMesh g_doll, g_shadow, g_room;
CView g_view;
CScissorBox g_scissor;


void initialize(void);
void input(void);
void draw(void);


/**
	The vertical blank interrupt handler
*/

void onVBlank(void)
{
	// compute the frame rate
	if(++g_nVBlanks == 60)
	{
		g_nFPS = g_nFramesDrawn;
		g_nFramesDrawn = 0;
		g_nVBlanks = 0;
	}
}


int main(int argc, char *argv[])
{
	initialize();

	while(true)
	{
		input();
		draw();
		swiWaitForVBlank();
	}

	return RSC_SUCCESS;
}
