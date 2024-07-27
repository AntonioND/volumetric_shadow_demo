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
	A simple check of the size parameter to see if it is a power of two.
*/

bool checkSize(uint32 size)
{
	uint32 i = 1;

	while(i < size)
		if((i <<= 1) == size)
			return(true);

	return(false);
}


/*
The .tga header:

typedef struct
{
	char  idlength;				// 0
	char  colourmaptype;			// 1
	char  datatypecode;			// 2 <- we only need this...
	short int colourmaporigin;	// 3/4
	short int colourmaplength;	// 5/6
	char  colourmapdepth;		// 7
	short int x_origin;			// 8/9
	short int y_origin;			// 10/11
	short width;					// 12/13 <- this...
	short height;				// 14/15 <- this...
	char  bitsperpixel;			// 16 <- and finally, this
	char  imagedescriptor;		// 17
} TGA_HEADER;


The header is "packed", so that sizeof(TGA_HEADER) should return exactly 18, not more, not less!

Notes on the image data stored in the .tga format:
Pixels of a 4 x 2 texture, that are displayed like that (each number represents a certain pixel):

	0123
	4567

are actually stored "upside down":

	4567
	0123

Also, due to the component write order RGB becomes BGR and RGBA becomes BGRA!
*/


/**
	Load a 24/32bit .tga texture image from a file and return the image data in .tga's B-G-R(-A) format.
*/

uint32 loadTargaImage(const char *filename, uint32 &width, uint32 &height, uint8 &bpp, uint8 *&pImageData)
{
	FILE *pFile = NULL;		// Mr. Filestream
	uint8 imageTypeCode = 0, bitCount = 0;
	uint16 imageWidth = 0, imageHeight = 0;
	uint32	nImageSize = 0, i = 0;
	uint8 header[18];		// space for the .tga header (allocated on the stack!)

	// open the TGA file
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
			iprintf("\nloadTargaImage() failed: File \"%s\"not found!\n", path);
			// leave
			return RSC_ERROR_FNF;
		}

		delete [] path;
	}

	// read in the header
	fread(header, sizeof(uint8), 18, pFile);

	imageTypeCode = header[2];

	// only loading 24bit RGB or 32bit RGBA types
	if((imageTypeCode != 2) && (imageTypeCode != 3))
	{
		// ERROR - unsupported image type!
		// clean up
		imageTypeCode = 0;
		for(i = 0; i < 18; i++)
			header[i] = 0;
		iprintf("\nloadTargaImage() failed: Unsupported image type!\n");
		// close the file
		fclose(pFile);
		pFile = NULL;
		// ... and leave
		return RSC_ERROR;
	}

    /// NOTE: the next two are short (16bit) ints!
	imageWidth = *((uint16 *)&header[12]);
	imageHeight = *((uint16 *)&header[14]);

	bitCount = header[16];

	bpp    = bitCount / 8;
	width  = (uint32)imageWidth;
	height = (uint32)imageHeight;

	if(!checkSize(width) || !checkSize(height))
	{
		// ERROR - not power of two!
		// clean up
		imageTypeCode = 0;
		for(i = 0; i < 18; i++)
			header[i] = 0;
		imageWidth = 0;
		imageHeight = 0;
		bitCount = 0;
		bpp    = 0;
		width  = height = 0;
		iprintf("\nloadTargaImage() failed : Not power of two!\n");
		// close the file
		fclose(pFile);
		pFile = NULL;
		// ... and leave
		return RSC_ERROR;
	}

	nImageSize = width * height * bpp;

	if(pImageData != NULL)
	{
		delete [] pImageData;
		pImageData = NULL;
	}

	pImageData = new uint8[nImageSize];

	if(pImageData == NULL)
	{
		// ERROR - out of memory!
		// clean up
		imageTypeCode = 0;
		for(i = 0; i < 18; i++)
			header[i] = 0;
		imageWidth = 0;
		imageHeight = 0;
		bitCount = 0;
		bpp    = 0;
		width  = height = 0;
		nImageSize = 0;
		iprintf("\nloadTargaImage() failed: Out of memory! (pImage)\n");
		// close the file
		fclose(pFile);
		pFile = NULL;
		// ... and leave
		return RSC_ERROR_OOM;
	}

	// read the image data
	fread(pImageData, sizeof(uint8), nImageSize, pFile);

	// clean up and return the image data
	imageTypeCode = 0;
	for(i = 0; i < 18; i++)
		header[i] = 0;
	imageWidth = 0;
	imageHeight = 0;
	bitCount = 0;
	nImageSize = 0;

	// close the file
	fclose(pFile);
	pFile = NULL;

	// finally return success!
	return RSC_SUCCESS;
}
