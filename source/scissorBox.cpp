//------------------------------------------------------------------------------
//
//	by RHL (robs-basement.de) 2007 - 2008
//
//	-> PLEASE READ THE README! <-
//
//------------------------------------------------------------------------------


#include <nds.h>

#include <stdio.h>


#include "defines.hpp"
#include "types.hpp"
#include "scissorBox.hpp"


int32 CScissorBox::s_viewportCutoff[4];
int32 CScissorBox::s_viewportScissored[4];


void CScissorBox::scissor(void)
{
	s_viewportScissored[0] = centerX - (width / 2);
	s_viewportScissored[1] = (191 - centerY) - (height / 2);
	s_viewportScissored[2] = centerX + (width / 2);
	s_viewportScissored[3] = (191 - centerY) + (height / 2);

	if(s_viewportScissored[0] < 0)
	{
		s_viewportCutoff[0] = -s_viewportScissored[0];
		s_viewportScissored[0] = 0;
	}
	else if(s_viewportScissored[0] > 255)
	{
		s_viewportCutoff[0] = s_viewportScissored[0] - 255;
		s_viewportScissored[0] = 255;
	}
	else
	{
		s_viewportCutoff[0] = 0;
	}

	if(s_viewportScissored[1] < 0)
	{
		s_viewportCutoff[1] = -s_viewportScissored[1];
		s_viewportScissored[1] = 0;
	}
	else if(s_viewportScissored[1] > 191)
	{
		s_viewportCutoff[1] = s_viewportScissored[1] - 191;
		s_viewportScissored[1] = 191;
	}
	else
	{
		s_viewportCutoff[1] = 0;
	}

	if(s_viewportScissored[2] < 0)
	{
		s_viewportCutoff[2] = -s_viewportScissored[2];
		s_viewportScissored[2] = 0;
	}
	else if(s_viewportScissored[2] > 255)
	{
		s_viewportCutoff[2] = s_viewportScissored[2] - 255;
		s_viewportScissored[2] = 255;
	}
	else
	{
		s_viewportCutoff[2] = 0;
	}

	if(s_viewportScissored[3] < 0)
	{
		s_viewportCutoff[3] = -s_viewportScissored[3];
		s_viewportScissored[3] = 0;
	}
	else if(s_viewportScissored[3] > 191)
	{
		s_viewportCutoff[3] = s_viewportScissored[3] - 191;
		s_viewportScissored[3] = 191;
	}
	else
	{
		s_viewportCutoff[3] = 0;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(s_viewportScissored[0], s_viewportScissored[1], s_viewportScissored[2], s_viewportScissored[3]);
	gluPickMatrix(centerX + ((s_viewportCutoff[0] - s_viewportCutoff[2]) / 2), 191 - centerY + ((s_viewportCutoff[1] - s_viewportCutoff[3]) / 2), width - s_viewportCutoff[0] - s_viewportCutoff[2], height - s_viewportCutoff[1] - s_viewportCutoff[3], (int*)c_viewportDimensions);

	glMultMatrix4x4(&savedProjectionMatrix);
	glMatrixMode(GL_MODELVIEW);
}
