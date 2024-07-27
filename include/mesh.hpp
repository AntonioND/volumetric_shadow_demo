//------------------------------------------------------------------------------
//
//	by RHL (robs-basement.de) 2007 - 2008
//
//	-> PLEASE READ THE README! <-
//
//------------------------------------------------------------------------------


#ifndef __MESH_HPP
#define __MESH_HPP


typedef enum RENDERMODE_ENUM
{
	RM_FILL,
	RM_WIRE,
	RM_MASK,
	RM_SHAD,
}	ERenderMode;


class CVertex
{
public:
	v16		x, y, z;
	v10		normal[3];
	CRGBA	color;

	CVertex(void)
	{
		x = y = z = 0;
		normal[0] = normal[1] = normal[2] = 0;
		color = c_white;
	}

	~CVertex(void)
	{
		x = y = z = 0;
		normal[0] = normal[1] = normal[2] = 0;
		color = c_black;
	}
};


class CTriangle
{
public:
	// this is not a good example - use a vertex array (in the mesh) and vertex indices instead
	CVertex	vertex[3];
	t16		texCoordU[3], texCoordV[3];
	v10		normal[3];
	CRGBA	color;

	CTriangle(void)
	{
		texCoordU[0] = texCoordU[1] = texCoordU[2] = 0;
		texCoordV[0] = texCoordV[1] = texCoordV[2] = 0;
		normal[0] = normal[1] = normal[2] = 0;
		color = c_white;
	}

	~CTriangle(void)
	{
		texCoordU[0] = texCoordU[1] = texCoordU[2] = 0;
		texCoordV[0] = texCoordV[1] = texCoordV[2] = 0;
		normal[0] = normal[1] = normal[2] = 0;
		color = c_black;
	}
};


/// @note This is a highly over-symplified mesh class, don't use it in real projects!

class CMesh
{
protected:
	CTriangle	*pTris;
	uint16		nTris;

	friend uint32 loadMeshChar(CMesh &mesh, const char *filename, float scale, CRGBA color);

public:
	int32 		x, y, z;		// position - use a vector instead!
	int32 		ax, ay, az;		// euler angles - use a vector / quaternion / matrix instead!
	int			texID;			// texture slot id

	CMesh(void)
	{
		x = y = z = ax = ay = az = 0;
		texID = 0;
		nTris = 0;
		pTris = NULL;
	}

	~CMesh(void)
	{
		x = y = z = ax = ay = az = 0;
		texID = 0;
		nTris = 0;

		if(pTris)
			delete [] pTris;
		pTris = NULL;
	}

	/// @param shift controlls the texture coordinates via the texture size (e.g. for texture lod)
	void draw(uint8 shift, ERenderMode mode);
};


#endif	// __MESH_HPP
