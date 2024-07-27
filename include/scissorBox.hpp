//------------------------------------------------------------------------------
//
//	by RHL (robs-basement.de) 2007 - 2008
//
//	-> PLEASE READ THE README! <-
//
//------------------------------------------------------------------------------


#ifndef __SCISSORBOX_HPP
#define __SCISSORBOX_HPP


/**
	@file scissorBox.hpp

	The scissor box test is used to cut out a section of the viewport and only render what is inside.
*/


class CScissorBox
{
public:
	int16 centerX, centerY;
	uint16 width, height;
	m4x4 savedProjectionMatrix;				// saved projection matrix

	/// These two are temporary variables for the class to use.
	static int32 s_viewportCutoff[4];		// how much is getting cut off because the viewport hit a screen edge
	static int32 s_viewportScissored[4];	// the final viewport after scissoring


	CScissorBox(void)
	{
		centerX = 128;
		centerY = 96;
		width = 256;
		height = 192;
	}

	CScissorBox(int16 centerX, int16 centerY, uint16 width, uint16 height)
	{
		this->centerX = centerX;
		this->centerY = centerY;
		this->width = width;
		this->height = height;
	}

	~CScissorBox(void)
	{
		centerX = 0;
		centerY = 0;
		width = 0;
		height = 0;
	}


	// most of these member functions should be inlined

	/**
		@fn save
		@note Call this *after* changing the projection matrix to what you need it to be and while still in projection matrix mode!
		Save the current projection matrix for later restoration.
	*/

	void save(void)
	{
		glGetFixed(GL_GET_MATRIX_PROJECTION, savedProjectionMatrix.m);
	}

	/**
		@fn scissor
		Scissor the viewport via the pick matrix, combined with the previously saved (via "update()") projection matrix.
	*/

	void scissor(void);

	/**
		@fn restore
		Restore the saved projection matrix.
	*/

	void restore(void)
	{
		glMatrixMode(GL_PROJECTION);
		glViewport(c_viewportDimensions[0], c_viewportDimensions[1], c_viewportDimensions[2], c_viewportDimensions[3]);
		glLoadMatrix4x4(&savedProjectionMatrix);
		glMatrixMode(GL_MODELVIEW);
	}
};


#endif
