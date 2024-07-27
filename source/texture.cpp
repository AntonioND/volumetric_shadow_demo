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


/// @note A pointer to the image data will be returned in the reference to the uint8 pointer pImageData!
/// @return The function returns a Return Status Code!
uint32 loadTargaImage(const char *filename, uint32 &width, uint32 &height, uint8 &bpp, uint8 *&pImageData);


/**
	Small helper function that "calculates" a texture size (0 to 7) from a given pixel size value.
*/

static GL_TEXTURE_SIZE_ENUM calcTextureSize(uint32 size)
{
	uint32 i = 0, j = 0;

	while(j < size && i++ < 8)
	{
		if((j = (8 << i)) == size)
			return (GL_TEXTURE_SIZE_ENUM)i;
	}

	// Unsupported size!
//	return RSC_ERROR; <- That could be returned too, but for simplicity let's return the following instead:
	return TEXTURE_SIZE_8;
}


/**
	Load an RGB texture from a file (currently only .tga is supported) and automaticaly generate and assign a texture object.
*/

uint32 loadTexture(const char *filename, int &textureID)
{
	uint32 width = 0, height = 0, t = 0, returnCode = RSC_SUCCESS;
	uint8 bpp = 0, *pSData = NULL, r = 0, g = 0, b = 0, a = 0;
	uint16 *pTexData = NULL;
	GL_TEXTURE_SIZE_ENUM uSize = TEXTURE_SIZE_8, vSize = TEXTURE_SIZE_8;

	returnCode = loadTargaImage(filename, width, height, bpp, pSData);

	if(returnCode != RSC_SUCCESS)
	{
		// something went wrong
//		printf("\x1b[2;2H");
		printf("\nloadTexture() failed: Texture \"%s\" did not load!\n", filename);
		return returnCode;
	}

	// No error checking is performed here, but could be added, if needed.
	uSize = calcTextureSize(width);
	vSize = calcTextureSize(height);

	pTexData = new uint16[width * height];

	if(pTexData == NULL)
	{
		printf("\x1b[2;2H");
		printf("\nloadTexture() failed: pTexData OOM!\n");
		// clean up
		delete [] pSData;
		pSData = NULL;
		returnCode = 0;
		return RSC_ERROR_OOM;
	}

	// convert 24bit data to 16bit data
	for(t = 0; t < width * height; t++)
	{
		b = (pSData[t * bpp + 0] / 8) & 31;
		g = (pSData[t * bpp + 1] / 8) & 31;
		r = (pSData[t * bpp + 2] / 8) & 31;
		a = (1);

		pTexData[t] = RGB15(r, g, b) | (a << 15);
	}

	glGenTextures(1, &textureID);
	glBindTexture(0, textureID);
	glTexImage2D(0, 0, GL_RGB, uSize , vSize, 0, TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T, (uint8 *)pTexData);

	// clean up
	delete [] pTexData;
	pTexData = NULL;
	delete [] pSData;
	pSData = NULL;
	return (returnCode = RSC_SUCCESS);
}
