/*
 * Sampler.cpp
 *
 *  Created on: Oct 29, 2009
 *      Author: Juan Ibiapina
 */

#include "Sampler.h"
#include <iostream>

using namespace ibi;

Sampler::Sampler()
{
	ctx = 0;
	pvl = 0;
}

Sampler::~Sampler()
{
	if (ctx)
	{
		ctx = gageContextNix(ctx);
	}
	pvl = NULL;
}

void Sampler::setNrrd(Nrrd* nin)
{
	this->nin = nin;
}

void Sampler::update()
{
	double kparm[NRRD_KERNEL_PARMS_NUM];
	int E;

	if (gageKindVolumeCheck(gageKindScl, nin))
	{
		char* err = biffGetDone(GAGE);
		throw Exception("Sampler.cpp", "Problem setting up sampler.", err);
	}

	kparm[0] = 1.0; /* scale parameter, in units of samples */
	//	kparm[1] = 0.0; /* B */
	//	kparm[2] = 0.5; /* C */

	ctx = gageContextNew();
	E = 0;
	if (!E)
		E |= !(pvl = gagePerVolumeNew(ctx, nin, gageKindScl));
	if (!E)
		E |= gagePerVolumeAttach(ctx, pvl);
	if (!E)
		E |= gageKernelSet(ctx, gageKernel00, nrrdKernelTent, kparm);
	//	if (!E)
	//		E |= gageKernelSet(ctx, gageKernel11, nrrdKernelBCCubicD, kparm);
	if (!E)
		E |= gageQueryItemOn(ctx, pvl, gageSclValue);
	if (!E)
		E |= gageUpdate(ctx);
	if (E)
	{
		char* err = biffGetDone(GAGE);
		//		fprintf(stderr, "%s: trouble:\n%s\n", me, err);
		throw Exception("Sampler.cpp", "Problem setting up sampler.", err);
	}
	result = gageAnswerPointer(ctx, pvl, gageSclValue);

	/* **** bounds checking **** */
	top[0] = ctx->shape->size[0] - 1;
	top[1] = ctx->shape->size[1] - 1;
	top[2] = ctx->shape->size[2] - 1;
	if (nrrdCenterNode == ctx->shape->center)
	{
		min = 0;
		max[0] = top[0];
		max[1] = top[1];
		max[2] = top[2];
	}
	else
	{
		min = -0.5;
		max[0] = top[0] + 0.5;
		max[1] = top[1] + 0.5;
		max[2] = top[2] + 0.5;
	}

	size[0] = max[0] - min;
	size[1] = max[1] - min;
	size[2] = max[2] - min;

}

double Sampler::sample(double nx, double ny, double nz)
{
	// Calculate index space indices
	double ix, iy, iz;
	ix = nx * size[0] + min;
	iy = ny * size[1] + min;
	iz = nz * size[2] + min;

	if (gageProbe(ctx, ix, iy, iz))
	{
		fprintf(stderr, "%s: trouble:\n(%d) %s\n", "sample", ctx->errNum,
				ctx->errStr);
		return 0;
	}

	//	printf("the gradient is (%g,%g,%g)\n", result[0], result[1], result[2]);
	return result[0];
}
