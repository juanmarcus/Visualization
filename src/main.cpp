/*
 * main.cpp
 *
 *  Created on: Oct 2, 2009
 *      Author: juanmarcus
 */

#include <teem/nrrd.h>
#include <teem/hoover.h>
#include <teem/mite.h>
#include <teem/limn.h>

#include <string>

using namespace std;

int main(int argc, char **argv)
{
	char *err;
	char *me = "main";
	string fileName = "data/A-spgr-deface.nhdr";

	// Array to hold stuff to destroy
	airArray *mop;
	mop = airMopNew();

	// Create the structure
	Nrrd* nin = nrrdNew();

	// Load the input nrrd to nin
	if (nrrdLoad(nin, fileName.c_str(), NULL))
	{
		char* err = biffGetDone(NRRD);
		fprintf(stderr, "%s: trouble reading \"%s\" data:\n%s", me,
				fileName.c_str(), err);
		free(err);
		//precisa nukar o nrrd aqui?
		return 1;
	}

	// Load the transfers functions
	Nrrd* ntxf[1];
	Nrrd* txf = nrrdNew();
	// Load the tranfer function
	if (nrrdLoad(txf, "data/txf-step.nhdr", NULL))
	{
		err = biffGetDone(NRRD);
		fprintf(stderr, "%s: trouble reading \"%s\" data:\n%s", me,
				fileName.c_str(), err);
		free(err);
		//precisa nukar o nrrd aqui?
		return 1;
	}
	ntxf[0] = txf;

	// Create rendering parameters
	miteUser* muu = miteUserNew();
	airMopAdd(mop, muu, (airMopper) miteUserNix, airMopAlways);

	// Set callback functions
	muu->hctx->user = muu;
	muu->hctx->renderBegin = (hooverRenderBegin_t *) miteRenderBegin;
	muu->hctx->threadBegin = (hooverThreadBegin_t *) miteThreadBegin;
	muu->hctx->rayBegin = (hooverRayBegin_t *) miteRayBegin;
	muu->hctx->sample = (hooverSample_t *) miteSample;
	muu->hctx->rayEnd = (hooverRayEnd_t *) miteRayEnd;
	muu->hctx->threadEnd = (hooverThreadEnd_t *) miteThreadEnd;
	muu->hctx->renderEnd = (hooverRenderEnd_t *) miteRenderEnd;

	// Set parameters
	ELL_3V_SET(muu->lit->amb, 1, 1, 1);
	ELL_3V_SET(muu->lit->col[0], 1, 1, 1);
	ELL_3V_SET(muu->lit->dir[0], 0, 0, -1);
	muu->lit->on[0] = AIR_TRUE;
	muu->lit->vsp[0] = AIR_TRUE;
	strncpy(muu->shadeStr, "phong:gage(scalar:n)", AIR_STRLEN_MED - 1);
	strncpy(muu->normalStr, "", AIR_STRLEN_MED - 1);
	muu->shadeStr[AIR_STRLEN_MED - 1] = 0;

	muu->rayStep = 0.01;

	muu->hctx->imgSize[0] = 256;
	muu->hctx->imgSize[1] = 256;

	muu->hctx->cam->from[0] = 3;
	muu->hctx->cam->from[1] = -8.1;
	muu->hctx->cam->from[2] = 7.2;

	muu->hctx->cam->at[0] = 0;
	muu->hctx->cam->at[1] = 0;
	muu->hctx->cam->at[2] = 0;

	muu->hctx->cam->up[0] = 0;
	muu->hctx->cam->up[1] = 0;
	muu->hctx->cam->up[2] = 1;

	muu->hctx->cam->atRelative = 1;
	muu->hctx->cam->neer = -2;
	muu->hctx->cam->faar = 2;
	muu->hctx->cam->dist = 0;

	muu->hctx->cam->fov = 20.0;

	muu->nsin = nin;

	muu->hctx->volSize[0] = nin->axis[0].size;
	muu->hctx->volSize[1] = nin->axis[1].size;
	muu->hctx->volSize[2] = nin->axis[2].size;

	muu->ntxfNum = 1;
	muu->ntxf = ntxf;

	muu->nout = nrrdNew();
	airMopAdd(mop, muu->nout, (airMopper) nrrdNuke, airMopAlways);

	muu->hctx->numThreads = 1;

	NrrdKernelSpec* kernelSpec00 = nrrdKernelSpecNew();
	NrrdKernelSpec* kernelSpec11 = nrrdKernelSpecNew();
	NrrdKernelSpec* kernelSpec22 = nrrdKernelSpecNew();
	if (nrrdKernelSpecParse(kernelSpec00, "tent") || nrrdKernelSpecParse(
			kernelSpec11, "cubicd:1,0") || nrrdKernelSpecParse(kernelSpec22,
			"cubicdd:1,0"))
	{
		airMopAdd(mop, err = biffGetDone(NRRD), airFree, airMopAlways);
		fprintf(stderr, "%s: trouble parsing kernel:\n%s\n", me, err);
		airMopError(mop);
		return 1;
	}

	muu->ksp[gageKernel00] = kernelSpec00;
	muu->ksp[gageKernel11] = kernelSpec11;
	muu->ksp[gageKernel22] = kernelSpec22;

	double v[NRRD_SPACE_DIM_MAX];
	nrrdSpacingCalculate(nin, 0, &(muu->hctx->volSpacing[0]), v);
	nrrdSpacingCalculate(nin, 1, &(muu->hctx->volSpacing[1]), v);
	nrrdSpacingCalculate(nin, 2, &(muu->hctx->volSpacing[2]), v);

	if (limnCameraAspectSet(muu->hctx->cam, muu->hctx->imgSize[0],
			muu->hctx->imgSize[1], nrrdCenterCell) || limnCameraUpdate(
			muu->hctx->cam) || limnLightUpdate(muu->lit, muu->hctx->cam))
	{
		airMopAdd(mop, err = biffGetDone(LIMN), airFree, airMopAlways);
		fprintf(stderr, "%s: trouble setting camera:\n%s\n", me, err);
		airMopError(mop);
		return 1;
	}

	// Render
	printf("before\n");
	int E, Ecode, Ethread;
	E = hooverRender(muu->hctx, &Ecode, &Ethread);
	printf("after\n");
	if (E)
	{
		if (hooverErrInit == E)
		{
			err = biffGetDone(HOOVER);
		}
		else
		{
			err = biffGetDone(MITE);
		}
		airMopAdd(mop, err, airFree, airMopAlways);
		fprintf(stderr, "%s: %s error (code %d, thread %d):\n%s\n", me,
				airEnumStr(hooverErr, E), Ecode, Ethread, err);
		airMopError(mop);
		return 1;
	}

	if (nrrdSave("data/name.nhdr", muu->nout, NULL))
	{
		err = biffGetDone(NRRD);
		fprintf(stderr, "%s: trouble saving \"%s\" data:\n%s", me, "name.nhdr",
				err);
		free(err);
		return 1;
	}

	// Destroy stuff
	airMopOkay(mop);
	return 0;
}
