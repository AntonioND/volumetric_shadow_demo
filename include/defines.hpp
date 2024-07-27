//------------------------------------------------------------------------------
//
//	by RHL (robs-basement.de) 2007 - 2008
//
//	-> PLEASE READ THE README! <-
//
//------------------------------------------------------------------------------


#ifndef __DEFINES_HPP
#define __DEFINES_HPP


/**
	@file defines.hpp
	@brief Contains some common definitions.
*/


// Define some brightness register mode numbers... weather brightness should be in- or decreased.
#define BRIGHTNESS_INC BIT(14)
#define BRIGHTNESS_DEC BIT(15);

#define min(a, b)	(((a) < (b)) ? (a) : (b))
#define max(a, b)	(((a) > (b)) ? (a) : (b))

// some Return Status Codes (success, generic error, file not found, out of memory)
#define RSC_SUCCESS		0
#define RSC_ERROR		1
#define RSC_ERROR_FNF	13
#define RSC_ERROR_OOM	666

/// This will "trap" execution in an endless loop - use when debugging and no debugger is at hand (requires recompilation for every change) or when needed otherwise (e.g. resource not found.)
#define _DEBUG_HALT		while(true) { swiWaitForVBlank(); }

#define _MAX_PATH_LENGTH	512


extern const int c_viewportDimensions[];

extern const char *c_dataPath;
extern const uint8 c_dataPathLength;


#endif
