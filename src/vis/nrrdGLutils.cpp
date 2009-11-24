/*
 * nrrdGLutils.cpp
 *
 *  Created on: Nov 24, 2009
 *      Author: Juan Ibiapina
 */

#include "nrrdGLutils.h"

#include <teem/nrrd.h>

GLenum convert_type_to_enum(int type)
{
	GLenum result;
	switch (type)
	{
		case nrrdTypeUChar:
		result = GL_UNSIGNED_BYTE;
		break;
		case nrrdTypeChar:
		result = GL_BYTE;
		break;
		case nrrdTypeUShort:
		result = GL_UNSIGNED_SHORT;
		break;
		case nrrdTypeShort:
		result = GL_SHORT;
		break;
		case nrrdTypeFloat:
		result = GL_FLOAT;
		break;
		case nrrdTypeDouble:
		result = GL_DOUBLE;
		break;
		default:
		result = GL_UNSIGNED_BYTE;
		break;
	}
	return result;
}
