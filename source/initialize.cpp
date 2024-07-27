//------------------------------------------------------------------------------
//
//	by RHL (robs-basement.de) 2007 - 2008
//
//	-> PLEASE READ THE README! <-
//
//------------------------------------------------------------------------------


#include <nds.h>
#include <nds/arm9/console.h>

#include <fat.h>

#include <stdio.h>
#include <string.h>

#include "defines.hpp"
#include "types.hpp"
#include "scissorBox.hpp"
#include "mesh.hpp"


// all the "evil" globals needed in this unit
extern int8 g_nBrightness, g_nCapturemode;
extern bool g_bTextures, g_bShadows, g_bLight;
extern uint32 g_nFramesDrawn, g_nVBlanks, g_nFPS;

extern CMesh g_doll, g_shadow, g_room;
extern CView g_view;
extern CScissorBox g_scissor;


void onVBlank(void);
void updateFogTable(uint32 mass);

/// @return All these functions return a Return Status Code!
uint32 loadTexture(const char *filename, int &textureID);
uint32 loadMeshChar(CMesh &mesh, const char *filename, float scale = 0.0156f, CRGBA color = c_white);
uint32 displayPBI(const char *filename, uint32 *screenBlock, uint8 loadPalette = 0);


// The following functions were stripped from initialize(), to improve modularity and readability.

static uint32 loadFont(void)
{
	FILE *pFile = NULL;
	uint8 *pBuffer = NULL;
	uint32 palSize = 0, width = 0, height = 0, i = 0, imgSize = 0;
	const char *filename = "font.pbi";

	// load the .pbi font image manually (because it needs "special treatment") ->
	pFile = fopen(filename, "rb");

	if(pFile == NULL)
	{
		// if not in the root, is it in the data directory?
		/// @todo FIX: This is a little bit hacky! Try chdir instead.
		char *path = new char[_MAX_PATH_LENGTH];
		if(path == NULL)
		{
			return RSC_ERROR_OOM;
		}
		memset(path, '\0', _MAX_PATH_LENGTH);
		strncpy(path, c_dataPath, c_dataPathLength);
		strncat(path, filename, _MAX_PATH_LENGTH - c_dataPathLength - 1);
		pFile = fopen(path, "rb");

		if(pFile == NULL)
		{
			// ERROR - file not found!
			consoleDemoInit();
			iprintf("\nCould not load font \"%s\"!\nPlease put all data in ROOT and try again.\n", path);
			_DEBUG_HALT
			// leave
			return RSC_ERROR_FNF;
		}

		delete [] path;
	}

	// load the header
	fread(&palSize, sizeof(uint32), 1, pFile);
	fread(&width, sizeof(uint32), 1, pFile);
	fread(&height, sizeof(uint32), 1, pFile);
	fseek(pFile, sizeof(uint32), SEEK_CUR); // skip some bytes
	palSize = min(palSize, 256);
	imgSize = width * height;

	// allocate maximum needed amount of memory
	// (this is a little trick, so you just need to allocate memory once per file)
	pBuffer = new uint8[max((palSize * sizeof(uint16)), imgSize)];

	if(pBuffer == NULL)
		return RSC_ERROR_OOM;

	// load the palette
	fread(pBuffer, sizeof(uint16), palSize, pFile);

	for(i = 0; i < palSize; i++)
		BG_PALETTE_SUB[i] = ((uint16 *)pBuffer)[i];

	// load the character set
	fread(pBuffer, sizeof(uint8), imgSize, pFile);

	consoleInit((uint16 *)pBuffer, (uint16 *)CHAR_BASE_BLOCK_SUB(1), 95, 32, (uint16 *)SCREEN_BASE_BLOCK_SUB(0), 0, 8);

	consoleClear();
	iprintf("\x1b[0;0H");
	iprintf("Initializing system... ");

	imgSize = width * height / (sizeof(uint32));

	for(i = 0; i < imgSize; i++)
	{
		((uint32 *)BG_TILE_RAM_SUB(1))[i] = ((uint32 *)pBuffer)[i];
	}

	// clean up
	delete [] pBuffer;
	pBuffer = NULL;

	palSize = width = height = i = imgSize = 0;

	fclose(pFile);
	pFile = NULL;

	return RSC_SUCCESS;
}

static uint32 loadTextures(void)
{
	uint32 returnCode = RSC_SUCCESS;

	iprintf("\x1b[1;0H                                \x1b[1;2H");
	iprintf("(textures");

	returnCode = loadTexture("doll256.tga", g_doll.texID);
	iprintf(".");

	returnCode |= loadTexture("test.tga", g_room.texID); // |= will result in anything but RSC_SUCCESS, if any error occurs
	iprintf(".");

	return returnCode;
}


static uint32 loadGeometry(void)
{
	uint32 returnCode = RSC_SUCCESS;

	iprintf("\x1b[1;0H                                \x1b[1;2H");
	iprintf("(scene");

	returnCode = loadMeshChar(g_doll, "doll.char");
	iprintf(".");

	returnCode |= loadMeshChar(g_shadow, "shadow.char", 0.0156f, CRGBA(8, 2, 2));
	iprintf(".");

	returnCode |= loadMeshChar(g_room, "room.char");
	iprintf(".");

	return returnCode;
}


