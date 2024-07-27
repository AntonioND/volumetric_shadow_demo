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
#include "mesh.hpp"


/**
	Load a .char (ASCII) mesh... simple. :^)
*/

uint32 loadMeshChar(CMesh &mesh, const char *filename, float scale, CRGBA color)
{
	FILE *pFile = NULL;
	pFile = fopen(filename, "r");

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
		pFile = fopen(path, "r");

		if(pFile == NULL)
		{
			// ERROR - file not found!
			printf("\nloadMeshChar() failed: File not found!\n");
			// leave
			return RSC_ERROR_FNF;
		}

		delete [] path;
	}

	uint32 i = 0, nv = 0, nt = 0;
	float **ppVData = NULL;

	char *line = new char[255];
	line[0] = '\0';

	// skip the first line!
	fgets(line, 256, pFile);

	//
	// vertices
	//

	fgets(line, 256, pFile);
	sscanf(line, "%lu %lu", &nv, &nt);

	ppVData = new float*[nv];

	if (ppVData == NULL)
	{
		return RSC_ERROR_OOM;     // out of memory!
	}

	for (i = 0; i < nv; i++)
	{
		ppVData[i] = new float[6];

		if (ppVData[i] == NULL)
		{
			for (i = 0; i < nv; i++)
			{
				if (ppVData[i] != NULL)
				{
					delete [] ppVData[i];
				}

				ppVData[i] = NULL;
			}

			delete [] ppVData;
			ppVData = NULL;

			return RSC_ERROR_OOM;
		}
	}

	for (i = 0; i < nv; i++)
	{
		fgets(line, 256, pFile);
		sscanf(line, "%f %f %f %f %f %f", &ppVData[i][0], &ppVData[i][1], &ppVData[i][2], &ppVData[i][3], &ppVData[i][4], &ppVData[i][5]);
	}

	//
	// triangles
	//

	int32 iTemp[3];
	float fTemp[9];

	mesh.nTris = nt;
	mesh.pTris = new CTriangle[mesh.nTris];

	for(i = 0; i < mesh.nTris; i++)
	{
		fgets(line, 256, pFile);
		sscanf(line, "%ld %ld %ld %f %f %f %f %f %f %f %f %f",
			&iTemp[0], &iTemp[1], &iTemp[2],
			&fTemp[0], &fTemp[1], &fTemp[2],
			&fTemp[3], &fTemp[4],
			&fTemp[5], &fTemp[6],
			&fTemp[7], &fTemp[8]);

		mesh.pTris[i].vertex[0].x = floattov16(ppVData[iTemp[0]][0] * scale);
		mesh.pTris[i].vertex[0].y = floattov16(ppVData[iTemp[0]][1] * scale);
		mesh.pTris[i].vertex[0].z = floattov16(ppVData[iTemp[0]][2] * scale);
		mesh.pTris[i].vertex[0].normal[0] = floattov10(ppVData[iTemp[0]][3]);
		mesh.pTris[i].vertex[0].normal[1] = floattov10(ppVData[iTemp[0]][4]);
		mesh.pTris[i].vertex[0].normal[2] = floattov10(ppVData[iTemp[0]][5]);
		mesh.pTris[i].texCoordU[0] = floattot16(fTemp[3] * 8.0f);
		mesh.pTris[i].texCoordV[0] = floattot16(fTemp[4] * 8.0f);

		mesh.pTris[i].vertex[1].x = floattov16(ppVData[iTemp[1]][0] * scale);
		mesh.pTris[i].vertex[1].y = floattov16(ppVData[iTemp[1]][1] * scale);
		mesh.pTris[i].vertex[1].z = floattov16(ppVData[iTemp[1]][2] * scale);
		mesh.pTris[i].vertex[1].normal[0] = floattov10(ppVData[iTemp[1]][3]);
		mesh.pTris[i].vertex[1].normal[1] = floattov10(ppVData[iTemp[1]][4]);
		mesh.pTris[i].vertex[1].normal[2] = floattov10(ppVData[iTemp[1]][5]);
		mesh.pTris[i].texCoordU[1] = floattot16(fTemp[5] * 8.0f);
		mesh.pTris[i].texCoordV[1] = floattot16(fTemp[6] * 8.0f);

		mesh.pTris[i].vertex[2].x = floattov16(ppVData[iTemp[2]][0] * scale);
		mesh.pTris[i].vertex[2].y = floattov16(ppVData[iTemp[2]][1] * scale);
		mesh.pTris[i].vertex[2].z = floattov16(ppVData[iTemp[2]][2] * scale);
		mesh.pTris[i].vertex[2].normal[0] = floattov10(ppVData[iTemp[2]][3]);
		mesh.pTris[i].vertex[2].normal[1] = floattov10(ppVData[iTemp[2]][4]);
		mesh.pTris[i].vertex[2].normal[2] = floattov10(ppVData[iTemp[2]][5]);
		mesh.pTris[i].texCoordU[2] = floattot16(fTemp[7] * 8.0f);
		mesh.pTris[i].texCoordV[2] = floattot16(fTemp[8] * 8.0f);

		mesh.pTris[i].normal[0] = floattov10(fTemp[0]);
		mesh.pTris[i].normal[1] = floattov10(fTemp[1]);
		mesh.pTris[i].normal[2] = floattov10(fTemp[2]);

		mesh.pTris[i].color = color;
	}

	// clean up
	for (i = 0; i < nv; i++)
	{
		delete [] ppVData[i];
		ppVData[i] = NULL;
	}

	delete [] ppVData;
	ppVData = NULL;
	i = nv = nt = 0;
	fclose(pFile);
	pFile = NULL;
	return RSC_SUCCESS;
}
