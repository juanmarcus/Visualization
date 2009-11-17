/*
 * Sampler.cpp
 *
 *  Created on: Oct 29, 2009
 *      Author: Juan Ibiapina
 */

#include "Sampler.h"

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
	char me[] = "Sampler";

	double kparm[NRRD_KERNEL_PARMS_NUM];
	int E;

	if (gageKindVolumeCheck(gageKindScl, nin))
	{
		char* err = biffGetDone(GAGE);
		//		fprintf(stderr, "%s: didn't get a %s volume:\n%s\n", me,
		//				gageKindScl->name, err);
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
}

double Sampler::sample(double ix, double iy, double iz)
{
	if (gageProbe(ctx, ix, iy, iz))
	{
		fprintf(stderr, "%s: trouble:\n(%d) %s\n", "sample", ctx->errNum,
				ctx->errStr);
		return 0;
	}

	//	printf("the gradient is (%g,%g,%g)\n", result[0], result[1], result[2]);
	return result[0];
}
