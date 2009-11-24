/*
 * TextureConfigurator_Nrrd.h
 *
 *  Created on: Nov 24, 2009
 *      Author: Juan Ibiapina
 */

#ifndef TEXTURECONFIGURATOR_NRRD_H_
#define TEXTURECONFIGURATOR_NRRD_H_

#include "ibi.h"
#include "ibi_gl/TextureLoadingInfo.h"
#include <teem/nrrd.h>
#include "nrrdGLutils.h"

using namespace ibi;

class TextureConfigurator_Nrrd
{
public:
	static inline TextureLoadingInfo fromNrrd2D(Nrrd* nin)
	{
		TextureLoadingInfo info;

		// Allocate and fill memory (upside down)
		int width = nin->axis[0].size;
		int height = nin->axis[1].size;
		int elemsize = nrrdElementSize(nin);
		char* tmpdata = new char[width * height * elemsize];

		char* src = (char*) nin->data;
		for (int i = 0; i < height; ++i)
		{
			int linesize = width * elemsize;
			memcpy(&tmpdata[(height - i - 1) * linesize], &src[i * linesize],
					linesize);
		}

		info.width = width;
		info.height = height;
		info.internalformat = 1;
		info.format = GL_LUMINANCE;
		info.type = convert_type_to_enum(nin->type);
		info.data = tmpdata;
		info.destroydata = true;

		return info;
	}

	static inline TextureLoadingInfo fromNrrd3D(Nrrd* nin)
	{
		TextureLoadingInfo info;

		int width = nin->axis[0].size;
		int height = nin->axis[1].size;
		int depth = nin->axis[2].size;
		//		int elemsize = nrrdElementSize(nin);
		//		char* tmpdata = new char[width * height * elemsize];
		//
		//		char* src = (char*) nin->data;
		//		for (int i = 0; i < height; ++i)
		//		{
		//			int linesize = width * elemsize;
		//			memcpy(&tmpdata[(height - i - 1) * linesize], &src[i * linesize],
		//					linesize);
		//		}

		info.target = GL_TEXTURE_3D;
		info.dim = 3;
		info.width = width;
		info.height = height;
		info.depth = depth;
		info.internalformat = GL_ALPHA;
		info.format = GL_ALPHA;
		info.type = convert_type_to_enum(nin->type);
		info.data = nin->data;

		return info;
	}
};

#endif /* TEXTURECONFIGURATOR_NRRD_H_ */
