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


/**
	This function displays a Paletted Binary Image file by loading it into a propperly setup hardware background

	@note Depending on loadPalette's setting, the palette will be loaded as follows: 0 - skip, 1 - main, else - sub
*/

uint32 displayPBI(const char *filename, uint32 *screenBlock, uint8 loadPalette)
{
	FILE *pFile = NULL;
	uint8 *pBuffer = NULL;
	uint16 *pPaletteMemory = NULL;
	uint32 palSize = 0, width = 0, height = 0, i = 0, imgSize = 0;

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
			printf("\ndisplayPBI() failed: File \"%s\" not found!\n", path);
			// leave
			return RSC_ERROR_FNF;
		}

		delete [] path;
	}

	// read the header
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
	{
		return(RSC_ERROR_OOM);
	}

	if(loadPalette == 0)
	{
		// skip the palette
		fseek(pFile, palSize * sizeof(uint16), SEEK_CUR);
	}
	else
	{
		// where will the palette be written to?
		if(loadPalette == 1)
			pPaletteMemory = BG_PALETTE;
		else
			pPaletteMemory = BG_PALETTE_SUB;

		// load the palette to main memory...
		fread(pBuffer, sizeof(uint16), palSize, pFile);

		// and copy it to the palette memory
		for(i = 0; i < palSize; i++)
		{
			pPaletteMemory[i] = ((uint16 *)pBuffer)[i];
		}
	}

	// load the image to main memory...
	fread(pBuffer, sizeof(uint8), imgSize, pFile);

	// and copy it to the graphics memory
	for(i = 0; i < (imgSize / sizeof(uint32)); i++)
	{
		screenBlock[i] = ((uint32 *)pBuffer)[i];
	}

	// free the memory buffer
	delete [] pBuffer;
	pBuffer = NULL;

	// close the file and clean up
	fclose(pFile);
	pFile = NULL;
	pPaletteMemory = NULL;
	palSize = 0, width = 0, height = 0, i = 0, imgSize = 0;
	return RSC_SUCCESS;
}
