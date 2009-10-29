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

class Sampler
{
public:
	Sampler();
	~Sampler();

	void setNrrd(Nrrd* nin);

	void update();

	double sample(double ix, double iy, double iz);

private:
	Nrrd* nin;

	// Gage stuff
	gageContext *ctx;
	gagePerVolume *pvl;
	const double* result;
};

#endif /* SAMPLER_H_ */