/**
	@note This function has been excluded from main(), because most of the stack allocations (local variable) are only needed on initialization!

	Initialize the hardware, load content and prepare everything for program execution.
*/

void initialize(void)
{
	// Turn on everything and route the main engine to the top screen.
	powerON(POWER_ALL);
	lcdMainOnTop();

	// IRQ basic setup
	irqInit();
	irqSet(IRQ_VBLANK, 0);

	// assigne video ram banks
	vramSetBankA(VRAM_A_TEXTURE_SLOT0);
	vramSetBankB(VRAM_B_TEXTURE_SLOT1);
	vramSetBankC(VRAM_C_SUB_BG_0x06200000);
	vramSetBankD(VRAM_D_LCD);		// The captured image will be put here, so it *must* be set to LCD mode.

	vramSetBankE(VRAM_E_MAIN_BG);   // BEWARE: LIBNDS USES E FOR TEXTURE PALETTES BY DEFAULT
	vramSetBankF(VRAM_F_LCD);
	vramSetBankG(VRAM_G_LCD);
	vramSetBankH(VRAM_H_LCD);
	vramSetBankI(VRAM_I_LCD);

	// set the video modes and backgrounds
	videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
	BG3_CR = BG_BMP8_256x256 | BG_BMP_BASE(0) | BG_PRIORITY(0);

	BG3_XDX = 1 << 8;	//scale x
	BG3_XDY = 0;		//rotation x
	BG3_YDX = 0;		//rotation y
	BG3_YDY = 1 << 8;	//scale y
	BG3_CX = 0;			//translation x
	BG3_CY = 0;			//translation y

	videoSetModeSub(MODE_5_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG3_ACTIVE);
	SUB_BG0_CR = BG_32x32 | BG_256_COLOR | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_PRIORITY(0);
	SUB_BG3_CR = BG_BMP8_256x256 | BG_BMP_BASE(2) | BG_PRIORITY(3);

	SUB_BG3_XDX = 1 << 8;	//scale x
	SUB_BG3_XDY = 0;		//rotation x
	SUB_BG3_YDX = 0;		//rotation y
	SUB_BG3_YDY = 1 << 8;	//scale y
	SUB_BG3_CX = 0;			//translation x
	SUB_BG3_CY = 128;		//translation y

	// set the display brightness
	BRIGHTNESS = g_nBrightness;


	// initialize the file system
	if(!fatInitDefault())
	{
		// "halt" on error and display a message!
		consoleDemoInit();
		iprintf("FAT ERROR!\n");
		_DEBUG_HALT
	}

	loadFont();

	// display the backgrounds on both screens
	// main screen
	if(displayPBI("loading.pbi", ((uint32 *)BG_BMP_RAM(0)), 1) != RSC_SUCCESS)
	{
		_DEBUG_HALT
	}
	// sub screen
	if(displayPBI("loading.pbi", ((uint32 *)BG_BMP_RAM_SUB(2))) != RSC_SUCCESS)
	{
		_DEBUG_HALT
	}


	// set some default stuff, like clear depth etc.,
	// set our viewport to be the same size as the screen
	// and finally set the "clear plane's" color and alpha
	glInit();
	glViewport(0, 0, 255, 191);
	glClearColor(0, 25, 25, 31);


	iprintf("done.\n");
	iprintf("\x1b[0;0H                                \x1b[0;0H");
	iprintf("Loading data... \n");

	if(loadTextures() != RSC_SUCCESS)
	{
		_DEBUG_HALT
	}
	iprintf(")\n\x1b[1;0H                                \x1b[1;2H");

	if(loadGeometry() != RSC_SUCCESS)
	{
		_DEBUG_HALT
	}
	iprintf(")\n\x1b[1;0H                                \x1b[1;2H");

	iprintf("\x1b[0;16H");
	iprintf("done.\n");
	iprintf("\x1b[0;0H                                \x1b[0;0H");
	iprintf("\x1b[0;18HVSD Ver. 1.6.0");
	iprintf("\x1b[0;0HFPS:      ");
	iprintf("\x1b[1;0HVRT:      ");	// How many vertices are already in the vertex list ram? (6144 entries)
	iprintf("\x1b[2;0HPLY:      ");	// And what about the number of primitives? (2048 entries)
	iprintf("\x1b[3;0HRBL:      ");	// What's the minimum of the 48 available render buffer lines, that were free in the last frame? (more = better, max 46!)

	// sub screen again
	if(displayPBI("splash.pbi", ((uint32 *)BG_BMP_RAM_SUB(2))) != RSC_SUCCESS)
	{
		_DEBUG_HALT
	}

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

	g_nFramesDrawn = g_nVBlanks = 0;
	g_nFPS = 60;

	updateFogTable(12);

	// Prepare for shadow rendering (see end of draw() in draw.cpp)
	glFlush(GL_TRANS_MANUALSORT | GL_WBUFFERING);

	/// @note The frame buffer mode is needed for capturing to work with only one VRAM bank!
	// setup the main screen for 3D and single VRAM bank capturing (which all goes into VRAM bank D = FB3!)
	videoSetMode(MODE_FB3 | ENABLE_3D);
	BG0_CR = 0;
	BG3_CR = 0;

	irqSet(IRQ_VBLANK, onVBlank);
	irqEnable(IRQ_VBLANK);
}
