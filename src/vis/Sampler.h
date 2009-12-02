/*
 * Sampler.h
 *
 *  Created on: Oct 29, 2009
 *      Author: Juan Ibiapina
 */

#ifndef SAMPLER_H_
#define SAMPLER_H_

#include <teem/nrrd.h>
#include <teem/gage.h>
#include "ibi_error/Exception.h"

using namespace ibi;

class Sampler
{
public:
	Sampler();
	~Sampler();

	void setNrrd(Nrrd* nin);

	void update();

	double sample(double nx, double ny, double nz);

private:
	Nrrd* nin;

	// Gage stuff
	gageContext *ctx;
	gagePerVolume *pvl;
	const double* result;
	unsigned int top[3];
	double min, max[3], size[3];
};

#endif /* SAMPLER_H_ */
