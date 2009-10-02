/*
 * main.cpp
 *
 *  Created on: Oct 2, 2009
 *      Author: juanmarcus
 */

#include <teem/nrrd.h>

int main(int argc, char **argv) {
	char *err;
	char *me;
	string fileName = "data/helix.nhdr";

	// Array to hold stuff to destroy
	airArray *mop;
	mop = airMopNew();

	// Create the structure
	Nrrd* nin = nrrdNew();

	// Load the input nrrd to nin
	if (nrrdLoad(nin, fileName.c_str(), NULL))
	{
		char* err = biffGetDone(NRRD);
		fprintf(stderr, "%s: trouble reading \"%s\" data:\n%s", "Dataset",
				fileName.c_str(), err);
		free(err);
		//precisa nukar o nrrd aqui?
		return 1;
	}

	// Create the output nrrd in nout
	Nrrd* nout = nrrdNew();
	airMopAdd(mop, nout, (airMopper) nrrdNuke, airMopAlways);

	// Slice the nrrd
	if (nrrdSlice(nout, nin, 0, 0))
	{
		airMopAdd(mop, err = biffGetDone(NRRD), airFree, airMopAlways);
		fprintf(stderr, "%s: error slicing nrrd:\n%s", me, err);
		airMopError(mop);
		return 1;
	}

	// Save the result
	nrrdSave("result.nhdr", nout, NULL);

	// Destroy stuff
	airMopOkay(mop);

	return 0;
}
